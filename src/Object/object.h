#pragma once
#include <vector>
#include <sstream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <utility>
#include <map>

//this should be in a .cpp file
class object {

    //track object ID in case we need that or something
    static inline unsigned int totalIDs = 0;
    //tracks current ID's of each object

    //creates a map of objects that can be accessed from ID
    static inline std::map<unsigned int, object*> objects;

    //declare attributes
    double x, y, z, mass, radius;   //METERS, METERS, METERS, KILOGRAMS, METERS FOR NOW
    unsigned int id;

    //checks if 2 objects overlap
    bool isOverlapping(object &other) {
        double distanceCenter = calculateDistance(other);
        double radiusSum = this->radius + other.radius;
        return radiusSum > distanceCenter;
    }



public:

    std::string name;
    //constructor with default params using initialiser list
    object(double x = 0, double y = 0, double z = 0,
           double mass = 1, double radius = 1, std::string name = "")
        : x(x), y(y), z(z), mass(mass), radius(radius)
    {
        //checks if user has met the preconditions
        if (mass <= 0) {
            throw std::invalid_argument("mass must be positive");
        }
        if (radius <= 0) {
            throw std::invalid_argument("radius must be positive");
        }

        //throws an exception if 2 objects overlap during creation
        auto it = objects.begin();
        while (it != objects.end()) {
            if (isOverlapping(*(it->second) ) ) {
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << (*(it->second)).name + " id: " <<  (*(it->second)).id;
                throw std::invalid_argument(errorMessage.str());
            }
            it++;
        }

        id = ++totalIDs;   // gives object custom ID
        //gives object of name "unnamed" if name parameter is left blank
        this->name = name.empty() ? "unnamed" + std::to_string(id)
                                  : std::move(name);

        //adds object to list of objects
        objects[id] = this;
    }

    //normal functions

    //calculates euclidian distance
    double calculateDistance(object &other) {
        return sqrt(pow(this->x - other.x,2) + pow(this->y - other.y, 2) + pow(this->z - other.z, 2));
    }

    //
    // std::tuple<double, double, double> calculateNewtonianForce() {
    //
    // }

    //getter functions

    unsigned int getID() {
        return id;
    }

    double getX() {
        return x;
    }

    double getY() {
        return y;
    }

    double getZ() {
        return z;
    }

    double getMass() {
        return mass;
    }

    double getRadius(){
        return radius;
    }

    //setter functions

    void setX(double n) {
        double oldX = x;
        x = n;
        //throw exception if 2 objects overlap
        auto it = objects.begin();
        while (it != objects.end()) {
            if (isOverlapping(*(it->second) ) ) {
                x = oldX;
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << (*(it->second)).name + " id: " <<  (*(it->second)).id;
                throw std::invalid_argument(errorMessage.str());
            }
            it++;
        }
    }

    void setY(double n) {
        double oldY = y;
        y = n;
        //throw exception if 2 objects overlap
        auto it = objects.begin();
        while (it != objects.end()) {
            if (isOverlapping(*(it->second) ) ) {
                y = oldY;
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << (*(it->second)).name + " id: " <<  (*(it->second)).id;
                throw std::invalid_argument(errorMessage.str());
            }
            it++;
        }
    }

    void setZ(double n) {
        double oldZ = z;
        z = n;
        //throw exception if 2 objects overlap during
        auto it = objects.begin();
        while (it != objects.end()) {
            if (isOverlapping(*(it->second) ) ) {
                z = oldZ;
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << (*(it->second)).name + " id: " <<  (*(it->second)).id;
                throw std::invalid_argument(errorMessage.str());
            }
            it++;
        }
    }

    void setRadius(double n) {
        double oldRadius = radius;
        radius = n;
        //throw exception if 2 objects overlap during
        auto it = objects.begin();
        while (it != objects.end()) {
            if (isOverlapping(*(it->second) ) ) {
                radius = oldRadius;
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << (*(it->second)).name + " id: " <<  (*(it->second)).id;
                throw std::invalid_argument(errorMessage.str());
            }
            it++;
        }
    }

    void setMass(double n) {
        mass = n;
    }

    //operator overrides

    bool operator==(object &other) {
        return x == other.x && y == other.y && z == other.z;
    }

    //destructor
    ~object() {
        objects.erase(id);
        std::string filePath = "../coutLog.txt";
        std::stringstream ss;
        ss << "Destroyed object: " << id << std::endl;
        logConsole(filePath, ss);
    }

    //static methods

    //gets object by ID
    static object& getObjectByID(unsigned int num) {
        auto it = objects.find(num);
        if (it == objects.end()) {
            throw std::out_of_range("ID not found");
        }
        return *it->second;
    }


};

