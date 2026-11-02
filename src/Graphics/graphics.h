#pragma once


#ifndef GRAPHICS_H
#define GRAPHICS_H

#endif //GRAPHICS_H

struct GLFWwindow;

inline const char *vertexShaderSource = "#version 330 core\n"
    "layout (location=0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);"
    "}\0";

inline const char *fragShaderSource = "#version 330 core\n"
    "uniform vec4 primaryColor;"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor=primaryColor;\n"
    "}\0";
inline unsigned int shaderProgram,VBO,VAO,EBO;

inline double camera[] = {0,0,0};

int setUpVertices();
void onResize(GLFWwindow* window, int width, int height);