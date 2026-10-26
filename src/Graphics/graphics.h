#pragma once


#ifndef GRAPHICS_H
#define GRAPHICS_H

#endif //GRAPHICS_H

struct GLFWwindow;

const char *vertextShaderSource = "#version 330 core\n"
    "layout (location=0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);"
    "}\0";

const char *fragShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor=vec4(1.0f,1.0f,1.0f,1.0f);\n"
    "\0";

int setupShaders();
void runSimulationLoop(GLFWwindow* window);
void onResize(GLFWwindow* window, int width, int height);