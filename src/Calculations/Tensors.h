#pragma once

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

//
class Tensor {

    //number of covariant and contravariant indexes. Lowkey might be useless in the scope of this project but I'm putting them here just in case.
    unsigned short CoV, CtrV;
    //type of tensor. Metric, Christoffel, Ricci, etc.
    std::string name;
    glm::mat4 entries;

public:
    Tensor(glm::mat4 &seed, std::string name, short Co, short Ctr) {
        this->entries = seed;
        this->name = name;
        CoV = Co;
        CtrV = Ctr;
    }

    //returns the matrix representation of the tensor
    glm::mat4 toMatrix() {
        return entries;
    }

    ~Tensor(){}

};

