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

void runSimulationLoop(GLFWwindow* window);
void onResize(GLFWwindow* window, int width, int height);