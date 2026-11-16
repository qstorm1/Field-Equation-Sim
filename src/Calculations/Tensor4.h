/*
* ALL OF THESE METHODS ARE CLONES FROM TENSOR2 AND ADAPTED FOR HIGHER DIMENSIONS
 * TO VIEW THE FULL DOCUMENTATION FOR EACH METHOD LOOK AT TENSOR 2
 */



#ifndef TENSOR4_H
#define TENSOR4_H


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
#include "../GlobalVariables.h"



class Tensor4 {
    unsigned short CoV, CtrV;
    std::string name;
    std::string coordinateSystem;

    static expression trigSimplifyPythSin(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }

    static expression trigSimplifyPythCos(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }


public:
    expression entries[4][4];
    Tensor4(const std::string &name, const std::string &coordinateSystem, const unsigned short Co, const unsigned short Ctr) {
        this->name = name;
        this->coordinateSystem = coordinateSystem;
        CoV = Co;
        CtrV = Ctr;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                entries[i][j] = SymEngine::integer(0);
            }
        }
    }

    [[nodiscard]] Tensor4 getInverse() const {
        tensorMath::matrix m(4, std::vector<expression>(4));
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = entries[i][j];

        tensorMath::matrix invM = tensorMath::inverse(m, name);

        Tensor4 inv("Inverse " + name, coordinateSystem, CtrV, CoV);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                inv.entries[i][j] = invM[i][j];

        return inv;
    }

    //SUPPORTS COVARIANT TENSORS ONLY UNTIL I GET WORKING IMPLICIT FUNCTIONS
    Tensor4& convertCoordinates(const std::string & newCoordinateSystem, const variable &C1, const variable &C2, const variable &C3, const variable &C4, const expression &V1, const expression &V2, const expression &V3, const expression &V4) {
        expression expr[4] {V1, V2, V3, V4};
        variable vars[4] {C1, C2, C3, C4};

        expression J[4][4];
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < 4; i++) {
                J[k][i] = expr[k]->diff(vars[i]);
            }
        }
        expression oldEntries[4][4];
        for (int k = 0; k < 4; k++) {
            for (int m = 0; m < 4; m++) {
                oldEntries[k][m] = entries[k][m];
            }
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                expression sum = SymEngine::zero;
                for (int k = 0; k < 4; k++)
                    for (int m = 0; m < 4; m++)
                        sum = SymEngine::add(sum,
                              SymEngine::mul(SymEngine::mul(J[k][i], J[m][j]), oldEntries[k][m]));
                entries[i][j] = SymEngine::expand(sum);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[2]);
                entries[i][j] = trigSimplifyPythSin(entries[i][j], vars[3]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[0]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[1]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[2]);
                entries[i][j] = trigSimplifyPythCos(entries[i][j], vars[3]);
            }
        }

        this->coordinateSystem = newCoordinateSystem;
        if (this->name == "Kronecker Delta") {
            this->name = "Metric";
        }

        CoV = 2;
        CtrV = 0;

        return *this;
    }

    [[nodiscard]] glm::mat4 toMat4(const variable &C1, const variable &C2, const variable &C3, const variable &C4,const double n1, const double n2, const double n3, const double n4) const{
        glm::mat4 m(0.0f);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                SymEngine::map_basic_basic rules;
                rules[C1] = SymEngine::real_double(n1);
                rules[C2] = SymEngine::real_double(n2);
                rules[C3] = SymEngine::real_double(n3);
                rules[C4] = SymEngine::real_double(n4);
                expression e = entries[i][j]->subs(rules);
                //god dammit column major order
                m[j][i] = static_cast<float>(SymEngine::eval_double(*e));
            }
        }
        return m;
    }

    static Tensor4 getEnergyMomentumTensor(const Tensor4& metric) {
        tensorMath::matrix m(4, std::vector<expression>(4));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = metric.entries[i][j];
            }
        }

        expression gDet = tensorMath::determinant(m);
        expression sqrtNegG = SymEngine::sqrt(SymEngine::neg(gDet));

        Tensor4 EM("Energy-Momentum Tensor", metric.coordinateSystem, 0, 2);

        for (size_t a = 0; a < world.size(); a++) {
            const auto& obj = world[a];

            const expression V[4] = {
                SymEngine::integer(c),
                SymEngine::real_double(obj->getVx()),
                SymEngine::real_double(obj->getVy()),
                SymEngine::real_double(obj->getVz())
            };

            expression interval = SymEngine::zero;
            for (int alpha = 0; alpha < 4; alpha++) {
                for (int beta = 0; beta < 4; beta++) {
                    interval = SymEngine::add(interval,
                        SymEngine::mul(metric.entries[alpha][beta],
                                       SymEngine::mul(V[alpha], V[beta])));
                }
            }
            expression denominator = SymEngine::mul(sqrtNegG, SymEngine::sqrt(interval));

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    expression numerator = SymEngine::mul(SymEngine::integer(c),
                                               SymEngine::mul(SymEngine::real_double(obj->getMass()),
                                                   SymEngine::mul(V[i], V[j])));

                    EM.entries[i][j] = SymEngine::add(EM.entries[i][j],
                                                      SymEngine::div(numerator, denominator));
                }
            }
        }

        return EM;
    }

    static std::array<Tensor4*, 4> getChristoffelSymbols(const Tensor4 &metric, const variable &C1, const variable &C2, const variable &C3, const variable &C4) {
        variable vars[4]{C1, C2, C3, C4};
        Tensor4 inv = metric.getInverse();

        //creates an array of symbols
        std::array<Tensor4*, 4> symbols{
            new Tensor4("Christoffel Symbol 1", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor4("Christoffel Symbol 2", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor4("Christoffel Symbol 3", metric.coordinateSystem, metric.CoV, metric.CtrV),
            new Tensor4("Christoffel Symbol 4", metric.coordinateSystem, metric.CoV, metric.CtrV)
        };

        for (int k = 0; k < 4; k++) {           //symbol number
            for (int m = 0; m < 4; m++) {       //inner index
                for (int v = 0; v < 4; v++) {   //outer index
                    expression sum = SymEngine::zero;
                    for (int a = 0; a < 4; a++) {       //dummy summation index
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

    static Tensor4 getRicciTensor(const Tensor4 &metric, const variable &C1, const variable &C2, const variable &C3, variable const &C4) {
        Tensor4 Ricci("Ricci Curvature tensor", metric.coordinateSystem, metric.CoV, metric.CtrV);
        auto symbols = getChristoffelSymbols(metric, C1, C2, C3, C4);
        variable vars[4]{C1, C2, C3, C4};
        for (int i =0; i <4; i++) {
            for (int j = 0; j < 4; j++) {
                expression sum = SymEngine::integer(0);
                for (int k = 0; k < 4; k++) {
                    auto term = (symbols[k]->entries[i][j])->diff(vars[k]);
                    sum = SymEngine::add(sum, term);
                }
                for (int k = 0; k < 4; k++) {
                    auto term = (symbols[k]->entries[i][k])->diff(vars[j]);
                    sum = SymEngine::sub(sum, term);
                }
                for (int k = 0; k < 4; k++) {
                    for (int m = 0; m < 4; m++) {
                        auto term = SymEngine::mul(symbols[k]->entries[i][j], symbols[m]->entries[k][m]);
                        sum = SymEngine::add(sum, term);
                    }
                }
                for (int k = 0; k < 4; k++) {
                    for (int m = 0; m < 4; m++) {
                        auto term = SymEngine::mul(symbols[k]->entries[i][m], symbols[m]->entries[j][k]);
                        sum = SymEngine::sub(sum, term);
                    }
                }
                Ricci.entries[i][j] = sum;
            }
        }
        return Ricci;
    }

    static expression getRicciScalar(const Tensor4 &metric, variable const &C1, variable const &C2, variable const &C3, variable const &C4) {
        Tensor4 Ricci = getRicciTensor(metric, C1, C2, C3, C4);
        Tensor4 inv = metric.getInverse();
        expression sum = SymEngine::integer(0);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                expression term = SymEngine::mul(inv.entries[i][j], Ricci.entries[i][j]);
                sum = SymEngine::add(sum, term);
            }
        }
        return sum;
    }

    void printTensor() const{
        std::stringstream ss;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ss << *entries[i][j] << '\t';
            }
            ss << '\n';
        }
        logConsole(ss);
    }

    ~Tensor4() = default;

};

#endif