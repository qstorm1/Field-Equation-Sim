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

        //test objects for overlap checking
        object o = {0, 0, 0, 2, 4, "test object"};
        object p = {1, 1, 111, 2, 11};

        //test getObjectById
        std::stringstream ss;
        ss << std::addressof(object::getObjectByID(1)) << std::endl;
        std::string filepath = "../coutLog.txt";
        logConsole(filepath, ss);
        ss.str("");
        ss << std::addressof(object::getObjectByID(2)) << std::endl;
        logConsole(filepath, ss);
        ss.str("");
        //OH SWEET JESUS IT WORKS I SPEND LIKE 1.5HRS ON THIS SHIT
        //(I'm Jewish)

        o.setZ(1);



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



