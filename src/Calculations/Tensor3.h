#include "../configSym.h"
#include <vector>
#include <sstream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <utility>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logging/logger.h"

//i might put this in tensors.cpp or something
namespace tensorMath {
    using matrix = std::vector<std::vector<expression>>;

    // Copy of m with the given row and column removed
    inline matrix minorMatrix(const matrix &m, size_t row, size_t col) {
        const size_t n = m.size();
        matrix out;
        out.reserve(n - 1);
        for (size_t i = 0; i < n; i++) {
            if (i == row) continue;
            std::vector<expression> r;
            r.reserve(n - 1);
            for (size_t j = 0; j < n; j++) {
                if (j == col) continue;
                r.push_back(m[i][j]);
            }
            out.push_back(std::move(r));
        }
        return out;
    }

    // Determinant by Laplace expansion along the first row
    inline expression determinant(const matrix &m) {
        const size_t n = m.size();
        if (n == 1) return m[0][0];
        if (n == 2) {
            return SymEngine::sub(SymEngine::mul(m[0][0], m[1][1]),
                                  SymEngine::mul(m[0][1], m[1][0]));
        }

        expression det = SymEngine::zero;
        for (size_t col = 0; col < n; col++) {
            // Skipping zero entries makes diagonal/sparse metrics much cheaper
            if (SymEngine::eq(*m[0][col], *SymEngine::zero)) continue;

            expression term = SymEngine::mul(m[0][col],
                                             determinant(minorMatrix(m, 0, col)));
            det = (col % 2 == 0) ? SymEngine::add(det, term)
                                 : SymEngine::sub(det, term);
        }
        return SymEngine::expand(det);
    }

    // Inverse = adjugate / determinant, where adj[i][j] = (-1)^(i+j) * minor(j, i)
    inline matrix inverse(const matrix &m, const std::string &label) {
        const size_t n = m.size();
        expression det = determinant(m);

        if (SymEngine::eq(*det, *SymEngine::zero)) {
            throw std::runtime_error("Tensor '" + label + "' is singular (det = 0); cannot invert.");
        }

        matrix inv(n, std::vector<expression>(n));
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                expression cof = determinant(minorMatrix(m, j, i)); // note the (j, i) swap
                if ((i + j) % 2 == 1) cof = SymEngine::neg(cof);
                inv[i][j] = SymEngine::expand(SymEngine::div(cof, det));
            }
        }
        return inv;
    }
}

using expression = SymEngine::RCP<const SymEngine::Basic>;
using variable = SymEngine::RCP<const SymEngine::Symbol>;
class Tensor3 {
    //number of covariant and contravariant indexes. Lowkey might be useless in the scope of this project but I'm putting them here just in case.
    unsigned short CoV, CtrV;
    //type of tensor. Metric, Christoffel, Ricci, etc.
    std::string name;
    std::string coordinateSystem;

    //simplifies main trig identity, which not covered by SymEngine Library
    expression trigSimplifyPyth(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }


public:
    //creates the tensor and seeds with 0's
    expression entries[3][3];
    Tensor3(std::string name, std::string coordinateSystem, short Co, short Ctr) {
        this->name = name;
        this->coordinateSystem = coordinateSystem;
        CoV = Co;
        CtrV = Ctr;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                entries[i][j] = SymEngine::integer(0);
            }
        }
    }

    Tensor3 getInverse() const {
        tensorMath::matrix m(3, std::vector<expression>(3));
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = entries[i][j];

        tensorMath::matrix invM = tensorMath::inverse(m, name);

        // Inverting swaps covariant <-> contravariant (g_ij -> g^ij)
        Tensor3 inv("Inverse " + name, coordinateSystem, CtrV, CoV);
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                inv.entries[i][j] = invM[i][j];

        return inv;
    }

    //SUPPORTS COVARIANT TENSORS ONLY UNTIL I GET WORKING IMPLICIT FUNCTIONS
    Tensor3& convertCoordinates(std::string coordinateSystem, variable &C1, variable &C2, variable &C3, expression &V1, expression &V2, expression &V3) {
        expression expr[3] {V1, V2, V3};
        variable vars[3] {C1, C2, C3};

        //creates tensor Jacobian
        expression J[3][3];
        for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 3; i++) {
                J[k][i] = expr[k]->diff(vars[i]);
            }
        }
        //store old entries for future use
        expression oldEntries[3][3];
        for (int k = 0; k < 3; k++) {
            for (int m = 0; m < 3; m++) {
                oldEntries[k][m] = entries[k][m];
            }
        }

        //calculate indexes
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                expression sum = SymEngine::zero;
                for (int k = 0; k < 3; k++)
                    for (int m = 0; m < 3; m++)
                        sum = SymEngine::add(sum,
                              SymEngine::mul(SymEngine::mul(J[k][i], J[m][j]), oldEntries[k][m]));
                //simplifies normally
                entries[i][j] = SymEngine::expand(sum);
                //simplifies trig. ur basically gonna have to hard code any identity for all variables
                entries[i][j] = trigSimplifyPyth(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPyth(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPyth(entries[i][j], vars[2]);
            }
        }

        //sets new coordinate system and renamed if necessary
        this->coordinateSystem = coordinateSystem;
        if (this->name == "Kronecker Delta") {
            this->name = "Metric";
        }

        CoV = 2;
        CtrV = 0;

        return *this;
    }

    std::array<Tensor3*, 3> getChristoffelSymbols(Tensor3 &metric, variable &C1, variable &C2, variable &C3) {
        variable vars[3]{C1, C2, C3};
        Tensor3 inv = metric.getInverse();

        //creates an array of symbols
        std::array<Tensor3*, 3> symbols{
            new Tensor3("Christoffel Symbol 1", coordinateSystem, CoV, CtrV),
            new Tensor3("Christoffel Symbol 2", coordinateSystem, CoV, CtrV),
            new Tensor3("Christoffel Symbol 3", coordinateSystem, CoV, CtrV)
        };

        for (int k = 0; k < 3; k++) {           //symbol number
            for (int m = 0; m < 3; m++) {       //inner index
                for (int v = 0; v < 3; v++) {   //outer index
                    SymEngine::RCP<const SymEngine::Basic> sum = SymEngine::zero;
                    for (int a = 0; a < 3; a++) {       //dummy summation index
                        auto term = SymEngine::mul(
                            inv.entries[k][a],
                            SymEngine::sub(
                                SymEngine::add(metric.entries[a][v]->diff(vars[m]),
                                               metric.entries[a][m]->diff(vars[v])),
                                metric.entries[m][v]->diff(vars[a])));
                        sum = SymEngine::add(sum, term);
                    }
                    symbols[k]->entries[m][v] = SymEngine::div(sum, SymEngine::integer(2));
                }
            }
        }
        return symbols;
    }

    void printTensor() {
        std::stringstream ss;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ss << *entries[i][j] << '\t';
            }
            ss << '\n';
        }
        logConsole(ss);
    }

    ~Tensor3() {    }

};

