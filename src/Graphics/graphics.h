#include <cmath>

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

inline std::vector<float> sphereToCartesian(const float radius, const float theta, const float phi)
{
    return {(radius*std::sin(theta)*std::cos(phi)),(radius*std::sin(theta)*std::sin(phi)),(radius*std::cos(theta))};
}

/**
 * this is just a helper function for the generateSphere thing
 */
inline void generateSphereFromPoint(std::vector<float>* points, const float radius, const int i, const int j)
{
    std::vector<float> r =  sphereToCartesian(radius,static_cast<float>(2*std::numbers::pi/i),
                    static_cast<float>(std::numbers::pi/2-std::numbers::pi/j));
    points->insert(points->end(), r.begin(), r.end());
}

//TODO: return both normals and texture coordinates using pointers in the ()
/**
 * generate a list of vertices that represents a square
 * Read https://songho.ca/opengl/gl_sphere.html for reference
 * (I made most myself tho once I realized there wasn't a magic way of doing it :D)
 * @param radius the radius of the sphere
 * @param rows the number of triangles around the circle
 * @param columns the number of triangles around the circle upwards
 * @return a list of vertices that represent triangle coordinates
 */
inline std::vector<float> generateSphere(const float radius, const int rows,const int columns)
{
    std::vector<float> result;
    std::vector<float> r1;
    //start at one to prevent 0 division
    for (int i = 1; i<=rows; i++)
    {
        for (int j = 1; j<=columns; j++)
        {
            //the top and bottom of spheres are 1 triangle
            if (j==1)
            {
                //the bottom right
                generateSphereFromPoint(&result,radius,i,j);

                //the top
                r1 = sphereToCartesian(radius,0,std::numbers::pi/2.0f);
                result.insert(result.end(),r1.begin(), r1.end());

                //the bottom left
                generateSphereFromPoint(&result,radius,i-1,j);

            }
            else if (j==columns-1)
            {
                //bottom
                r1 = sphereToCartesian(radius,0,std::numbers::pi/2.0f);
                result.insert(result.end(),r1.begin(),r1.end());

                //top right
                generateSphereFromPoint(&result,radius,i,j+1);
                //top left
                r1 = sphereToCartesian(radius,static_cast<float>(2*std::numbers::pi/i),
                    static_cast<float>(std::numbers::pi/2-std::numbers::pi/j));
            }
            else
            {

            }
            //new coordinates:
            //
            // x = cos(phi)*cos(theta)
            // y = cos(phi)*sin(theta)
            // z = r*sin(theta)
            result.push_back(0);
        }
    }
    return result;
}

