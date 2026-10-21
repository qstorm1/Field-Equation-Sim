//
// Created by ASKPhD on 10/20/2026.
//

#ifndef GRAVITATIONAL_LENSING_SIMULATION_TESTFILE_H
#define GRAVITATIONAL_LENSING_SIMULATION_TESTFILE_H


#include "config.h"

struct testStruct {

    void call() {
        std::string filepath = "../coutLog.txt";
        logConsole(filepath, "test file ran");
    }

};

#endif //GRAVITATIONAL_LENSING_SIMULATION_TESTFILE_H
