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


using expression = SymEngine::RCP<const SymEngine::Basic>;
using variable = SymEngine::RCP<const SymEngine::Symbol>;
class Tensor2 {
    //number of covariant and contravariant indexes. Lowkey might be useless in the scope of this project but I'm putting them here just in case.
    unsigned short CoV, CtrV;
    //type of tensor. Metric, Christoffel, Ricci, etc.
    std::string name;
    std::string coordinateSystem;

    //simplifies main trig identity, which not covered by SymEngine Library
    expression trigSimplifyPythSin(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }

    expression trigSimplifyPythCos(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }


public:
    //creates the tensor and seeds with 0's
    expression entries[2][2];
    Tensor2(std::string name, std::string coordinateSystem, unsigned short Co, unsigned short Ctr) {
        this->name = name;
        this->coordinateSystem = coordinateSystem;
        CoV = Co;
        CtrV = Ctr;
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                entries[i][j] = SymEngine::integer(0);
            }
        }
    }


    //calculates inverse tensor
    Tensor2 getInverse() const {
        const expression &a = entries[0][0];
        const expression &b = entries[0][1];
        const expression &c = entries[1][0];
        const expression &d = entries[1][1];

        // det = ad - bc
        expression det = SymEngine::expand(
            SymEngine::sub(SymEngine::mul(a, d), SymEngine::mul(b, c)));

        //throws error if determinant is 0
        if (SymEngine::eq(*det, *SymEngine::zero)) {
            throw std::runtime_error("Tensor '" + name + "' is singular.");
        }

        // Inverting swaps covariant <-> contravariant (g_ij -> g^ij)
        Tensor2 inv("Inverse " + name, coordinateSystem, CtrV, CoV);

        // (1/det) * [ d  -b ]
        //           [ -c  a ]
        inv.entries[0][0] = SymEngine::expand(SymEngine::div(d, det));
        inv.entries[0][1] = SymEngine::expand(SymEngine::div(SymEngine::neg(b), det));
        inv.entries[1][0] = SymEngine::expand(SymEngine::div(SymEngine::neg(c), det));
        inv.entries[1][1] = SymEngine::expand(SymEngine::div(a, det));

        return inv;
    }

    //SUPPORTS COVARIANT TENSORS ONLY UNTIL I GET WORKING IMPLICIT FUNCTIONS
    Tensor2& convertCoordinates(std::string coordinateSystem, variable &C1, variable &C2, expression &V1, expression &V2) {
        expression expr[2] {V1, V2};
        variable vars[2] {C1, C2};

        //creates tensor Jacobian
        expression J[2][2];
        for (int k = 0; k < 2; k++) {
            for (int i = 0; i < 2; i++) {
                J[k][i] = expr[k]->diff(vars[i]);
            }
        }
        //store old entries for future use
        expression oldEntries[2][2];
        for (int k = 0; k < 2; k++) {
            for (int m = 0; m < 2; m++) {
                oldEntries[k][m] = entries[k][m];
            }
        }

        //calculate indexes
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                expression sum = SymEngine::zero;
                for (int k = 0; k < 2; k++)
                    for (int m = 0; m < 2; m++)
                        sum = SymEngine::add(sum,
                              SymEngine::mul(SymEngine::mul(J[k][i], J[m][j]), oldEntries[k][m]));
                //simplifies normally
                entries[i][j] = SymEngine::expand(sum);
                //simplifies trig. ur basically gonna have to hard code any identity for all variables
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[1]);
            }
        }

        this->coordinateSystem = coordinateSystem;
        if (this->name == "Kronecker Delta") {
            this->name = "Metric";
        }

        CoV = 2;
        CtrV = 0;

        return *this;
    }

    //converts from tensor to mat2 with substitution
    glm::mat2 toMat2(variable &C1, variable &C2, double n1, double n2) {
        glm::mat2 m(0.0f);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                SymEngine::map_basic_basic rules;
                rules[C1] = SymEngine::real_double(n1);
                rules[C2] = SymEngine::real_double(n2);
                expression e = entries[i][j]->subs(rules);
                //god dammit column major order
                m[j][i] = static_cast<float>(SymEngine::eval_double(*e));
            }
        }
        return m;
    }

    //returns an array of symbol pointers
    static std::array<Tensor2*, 2> getChristoffelSymbols(Tensor2 &metric, variable &C1, variable &C2) {
        variable vars[2]{C1, C2};
        Tensor2 inv = metric.getInverse();

        //creates an array of symbols
        std::array<Tensor2*, 2> symbols{
            new Tensor2("Christoffel Symbol 1", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor2("Christoffel Symbol 2", metric.coordinateSystem, metric.CoV, metric.CtrV)
        };

        for (int k = 0; k < 2; k++) {           //symbol number
            for (int m = 0; m < 2; m++) {       //inner index
                for (int v = 0; v < 2; v++) {   //outer index
                    expression sum = SymEngine::zero;
                    for (int a = 0; a < 2; a++) {       //dummy summation index
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

    static Tensor2 getRicciTensor(Tensor2 &metric, variable &C1, variable &C2) {
        Tensor2 Ricci("Ricci Curvature tensor", metric.coordinateSystem, metric.CoV, metric.CtrV);
        auto symbols = getChristoffelSymbols(metric, C1, C2);
        variable vars[2]{C1, C2};
        for (int i =0; i <2; i++) {
            for (int j = 0; j < 2; j++) {
                expression sum = SymEngine::integer(0);
                for (int k = 0; k < 2; k++) {
                    auto term = (symbols[k]->entries[i][j])->diff(vars[k]);
                    sum = SymEngine::add(sum, term);
                }
                for (int k = 0; k < 2; k++) {
                    auto term = (symbols[k]->entries[i][k])->diff(vars[j]);
                    sum = SymEngine::sub(sum, term);
                }
                for (int k = 0; k < 2; k++) {
                    for (int m = 0; m < 2; m++) {
                        auto term = SymEngine::mul(symbols[k]->entries[i][j], symbols[m]->entries[k][m]);
                        sum = SymEngine::add(sum, term);
                    }
                }
                for (int k = 0; k < 2; k++) {
                    for (int m = 0; m < 2; m++) {
                        auto term = SymEngine::mul(symbols[k]->entries[i][m], symbols[m]->entries[j][k]);
                        sum = SymEngine::sub(sum, term);
                    }
                }
                Ricci.entries[i][j] = sum;
            }
        }
        return Ricci;
    }

    void printTensor() {
        std::stringstream ss;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                ss << *entries[i][j] << '\t';
            }
            ss << '\n';
        }
        logConsole(ss);
    }

    ~Tensor2() {    }

};