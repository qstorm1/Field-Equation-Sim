#include <cmath>

#pragma once


#ifndef GRAPHICS_H
#define GRAPHICS_H

#endif //GRAPHICS_H

//Just tells compiler that this thing exists cause cpp is stupid like that
struct GLFWwindow;



inline unsigned int shaderProgram,
    sphereVBO,sphereVAO,numbOfTrianglesSphere,//mid-poly
    squareVBO,squareVAO;
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
inline void generateSpherePoint(std::vector<float>* points,const float stepAngleCol,const float stepAngleRow,  const float radius,
    const int i, const int j)
{
    std::vector<float> r =  sphereToCartesian(radius,stepAngleCol*static_cast<float>(j),stepAngleRow*static_cast<float>(i));
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

    const auto stepAngleCol=static_cast<float>(std::numbers::pi/columns);
    const auto stepAngleRow=static_cast<float>(2.0*std::numbers::pi/rows);


    //start at one to prevent 0 division
    for (int i = 0; i<rows; i++)
    {
        for (int j = 0; j<columns; j++)
        {
            //the top and bottom of spheres are 1 triangle
            if (j==0)
            {
                //    i   i+1
                //j     O = i and i+1
                //     | |
                //    |   |
                //   O-----O
                //j+1

                //the top
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j);

                //the bottom left
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j+1);

                //the bottom right
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i+1,j+1);

            }
            else if (j==columns-1)
            {
                //  i     i+1
                //j  O-----O
                //    |   |
                //     | |
                //j+1   O = i and i+1

                //top left
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j);

                //bottom
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j+1);

                //top right
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i+1,j);


            }
            else
            {
                //    i  i+1
                //j   O--O
                //    |  |
                //    |  |
                //    O--O
                //j+1

                //first triangle
                //top left
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j);
                //bottom left
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j+1);
                //bottom right
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i+1,j+1);

                //second triangle
                //top left
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i,j);
                //bottom right
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i+1,j+1);
                //top right
                generateSpherePoint(&result,stepAngleCol,stepAngleRow,radius,i+1,j);
            }
        }
    }
    return result;
}

