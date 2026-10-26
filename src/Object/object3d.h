/*  OBJECT CLASS STRUCTURE
 *  So far the main differences are a different obj file being rendered (for visual purposes)
 *
 *  Object
 *  ->Black Hole
 *  ->Neutron Star
 *  ->Galaxy
 *  ->Star
 *  ->Planet
 *      ->Mercury, Venus, Earth, ... (+ more if we want)
 *  ->Other
 */

#pragma once
#include <vector>
#include <sstream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <utility>
#include <map>
#include "../matrix.hpp"

#define G (6.6743 * pow(10,-11))

//this should be in a .cpp file
class object3d {

    //track object ID in case we need that or something
    static inline unsigned int totalIDs = 0;
    //tracks current ID's of each object

    //creates a map of objects that can be accessed from ID
    static inline std::map<unsigned int, object3d*> objects;

    //declare attributes
    double x, y, z, mass, radius;   //METERS, METERS, METERS, KILOGRAMS, METERS FOR NOW
    unsigned int id;

    //checks if 2 objects overlap
    bool isOverlapping(object3d &other) {
        double distanceCenter = calculateDistance(other);
        double radiusSum = this->radius + other.radius;
        return radiusSum > distanceCenter;
    }



public:

    std::string name;
    //constructor with default params using initialiser list
    object3d(double x = 0, double y = 0, double z = 0,
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
    double calculateDistance(object3d &other) {
        return sqrt(pow(this->x - other.x,2) + pow(this->y - other.y, 2) + pow(this->z - other.z, 2));
    }

    //return a distance vector from this to other
    Matrix* calculateDistanceVector(object &other) {
        Matrix* m = initMatrix(3, 1);
        double array[] = {other.x-x, other.y-y, other.z-z};
        assignMatrix(m, array);
        return m;
    }

    //gets magnitude of newtonian force
    double calculateNewtonForceMagnitude(object &other) {
        return (mass * other.mass * G)/(calculateDistance(other) * calculateDistance(other));
    }

    //return a force vector to another object
    Matrix* calculateNewtonForceVector(object &other) {
        Matrix* force = calculateDistanceVector(other);
        double normalisedForce = calculateNewtonForceMagnitude(other)/calculateDistance(other);
        for (int i = 0; i < 3; i++) {
            force->data[i] *= normalisedForce;   // scale the direction vector
        }
        return force;
    }

    //returns the total force being exerted on each object
    Matrix* calculateNewtonTotalForce() {
        Matrix* total = initMatrix(3, 1);
        for (auto& [otherId, other] : objects) {
            if (otherId == id) continue;
            Matrix* force = calculateNewtonForceVector(*other);
            Matrix* sum = addMatrix(total, force);
            freeMatrix(force);
            freeMatrix(total);
            total = sum;
        }
        return total;
    }

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

    static int getTotalNumbOfObjects()
    {
        return objects.size();
    }


    static std::vector<double[3]> getAllObjectsPosition()
    {

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

    bool operator==(object3d &other) {
        return x == other.x && y == other.y && z == other.z;
    }

    //destructor
    ~object3d() {
        objects.erase(id);
        std::string filePath = "../coutLog.txt";
        std::stringstream ss;
        ss << "Destroyed object: " << id << std::endl;
        logConsole(filePath, ss);
    }

    //static methods

    //gets object by ID
    static object3d& getObjectByID(unsigned int num) {
        auto it = objects.find(num);
        if (it == objects.end()) {
            throw std::out_of_range("ID not found");
        }
        return *it->second;
    }


};

