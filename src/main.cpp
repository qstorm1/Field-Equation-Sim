#include "config.h"

//test symbol library   DOUBLE CHECK EVERYTHING WORKS ON LINUX
//this library has random seg faults for whatever the fuck reason idk
#include "configSym.h"

using expression = SymEngine::RCP<const SymEngine::Basic>;
using variable = SymEngine::RCP<const SymEngine::Symbol>;


int main() {

    GLFWwindow* window= nullptr;
    try
    {
        //creates a window

        //check if the library is initialized
        if (!glfwInit())
        {
            std::string errorMessage = "Window Failed to Launch";
            logErrorString(errorMessage);
            return 50;
        }

        //settings for window (pre init)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



        //set the window
        window = glfwCreateWindow(GlobalVariables::initWidth, GlobalVariables::initHeight, "Hello World", nullptr, nullptr);
        //if there was an error in window creation, terminate all processes
        if (!window)
        {
            std::string errorMessage = "Window Failed to Launch";
            logErrorString(errorMessage);
            glfwTerminate();
            return 50;
        }
    }
    catch (const std::exception& e)
    {
        logError(e);
        return 50;
    }

    //from what I understand, this loads the openGL context onto the glfw window so the window can do glfw stuff
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        return 50;
    }



    //Make the starting size of the window
    glViewport(0, 0, GlobalVariables::initWidth, GlobalVariables::initHeight);

    //Run the onResize() llambda whenever the window is resized
    glfwSetFramebufferSizeCallback(window,onResize);


    //the position the camera is at
    cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //the direction vector of the camera
    cameraDirection = glm::vec3(0.0f,0.0f,-1.0f);
    //the vector pointing up from the camera
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



    //object used for testing
    object3d testObj = {0, 0.5, 0, 2, 0.1, "test object M"};
    object3d testObj2 = {2.0, 0, -3, 2, 0.1, "test object N"};


    //Set up the shaders (the code that is run on the graphics card) and link them to openGL
    //this is a shader, a tool responsible for dictating how the graphics card handles inputted information.
    //The first file is the vertex shader, the second file is the fragment shader. More explanation in the constructor
    shader currentShade(SHADER_DIR "vertex-shader", SHADER_DIR "fragment-shader");

    //set up graphics card communication things
    setUpVertices(object3d::getAllObjectsPosition().front());




    //the loop that runs so long as the window is open
    while (!glfwWindowShouldClose(window))
    {
        //the simulation loop, everything repeated check happens in here
        runSimulationLoop(window,&currentShade);

        glfwSwapBuffers(window);//swap the old buffers with updated one's (like change the monitor data)
        glfwPollEvents();//handle IO events
    }


    //de-allocate remaining processes
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    //deletes all objects after window closes for test objects
    object3d::deleteAllObjects();






    //panda tests




    //try catch block to log everything that errors
    try {


        //testing Tensors



        //Kroneker Delta initialization test
        Tensor2 K2("Kronecker Delta", "Cartesian", 1, 1);
        K2.entries[0][0] = SymEngine::integer(1);
        K2.entries[1][1] = SymEngine::integer(1);

        //create conversion functions
        auto rho = SymEngine::symbol("r");
        auto theta = SymEngine::symbol("t");
        expression xPolar = SymEngine::mul(rho, SymEngine::cos(theta));
        expression yPolar = SymEngine::mul(rho, SymEngine::sin(theta));
        //test to see if it works
        std::cout << *xPolar << '\n' << *yPolar << '\n';
        K2.printTensor();

        //test to see if conversion works
        Tensor2 PolarMetric = K2.convertCoordinates("Polar", rho, theta, xPolar, yPolar);
        std::cout << "Polar Metric" << '\n';
        PolarMetric.printTensor();

        //test Christoffel symbols
        auto polarSymbols = PolarMetric.getChristoffelSymbol(PolarMetric, rho, theta);
        std::cout << "0th symbol" << '\n';
        polarSymbols[0]->printTensor();
        std::cout << "1st symbol" << '\n';
        polarSymbols[1]->printTensor();


        Tensor3 K3("Kronecker Delta", "Cartesian", 1, 1);
        K3.entries[0][0] = SymEngine::integer(1);
        K3.entries[1][1] = SymEngine::integer(1);
        K3.entries[2][2] = SymEngine::integer(1);

        variable z = SymEngine::symbol("z");
        expression zPolar = z;


        //Tensor3 CylMetric = K3.convertCoordinates("Cylindrical", rho, theta, z, xPolar, yPolar, zPolar);
        //K3.printTensor();

        variable phi = SymEngine::symbol("p");

        expression xSphere = SymEngine::mul(SymEngine::mul(SymEngine::cos(phi), SymEngine::sin(theta)), rho);
        expression ySphere = SymEngine::mul(SymEngine::mul(SymEngine::sin(phi), SymEngine::sin(theta)), rho);
        expression zSphere = SymEngine::mul(rho, SymEngine::cos(theta));

        Tensor3 SphereMetric = K3.convertCoordinates("Spherical", rho, theta, phi, xSphere, ySphere, zSphere);
        std::cout << "Spherical Metric" << '\n';
        SphereMetric.printTensor();





    }catch (const std::out_of_range &e) {
        logError(e);
        return 2;
    }catch(const std::invalid_argument& e) {
        logError(e);
        return 1;
    }catch(std::exception &e) {
        logError(e);
        return -2;
    }catch(...){
        logError();
        return -1;
    }

    return 0;

}





int setUpVertices(glm::vec3 objectPos)
{
    //*many of these comments are @qstorm's interpretation and could possibly be wrong










    //A Vertex Array (VAO) it is the super array that handles all the type's of shader. This specific one is just for triangles

    //assign the specific ID that turns this int into a unique buffer
    glGenVertexArrays(1, &VAO);




    //A vertex buffer (VBO) is a list of vertex's that we then send to the gpu to do it's calculations
    //it is used to manage the graphics cards memory which is able to store large numbers of vertex


    //assign a specific ID that turns this int into a unique buffer
    glGenBuffers(1, &VBO);



    //EBO's are used to handle indices (lines connecting vertexes)
    //glGenBuffers(1, &EBO);


    //tells openGL that we want to use this VAO
    glBindVertexArray(VAO);



    //set openGL's vertex buffer to be VBO
    //GL_ARRAY_BUFFER just means it will send an array as a buffer
    //this means the graphics card will need to deal with an array when it gets the data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);



    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);




    //TODO: normalize these vertices onto screen space
    //currently there are a dummy set of coordinates given the objectPositions vector will return a single object with position (0,0,0)
    //it's coordinates of (0,0,0) will turn into these coordinates (A square centered around whatever the point is) ->

    constexpr float square[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //     {
    //     objectPos.x+0.5f, objectPos.y+0.5f, objectPos.z, // top right
    //     objectPos.x+0.5f, objectPos.y-0.5f, objectPos.z, // bottom right
    //     objectPos.x-0.5f, objectPos.y-0.5f, objectPos.z, // bottom left
    //     objectPos.x-0.5f, objectPos.y+0.5f, objectPos.z  // top left
    // };

    // const unsigned int indices[] = {  // note that we start from 0!
    //     0, 1, 3,  // first Triangle
    //     1, 2, 3   // second Triangle
    // };


    //sends the data that is set in the VBO to the GPU
    //change whatever the GL_ARRAY_BUFFER is (the VBO in the main function) to a buffer with our vertices
    //GL_STATIC_DRAW is a configuration given to the graphics card that says the data(testVertices) will only be set once and used a bunch
    //we will likely switch to GL_DYNAMIC_DRAW because we'll be using objects who's coordinates are moving
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);




    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    //tells openGL how to interpret the value from the VBO
    //VBO definition in setupShaders()
    //we assume the VBO is a long set of memory clumped together
    //the first value is related to vertexShaderSource in graphics.h. it specifies that we want to set the aPos to be whatever the input is
    //the second value states how many items we want for every aPos (x,y,z is 3 items total so 3)
    //the third represents what the type of the items inside the value is (float)
    //the fourth is weather we should normalize the point (ex: screen space = 500-500 , openGL space = 0.5,0.2 (not to scale))
    //the fifth is the distance in memory before making a new aPos
    //the sixth is the offset, meaning the memory distance from the start of the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));

    //Enables the VBO located at index 0 of the VAO (makes the shader we just set turn on)
    glEnableVertexAttribArray(0);


    //unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this is allowed because we already have the VBO saved to the VOA
    glBindVertexArray(0);


    //set wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    glEnable(GL_DEPTH_TEST);

    return 0;
}

void updateCurrentVertices()
{

}

void runRenderLoop(const shader* currentShader)
{

    //this sets the background color used when clearing
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //this clears the color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




    currentShader->use();

    //tells openGL to pull the data from the VAO inputted
    glBindVertexArray(VAO);




    //current time
    const double time = glfwGetTime();
    // float greenness = sin(time)/4.0f+0.5f;
    // int vertexColorLocation = glGetUniformLocation(currentShader->shaderProgram, "primaryColor");
    // glUniform4f(vertexColorLocation, 0.0f, greenness, 0.0f, 1.0f);







    const auto camX = static_cast<float>(sin(time)) ;
    const auto camZ = static_cast<float>(cos(time)) ;
    //Moves all the stuff to where the camera is, this is done using the magic lookAt matrix which I have already done the math for in other projects.
    //The first is the position vector (where the camera is) the second is where the camera is looking at, and the third is where the camera's up direction is.
    auto transView = glm::lookAt(cameraPos,cameraPos+cameraDirection, cameraUp);


    //This makes far things smaller and large things larger. Also normalizes points for us :D
    auto transProjection = glm::perspective(GlobalVariables::cameraFOV, GlobalVariables::aspectRatio, GlobalVariables::zNear,GlobalVariables::zFar);



    //set the matrix values in the shader to be whatever these values are
    glUniformMatrix4fv(currentShader->getUniform("view"),1,GL_FALSE, glm::value_ptr(transView));
    glUniformMatrix4fv(currentShader->getUniform("projection"),1,GL_FALSE, glm::value_ptr(transProjection));





    //loop through each object and render them
    std::vector<glm::vec3> objectPositions = object3d::getAllObjectsPosition();
    for (glm::vec3& objectPos : objectPositions)
    {
        //This matrix is responsible from moving objects at (0,0,0) to their correct positions in 3D space.
        //For instance, if you want to rotate an object around it's center, you need to make sure the object
        //is centered around (0,0,0) before rotating. Then you can move the object to it's position in 3D space.
        auto transLocal = glm::mat4(1.0f);
        //first value = matrix we want to transform, second value = angle change, third value = which axis to transform about
        transLocal = glm::translate(transLocal, objectPos);
        transLocal = glm::rotate(transLocal, glm::radians(static_cast<float>(time)*20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transLocal = glm::rotate(transLocal, glm::radians(static_cast<float>(time)*5.0f), glm::vec3(0.0f, 0.0f, 1.0f));



        //set the matrix values in the shader to be whatever these values are
        glUniformMatrix4fv(currentShader->getUniform("local"),1,GL_FALSE, glm::value_ptr(transLocal));


        //idk what this comment is
        //the first element states where in the current VBO we want to start with and the second states how many we want to draw

        //draw 6 triangles??? this is related to the EBO or whatever it's called.
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        //draw the triangles on the screen
        glDrawArrays(GL_TRIANGLES, 0, 36);



    }









}
void runPhysicsLoop()
{

}

void handleInput(GLFWwindow* window)
{
    //move forwardMoveExcel distance in the camera forward vector
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
        cameraPos+=(GlobalVariables::forwardMoveExcel * cameraDirection);

    //move forwardMoveExcel distance in the camera forward vector
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
        cameraPos-=(GlobalVariables::backwardMoveExcel*cameraDirection);

    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS)
        cameraPos+=(GlobalVariables::yMoveExcel*cameraUp);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS)
        cameraPos-= (GlobalVariables::yMoveExcel*cameraUp);

            //move sideMoveExcel distance in the camera Right vector
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
        cameraPos-=(GlobalVariables::sideMoveExcel*glm::cross(cameraDirection, cameraUp));


    //move sideMoveExcel distance in the camera Right vector
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
        cameraPos+=(GlobalVariables::sideMoveExcel*glm::cross(cameraDirection, cameraUp));



}


void runSimulationLoop(GLFWwindow* window, const shader* currentShader)
{

    handleInput(window);
    runRenderLoop(currentShader);


}



void onResize(GLFWwindow* window,int width, int height)
{
    glViewport(0, 0, width, height);
}

