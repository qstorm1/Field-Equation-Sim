#include "config.h"



int main() {
    //create files to write to
    std::fstream errorLog;

    errorLog.open("../ErrorLog.txt", std::ios::app);




    //TO BE DELETED:

    //{



    //we use a translation matrix instead of just adding because for thousands of objects that are all being translated
    //the same way, it's easier to combine the rotation and translation matrix before adding it
    // //to all objects in one step then repeatedly adding the same vector and then multiplying by the rotation matrix
    //
    // //we will eventually get these vectors from some form of file
    // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // //First, we create the correct translation matrix by taking a normal coordinate system (as in the unit matrix)
    // //and adding some vector to it. This gives us a translation matrix that we can multiply with vec to get vec in a translated coordinate system
    // vec = glm::translate( glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f)) * vec;
    // std::cout << vec.x << vec.y << vec.z << std::endl;
    //
    //
    //
    // // this will move all coordinates pi radians clockwise, scale the object to half it's size, and move the object up 1
    // auto finalTrans = glm::mat4(1.0f);
    // finalTrans = glm::translate(finalTrans,glm::vec3(0.0f,1.0f, 0.0f));
    // //finalTrans = glm::rotate(finalTrans,std::numbers::pi,gkn::vec3))
    // finalTrans = glm::scale(finalTrans,glm::vec3(0.5f, 0.5f, 0.5f));
    //



    //} END DELETE




    GLFWwindow* window= nullptr;
    try
    {
        //creates a window

        //check if the library is initialized
        if (!glfwInit())
        {
            std::string errorMessage = "Window Failed to Launch";
            logErrorString(errorLog, errorMessage);
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
            logErrorString(errorLog, errorMessage);
            glfwTerminate();
            return 50;
        }
    }
    catch (const std::exception& e)
    {
        logError(errorLog, e);
        return 50;
    }

    //from what I understand, this loads the openGL context onto the glfw window so the window can do glfw stuff
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return 50;
    }



    //make the starting size of the window
    glViewport(0, 0, GlobalVariables::initWidth, GlobalVariables::initHeight);

    //run the onResize() llambda whenever the window is resized
    glfwSetFramebufferSizeCallback(window,onResize);


    //object used for testing
    object3d testObj = {0, 0, 0, 2, 4, "test object M"};


    //set up the shaders (the code that is run on the graphics card) and link them to openGL
    //this is a shader, a tool responsable for dictating how the graphics card handles inputed information.
    //The first file is the vertex shader, the second file is the fragment shader. More explanation in the constructor
    shader currentShade(SHADER_DIR "vertex-shader", SHADER_DIR "fragment-shader");

    //set up graphics card communication things
    setUpVertices();




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



    //try catch block to log everything that errors
    try {

        //test objects for overlap checking
        object3d Earth = {0, 0, 0, 5.97*pow(10, 24), 6371000, "Earth"};
        object3d Person = {0, 6371002, 0, 70, 1};


        glm::vec3 testVector = Person.calculateAccelerationField();
        std::stringstream ss;

        ss << testVector.x << ' ' << testVector.y << ' ' << testVector.z << std::endl;
        std::string filepath = "../coutLog.txt";
        logConsole(filepath, ss);

        //OH MY GAWD WE GET LITTLE G IT WORKS



    }catch (const std::out_of_range &e) {
        logError(errorLog, e);
        return 2;
    }catch(const std::invalid_argument& e) {
        logError(errorLog, e);
        return 1;
    }catch(std::exception &e) {
        logError(errorLog, e);
        return -2;
    }catch(...){
        logError(errorLog);
        return -1;
    }

    errorLog.close();
    return 0;

}





int setUpVertices()
{
    //*many of these comments are @qstorm's interpretation and could possibly be wrong






    //the data points
    //TODO: normalize these vertices onto screen space
    constexpr float testVertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left
   };
    const unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };



    //A Vertex Array (VAO) it is the super array that handles all the type's of shader. This specific one is just for triangles

    //assign the specific ID that turns this int into a unique buffer
    glGenVertexArrays(1, &VAO);




    //A vertex buffer (VBO) is a list of vertex's that we then send to the gpu to do it's calculations
    //it is used to manage the graphics cards memory which is able to store large numbers of vertex


    //assign a specific ID that turns this int into a unique buffer
    glGenBuffers(1, &VBO);



    //EBO's are used to handle indices (lines connecting vertexes)
    glGenBuffers(1, &EBO);


    //tells openGL that we want to use this VAO
    glBindVertexArray(VAO);



    //set openGL's vertex buffer to be VBO
    //GL_ARRAY_BUFFER just means it will send an array as a buffer
    //this means the graphics card will need to deal with an array when it gets the data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);







    //sends the data that is set in the VBO to the GPU
    //change whatever the GL_ARRAY_BUFFER is (the VBO in the main function) to a buffer with our vertices
    //GL_STATIC_DRAW is a configuration given to the graphics card that says the data(testVertices) will only be set once and used a bunch
    //we will likely switch to GL_DYNAMIC_DRAW because we'll be using objects who's coordinates are moving
    glBufferData(GL_ARRAY_BUFFER, sizeof(testVertices), testVertices, GL_STATIC_DRAW);



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    //tells openGL how to interpret the value from the VBO
    //VBO definition in setupShaders()
    //we assume the VBO is a long set of memory clumped together
    //the first value is related to vertexShaderSource in graphics.h. it specifies that we want to set the aPos to be whatever the input is
    //the second value states how many items we want for every aPos (x,y,z is 3 items total so 3)
    //the third represents what the type of the items inside the value is (float)
    //the fourth is weather we should normalize the point (ex: screen space = 500-500 , openGL space = 0.5,0.2 (not to scale))
    //the fifth is the distance in memory before making a new aPos
    //the sixth is the offset, meaning the memory distance from the start of the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

    //Enables the VBO located at index 0 of the VAO (makes the shader we just set turn on)
    glEnableVertexAttribArray(0);


    //unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);//this is allowed because we already have the VBO saved to the VOA
    glBindVertexArray(0);


    //set wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    return 0;
}

void runRenderLoop(const shader* currentShader)
{

    //this sets the background color used when clearing
    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    //this clears the color
    glClear(GL_COLOR_BUFFER_BIT);




    float vertices[object3d::getTotalNumbOfObjects()][3];


    currentShader->use();

    const double time = glfwGetTime();
    float greenness = sin(time)/2.0f+0.5f;
    int vertexColorLocation = glGetUniformLocation(currentShader->shaderProgram, "primaryColor");
    glUniform4f(vertexColorLocation, 0.0f, greenness, 0.0f, 1.0f);

    double rotation = sin(time)*20.0;


    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(sin(time)/2, 0.0f, 0.0f));
    //first value = matrix we want to transform, second value = angle change, third value = which axis to transform about
    trans = glm::rotate(trans, static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

    glUniformMatrix4fv(currentShader->getUniform("worldTransform"),1,GL_FALSE, glm::value_ptr(trans));



    //the first element states where in the current VBO we want to start with and the second states how many we want to draw

    //draw 6 triangles???
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


    //tells openGL to pull the data from the VAO inputted
    glBindVertexArray(VAO);

}
void runPhysicsLoop()
{

}
void handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
        camera[0] += GlobalVariables::forwardMoveExcel;

    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
        camera[0] -= GlobalVariables::backwardMoveExcel;

    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS)
        camera[1] += GlobalVariables::yMoveExcel;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS)
        camera[1] -= GlobalVariables::yMoveExcel;

    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
        camera[2] += GlobalVariables::sideMoveExcel;

    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
        camera[2] -= GlobalVariables::sideMoveExcel;

}

void runSimulationLoop(GLFWwindow* window, const shader* currentShader)
{


    runRenderLoop(currentShader);


}



void onResize(GLFWwindow* window,int width, int height)
{
    glViewport(0, 0, width, height);
}

