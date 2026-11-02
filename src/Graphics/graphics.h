#pragma once


#ifndef GRAPHICS_H
#define GRAPHICS_H

#endif //GRAPHICS_H

//Just tells compiler that this thing exists cause cpp is stupid like that
struct GLFWwindow;



inline unsigned int shaderProgram,VBO,VAO,EBO;

//the location of the camera
inline double camera[] = {0,0,0};

int setUpVertices(const glm::vec3 objectPos);
void onResize(GLFWwindow* window, int width, int height);