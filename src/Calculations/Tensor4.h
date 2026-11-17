/*
 * Contains all documentation of the tensor classes.
 * Methods exactly the same in Tensor2 or Tensor3 unless otherwise specified in the class
 * Landau-Lifschitz convention used (+ - - -)
 * c is typically not set equal to 1
 * Some helper methods are located in TensorMath.h
 *
 * Sometimes "Stress-Energy-Tensor" is used interchangeably with "Energy-Momentum Tensor:
 * I am trying to keep this consistent thought.
 * EMT is often used as an abbreviation
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
    //might remove these if they become unnecessary
    unsigned short CoV, CtrV;


    std::string name;
    std::string coordinateSystem;

    //double check for built in trig simplification method
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
        //Each index is set to zero at first.
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                entries[i][j] = SymEngine::integer(0);
            }
        }
    }
    //ToDo: default constructor with Minkowski metric

    //gets inverse
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

    //SUPPORTS COVARIANT TENSORS ONLY UNTIL I GET  IMPLICIT FUNCTIONS WORKING IN SYMENGINE
    //converts from one coordinate system to the other
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

                //might remove these if becomes too time expensive
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

        //unnecessary might remove and have rename method
        this->coordinateSystem = newCoordinateSystem;
        if (this->name == "Kronecker Delta") {
            this->name = "Metric";
        }

        CoV = 2;
        CtrV = 0;

        return *this;
    }

    //Converts to a usable mat4 for rendering if necessary
    //TODO: replace with LambdaVisitor
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

    //calculates the average energy momentum tensor over the entire spacetime
    //formula listed in bibliography
    static Tensor4 getEnergyMomentumTensor(const Tensor4& metric) {

        //calculates determinant
        //needs conversion to tensor math matrix
        tensorMath::matrix m(4, std::vector<expression>(4));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = metric.entries[i][j];
            }
        }

        expression gDet = tensorMath::determinant(m);
        expression sqrtNegG = SymEngine::sqrt(SymEngine::neg(gDet));

        //create dummy tensor
        Tensor4 EM("Energy-Momentum Tensor", metric.coordinateSystem, 0, 2);

        //loops for every object a
        for (size_t a = 0; a < world.size(); a++) {
            const auto& obj = world[a];

            //creates "4-vector" of c and coordinate velocities
            //technically speaking not a 4-vector
            const expression V[4] = {
                SymEngine::integer(c),
                SymEngine::real_double(obj->getVx()),
                SymEngine::real_double(obj->getVy()),
                SymEngine::real_double(obj->getVz())
            };

            //calculates numerator for each object
            expression interval = SymEngine::zero;
            for (int alpha = 0; alpha < 4; alpha++) {
                for (int beta = 0; beta < 4; beta++) {
                    interval = SymEngine::add(interval,
                        SymEngine::mul(metric.entries[alpha][beta],
                                       SymEngine::mul(V[alpha], V[beta])));
                }
            }
            //calculates denominator for each object
            expression denominator = SymEngine::mul(sqrtNegG, SymEngine::sqrt(interval));

            //adds each component of the EMT
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

    //calculates Christoffel Symbols of covariant tensors
    static std::array<Tensor4*, 4> getChristoffelSymbols(const Tensor4 &metric, const variable &C1, const variable &C2, const variable &C3, const variable &C4) {
        //seeds array of variables
        variable vars[4]{C1, C2, C3, C4};
        //gets inverse
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

    //calculates covariant Ricci Tensor
    //shortcut formula derivates from Riemann tensor
    static Tensor4 getRicciTensor(const Tensor4 &metric, const variable &C1, const variable &C2, const variable &C3, variable const &C4) {
        Tensor4 Ricci("Ricci Curvature tensor", metric.coordinateSystem, metric.CoV, metric.CtrV);
        //stores Christoffel Symbols and variables in array
        auto symbols = getChristoffelSymbols(metric, C1, C2, C3, C4);
        variable vars[4]{C1, C2, C3, C4};
        for (int i =0; i <4; i++) {     //ith component of tensor
            for (int j = 0; j < 4; j++) {   //jth component of tensor
                expression sum = SymEngine::integer(0);
                for (int k = 0; k < 4; k++) {   //+ derivatives of Gammas
                    auto term = (symbols[k]->entries[i][j])->diff(vars[k]);
                    sum = SymEngine::add(sum, term);
                }
                for (int k = 0; k < 4; k++) {   //- derivatives of Gammas
                    auto term = (symbols[k]->entries[i][k])->diff(vars[j]);
                    sum = SymEngine::sub(sum, term);
                }
                for (int k = 0; k < 4; k++) {
                    for (int m = 0; m < 4; m++) {   //+ squares of Gammas
                        auto term = SymEngine::mul(symbols[k]->entries[i][j], symbols[m]->entries[k][m]);
                        sum = SymEngine::add(sum, term);
                    }
                }
                for (int k = 0; k < 4; k++) {
                    for (int m = 0; m < 4; m++) {   //- squares of Gammas
                        auto term = SymEngine::mul(symbols[k]->entries[i][m], symbols[m]->entries[j][k]);
                        sum = SymEngine::sub(sum, term);
                    }
                }
                Ricci.entries[i][j] = sum;
            }
        }
        return Ricci;
    }

    //calculates Ricci scalar from covariant metric and Ricci tensor
    static expression getRicciScalar(const Tensor4 &metric, variable const &C1, variable const &C2, variable const &C3, variable const &C4) {
        //store variables and get inverse
        Tensor4 Ricci = getRicciTensor(metric, C1, C2, C3, C4);
        Tensor4 inv = metric.getInverse();
        expression sum = SymEngine::integer(0);
        //sums and loops
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                expression term = SymEngine::mul(inv.entries[i][j], Ricci.entries[i][j]);
                sum = SymEngine::add(sum, term);
            }
        }
        return sum;
    }

    //print tensor in Row-Major order
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