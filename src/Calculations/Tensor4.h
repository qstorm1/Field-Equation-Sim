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
class Tensor4 {
    //number of covariant and contravariant indexes. Lowkey might be useless in the scope of this project but I'm putting them here just in case.
    unsigned short CoV, CtrV;
    //type of tensor. Metric, Christoffel, Ricci, etc.
    std::string name;
    std::string coordinateSystem;
    glm::mat4 entries;

public:
    Tensor4(glm::mat4 &seed, std::string name, std::string coordinateSystem, short Co, short Ctr) {
        this->entries = seed;
        this->name = name;
        this->coordinateSystem = coordinateSystem;
        CoV = Co;
        CtrV = Ctr;
    }

    void convertCoordinates(expression &V1, expression &V2, expression &V3, expression &V4) {

    }

    //returns the matrix representation of the tensor
    glm::mat4 toMatrix() {
        return entries;
    }

    ~Tensor4(){}

};