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


using expression = SymEngine::RCP<const SymEngine::Basic>;
using variable = SymEngine::RCP<const SymEngine::Symbol>;
class Tensor2 {
    //number of covariant and contravariant indexes. Lowkey might be useless in the scope of this project but I'm putting them here just in case.
    unsigned short CoV, CtrV;
    //type of tensor. Metric, Christoffel, Ricci, etc.
    std::string name;
    std::string coordinateSystem;


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
    Tensor2* convertCoordinates(variable &C1, variable &C2, expression &V1, expression &V2) {
        expression expr[2] {V1, V2};
        variable vars[2] {C1, C2};
        auto oldEntries = entries;
        //oh god there has to be a better way then 4 nested four loops  (ba-dum tssss)
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    for (int m = 0; m < 2; m++) {
                        entries[i][j] = SymEngine::mul(SymEngine::mul(expr[k]->diff(vars[i]), expr[m]->diff(vars[j])), oldEntries[k][m]);
                    }
                }
            }
        }
        return this;
    }


    ~Tensor2() {
        //free up 2d array memory
        for(int i = 0; i < 2; i++){
            delete[] entries[i];
            entries[i] = nullptr;
        }

        delete[] entries;
        entries = nullptr;
    }

};