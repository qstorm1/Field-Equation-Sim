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

    //test colour to make sure window works on windows (ba-dum tsss)
    glClearColor(0.75f, 0.5f, 0.75f, 1.0f);

    //the loop that runs so long as the window is open
    while (!glfwWindowShouldClose(window))
    {
        runSimulationLoop(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();



    //try catch block to log everything that errors
    try {

        //test objects for overlap checking
        object o = {0, 0, 0, 2, 4, "test object"};
        object p = {1, 1, 111, 2, 11};

        //test equals operator
        std::string filepath = "../coutLog.txt";

        std::stringstream ss;
        ss << (o != p) << std::endl;
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

}

void onResize(GLFWwindow* window,int width, int height)
{
    glViewport(0, 0, width, height);
}

