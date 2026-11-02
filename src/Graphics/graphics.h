#pragma once


#ifndef GRAPHICS_H
#define GRAPHICS_H

#endif //GRAPHICS_H

//Just tells compiler that this thing exists cause cpp is stupid like that
struct GLFWwindow;



inline unsigned int shaderProgram,VBO,VAO,EBO;
inline glm::vec3 cameraPos,cameraDirection,cameraUp;

int setUpVertices(glm::vec3 objectPos);
void onResize(GLFWwindow* window, int width, int height);
void handleInput(GLFWwindow* window);