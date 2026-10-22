#pragma once
#include "../config.h"

class object {

    //track object ID in case we need that or something
    static inline unsigned int IDs = 0;

private:
    //declare attributes
    double x, y, z, mass, radius;   //METERS, METERS, METERS, KILOGRAMS, METERS FOR NOW
    unsigned int id;
    std::string name;

public:

    //constructor with default params using initialiser list
    object(double x = 0, double y = 0, double z = 0,
           double mass = 1, double radius = 1, std::string name = "")
        : x(x), y(y), z(z), mass(mass), radius(radius)
    {
        //checks if user has met the preconditions
        if (mass <= 0)
            throw std::invalid_argument("mass must be positive");
        if (radius <= 0)
            throw std::invalid_argument("radius must be positive");

        id = ++IDs;   // gives object custom ID
        //gives object of name "unnamed" if name parameter is left blank
        this->name = name.empty() ? "unnamed" + std::to_string(id)
                                  : std::move(name);
    }

    unsigned int getID() {
        return id;
    }

    //destructor
    ~object() {
        std::string filePath = "../coutLog.txt";
        std::stringstream ss;
        ss << "Destroyed object: " << id;
        logConsole(filePath, ss);
    }


};

