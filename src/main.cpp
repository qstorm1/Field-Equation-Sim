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
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    return 0;


    //try catch block to log everything that errors
    try {
        testStruct t;
        t.call();
    }catch(const std::invalid_argument& e) {
        logError(errorLog, e);
    }catch(std::exception &e) {
        logError(errorLog, e);
    }catch(...){
        logError(errorLog);
    }

    errorLog.close();


}



