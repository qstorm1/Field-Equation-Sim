#pragma once
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

//logs unknown error
inline void logError() {
    std::fstream file;
    file.open("..\\src\\Logging\\ErrorLog.txt", std::ios::app);
    if (file.is_open()) {
        //get time
        const time_t now = time(nullptr);
        //log error with timestamp
        file << ctime(&now) << ' ' << "An Unknown Error Occurred" << std::endl;
    }
    file.close();
}

//logs error
template <typename T>
void logError(T &e) {
    std::fstream file;
    file.open("..\\src\\Logging\\ErrorLog.txt", std::ios::app);
    if (file.is_open()) {
        //get time and log error with timestamp
        const time_t now = time(nullptr);
        file << ctime(&now) << ' ' << e.what() << std::endl;
    }
    file.close();
}

void logErrorString(std::string &str) {
    std::fstream file;
    file.open("..\\src\\Logging\\ErrorLog.txt", std::ios::app);
    if (file.is_open()) {
        //get time and log error with timestamp
        const time_t now = time(nullptr);
        file << ctime(&now) << ' ' << str << std::endl;
    }
    file.close();
}

//puts console output also in file. USE THIS INSTEAD SO WE HAVE OUTPUT LOG OF CONSOLE IN CASE PROGRAM ENDS ABRUPTLY
inline void logConsole(std::stringstream &stream) {
    //create file
    std::fstream file;
    file.open("..\\src\\Logging\\coutLog.txt", std::ios::app);
    //get current time
    time_t now = time(nullptr);
    std::string output = (std::string) ctime(&now) + stream.str();
    //write to cout and output
    std::cout << output;
    if (file.is_open()) {
        file << output;
    }
    file.close();
}