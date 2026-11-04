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
    expression trigSimplifyPyth(const expression &e, const variable &angle) {
        SymEngine::map_basic_basic rules;
        rules[SymEngine::pow(SymEngine::sin(angle), SymEngine::integer(2))] = SymEngine::sub(SymEngine::one,SymEngine::pow(SymEngine::cos(angle), SymEngine::integer(2)));
        return SymEngine::expand(SymEngine::expand(e)->subs(rules));
    }


public:
    expression** entries;
    Tensor2(std::string name, std::string coordinateSystem, short Co, short Ctr) {
        this->name = name;
        this->coordinateSystem = coordinateSystem;
        CoV = Co;
        CtrV = Ctr;
        entries = new expression *[2];
        for (int i = 0; i < 2; i++) {
            entries[i] = new expression[2];
        }
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                entries[i][j] = SymEngine::integer(0);
            }
        }
    }

    //SUPPORTS COVARIANT TENSORS ONLY UNTIL I GET WORKING IMPLICIT FUNCTIONS
    Tensor2& convertCoordinates(variable &C1, variable &C2, expression &V1, expression &V2) {
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
                //simplifies trig
                entries[i][j] = trigSimplifyPyth(entries[i][j], vars[1]);
            }
        }


        return *this;
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