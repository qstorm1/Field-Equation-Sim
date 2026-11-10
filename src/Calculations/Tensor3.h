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
#include "TensorMath.h"



class Tensor3 {
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
    expression entries[3][3];
    Tensor3(std::string name, std::string coordinateSystem, unsigned short Co, unsigned short Ctr) {
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
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[2]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[2]);
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

    static std::array<Tensor3*, 3> getChristoffelSymbols(Tensor3 &metric, variable &C1, variable &C2, variable &C3) {
        variable vars[3]{C1, C2, C3};
        Tensor3 inv = metric.getInverse();

        //creates an array of symbols
        std::array<Tensor3*, 3> symbols{
            new Tensor3("Christoffel Symbol 1", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor3("Christoffel Symbol 2", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor3("Christoffel Symbol 3", metric.coordinateSystem, metric.CoV, metric.CtrV)
        };

        for (int k = 0; k < 3; k++) {           //symbol number
            for (int m = 0; m < 3; m++) {       //inner index
                for (int v = 0; v < 3; v++) {   //outer index
                    expression sum = SymEngine::zero;
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

    static Tensor3 getRicciTensor(Tensor3 &metric, variable &C1, variable &C2, variable &C3) {
        Tensor3 Ricci("Ricci Curvature tensor", metric.coordinateSystem, metric.CoV, metric.CtrV);
        auto symbols = getChristoffelSymbols(metric, C1, C2, C3);
        variable vars[3]{C1, C2, C3};
        for (int i =0; i <3; i++) {
            for (int j = 0; j < 3; j++) {
                expression sum = SymEngine::integer(0);
                for (int k = 0; k < 3; k++) {
                    auto term = (symbols[k]->entries[i][j])->diff(vars[k]);
                    sum = SymEngine::add(sum, term);
                }
                for (int k = 0; k < 3; k++) {
                    auto term = (symbols[k]->entries[i][k])->diff(vars[j]);
                    sum = SymEngine::sub(sum, term);
                }
                for (int k = 0; k < 3; k++) {
                    for (int m = 0; m < 3; m++) {
                        auto term = SymEngine::mul(symbols[k]->entries[i][j], symbols[m]->entries[k][m]);
                        sum = SymEngine::add(sum, term);
                    }
                }
                for (int k = 0; k < 3; k++) {
                    for (int m = 0; m < 3; m++) {
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

