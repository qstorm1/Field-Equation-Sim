#include "config.h"
#include <GLFW/glfw3.h>

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
            return 50;
        }

        //set the window
        window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
        //if there was an error in window creation, terminate all processes
        if (!window)
        {
            glfwTerminate();
            return 50;
        }
    }
    catch (const std::exception& e)
    {
        logError(errorLog, e);
    }


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwTerminate();



    //try catch block to log everything that errors
    try {
        //test console logger
        std::string filepath = "../coutLog.txt";
        logConsole(filepath, "hi");
        logConsole(filepath, "hello");

        object o = {0, 0, 0, 2, 4, "test object"};

    }catch(const std::invalid_argument& e) {
        logError(errorLog, e);
    }catch(std::exception &e) {
        logError(errorLog, e);
    }catch(...){
        logError(errorLog);
    }

    errorLog.close();
    return 0;

}



