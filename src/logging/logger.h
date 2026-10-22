#pragma once
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

//logs unknown error
inline void logError(std::fstream &file) {
    if (file.is_open()) {
        const time_t now = time(nullptr);
        file << ctime(&now) << ' ' << "An Unknown Error Occurred" << std::endl;
    }
}

//logs error
void logError(std::fstream &file, auto &e) {
    if (file.is_open()) {
        const time_t now = time(nullptr);
        file << ctime(&now) << ' ' << e.what() << std::endl;
    }
}

//puts console output also in file. USE THIS INSTEAD SO WE HAVE OUTPUT LOG OF CONSOLE IN CASE PROGRAM ENDS ABRUPTLY
inline void logConsole(std::string &filePath, std::stringstream &stream) {
    std::fstream file;
    file.open(filePath, std::ios::app);
    time_t now = time(nullptr);
    std::string output = (std::string) ctime(&now) + " " + stream.str();
    std::cout << output;
    if (file.is_open()) {
        file << output << std::endl;
    }
    file.close();
}