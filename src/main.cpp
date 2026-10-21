#include "config.h"


int main() {

    //create files to write to
    std::fstream errorLog;

    errorLog.open("../ErrorLog.txt", std::ios::app);

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



