#include "config.h"
#include "matrix.h"




int main() {
    //create files to write to
    std::fstream errorLog;

    errorLog.open("../ErrorLog.txt", std::ios::app);



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
        window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
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
    if (setupShaders()==512)
    {
        return 512;
    }





    //the loop that runs so long as the window is open
    while (!glfwWindowShouldClose(window))
    {
        runSimulationLoop(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //de-allocate remaining processes
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();



    //try catch block to log everything that errors
    try {

        //test objects for overlap checking
        object3d o = {0, 0, 0, 2, 4, "test object"};
        object3d p = {1, 1, 111, 2, 11};

        //test equals operator
        std::string filepath = "../coutLog.txt";

        std::stringstream ss;
        ss << (o == p) << std::endl;
        logConsole(filepath, ss);

        ss.str("");
        ss << (o == o) << std::endl;
        logConsole(filepath, ss);
        ss.str("");

        p.name = "testing object 2";



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


int setupShaders()
{
    //*many of these comments are @qstorm's interpretation and could possibly be wrong


    //the data points
    //TODO: normalize these verticies onto screen space
    float testVertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left
   };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };



    //set the vertex shader (shader responsible for moving points to the correct location)
    //create a shader of type vertex shader cause we're making a vertex shader.
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);


    //sets the code for the shader that we're making to be the one set in graphics.h. the second and fourth parameters are unimportant because we are not using multiple strings
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //compile the shader's code
    glCompileShader(vertexShader);


    //this shader is responsible for the functionality behind (look at vertex shader for explanation on what each function do)
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragmentShader);


    //check if shader loaded correctly
    int shaderSuccess;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 512;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 512;
    }



    //this is a shader program that brings the other shaders together in a single program that will run every time we render an object
    shaderProgram = glCreateProgram();
    //connect the 2 shaders to the shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //makes shaderProgram a full shader by A) checking to make sure everything is fit correctly and B) bringing it together
    glLinkProgram(shaderProgram);

    //check if final shader loaded correctly
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 512;
    }

    //delete shaders from local memory?
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);




    //vertex handling


    //A Vertex Array (VAO) it is the super array that handles all of the type's of shader. This specific one is just for triangles

    //assign the specific ID that turns this int into a unique buffer
    glGenVertexArrays(1, &VAO);




    //A vertex buffer (VBO) is a list of vertex's that we then send to the gpu to do it's calculations
    //it is used to manage the graphics cards memory which is able to store large numbers of vertex

    //assign a specific ID that turns this int into a unique buffer
    glGenBuffers(1, &VBO);




    //set openGL's vertex buffer to be VBO
    //GL_ARRAY_BUFFER just means it will send an array as a buffer
    //this means the graphics card will need to deal with an array when it gets the data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    //tells openGL that we want to use this VAO
    glBindVertexArray(VAO);







    //sends the data that is set in the VBO to the GPU
    //change whatever the GL_ARRAY_BUFFER is (the VBO in the main function) to a buffer with our vertices
    //GL_STATIC_DRAW is a configuration given to the graphics card that says the data(testVertices) will only be set once and used a bunch
    //we will likely switch to GL_DYNAMIC_DRAW because we'll be using objects who's coordinates are moving
    glBufferData(GL_ARRAY_BUFFER, sizeof(testVertices), testVertices, GL_STATIC_DRAW);



    //EBO's are used to handle indices (lines connecting vertexes)
    glGenBuffers(1, &EBO);
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
    //glBindVertexArray(0);


    return 0;
}

void runRenderLoop()
{
    float vertices[object3d::getTotalNumbOfObjects()][3];

    //the thing that tells openGL to use the shader whenever rendering something
    glUseProgram(shaderProgram);
    //tells openGL to pull the data from the VAO inputted
    glBindVertexArray(VAO);
    //the first element states where in the current VBO we want to start with and the second states how many we want to draw

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
void runPhysicsLoop()
{

}

void runSimulationLoop(GLFWwindow* window)
{
    //this sets the background color used when clearing
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //this clears the color
    glClear(GL_COLOR_BUFFER_BIT);


    runRenderLoop();





}

void onResize(GLFWwindow* window,int width, int height)
{
    glViewport(0, 0, width, height);
}

