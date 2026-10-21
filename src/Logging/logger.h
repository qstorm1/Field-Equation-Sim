#pragma once
#include <ctime>
#include <fstream>
#include <iostream>

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
inline void logConsole(std::string &filePath, std::string stream) {
    std::fstream file;
    file.open(filePath);
    time_t now = time(nullptr);
    stream = (std::string) ctime(&now) + " " + stream;
    std::cout << stream;
    if (file.is_open()) {
        file << stream;
    }
    file.close();
}