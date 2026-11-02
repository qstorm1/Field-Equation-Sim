//
// Created by anahita on 10/31/26.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "glad/glad.h"


#ifndef SHADER_H
#define SHADER_H

class shader{
public:
    int shaderProgram,vertex,fragment;
    shader(const char*, const char*);

    //const means read-only
    void use() const;


};



#endif //SHADER_H
