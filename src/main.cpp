#include "config.h"


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


    //A vertex buffer is a list of vertex's that we then send to the gpu to do it's calculations
    unsigned int VBO;
    //assign a specific ID that turns this int into a buffer
    glGenBuffers(1, &VBO);
    //set openGL's vertex buffer to be VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    //the loop that runs so long as the window is open
    while (!glfwWindowShouldClose(window))
    {
        runSimulationLoop(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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



void runSimulationLoop(GLFWwindow* window)
{
    //this sets the color used when clearing
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //this clears the color
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[object3d::getTotalNumbOfObjects()][3];

    //TODO: normalize these verticies onto screen space

    float testVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    //change whatever the GL_ARRAY_BUFFER is (the VBO in the main function) to a buffer with our vertices
    //GL_STATIC_DRAW is a configuration given to the graphics card that says the data(testVertices) will only be set once and used a bunch
    //we will likely switch to GL_DYNAMIC_DRAW because we'll be using objects who's coordinates are moving
    glBufferData(GL_ARRAY_BUFFER, sizeof(testVertices), testVertices, GL_STATIC_DRAW);




}

void onResize(GLFWwindow* window,int width, int height)
{
    glViewport(0, 0, width, height);
}

