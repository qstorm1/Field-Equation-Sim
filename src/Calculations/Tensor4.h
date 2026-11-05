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



class Tensor4 {
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
    expression entries[4][4];
    Tensor4(std::string name, std::string coordinateSystem, short Co, short Ctr) {
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

    Tensor4 getInverse() const {
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
    Tensor4& convertCoordinates(std::string coordinateSystem, variable &C1, variable &C2, variable &C3, variable &C4, expression &V1, expression &V2, expression &V3, expression &V4) {
        expression expr[4] {V1, V2, V3, V4};
        variable vars[4] {C1, C2, C3, C4};

        //creates tensor Jacobian
        expression J[4][4];
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < 4; i++) {
                J[k][i] = expr[k]->diff(vars[i]);
            }
        }
        //store old entries for future use
        expression oldEntries[4][4];
        for (int k = 0; k < 4; k++) {
            for (int m = 0; m < 4; m++) {
                oldEntries[k][m] = entries[k][m];
            }
        }

        //calculate indexes
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                expression sum = SymEngine::zero;
                for (int k = 0; k < 4; k++)
                    for (int m = 0; m < 4; m++)
                        sum = SymEngine::add(sum,
                              SymEngine::mul(SymEngine::mul(J[k][i], J[m][j]), oldEntries[k][m]));
                //simplifies normally
                entries[i][j] = SymEngine::expand(sum);
                //simplifies trig. ur basically gonna have to hard code any identity for all variables
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

        //sets new coordinate system and renamed if necessary
        this->coordinateSystem = coordinateSystem;
        if (this->name == "Kronecker Delta") {
            this->name = "Metric";
        }

        CoV = 2;
        CtrV = 0;

        return *this;
    }

    //i'm going to pray this works since I don't really have test cases for it
    std::array<Tensor4*, 4> getChristoffelSymbols(Tensor4 &metric, variable &C1, variable &C2, variable &C3, variable &C4) {
        variable vars[4]{C1, C2, C3, C4};
        Tensor4 inv = metric.getInverse();

        //creates an array of symbols
        std::array<Tensor4*, 4> symbols{
            new Tensor4("Christoffel Symbol 1", coordinateSystem, CoV, CtrV),
            new Tensor4("Christoffel Symbol 2", coordinateSystem, CoV, CtrV),
            new Tensor4("Christoffel Symbol 3", coordinateSystem, CoV, CtrV)
        };

        for (int k = 0; k < 4; k++) {           //symbol number
            for (int m = 0; m < 4; m++) {       //inner index
                for (int v = 0; v < 4; v++) {   //outer index
                    SymEngine::RCP<const SymEngine::Basic> sum = SymEngine::zero;
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

    void printTensor() {
        std::stringstream ss;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ss << *entries[i][j] << '\t';
            }
            ss << '\n';
        }
        logConsole(ss);
    }

    ~Tensor4() {    }

};