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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

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
    bool isOverlapping(const object3d &other) const
    {
        const double distanceCenter = calculateDistance(other);
        const double radiusSum = this->radius + other.radius;
        return radiusSum > distanceCenter;
    }



public:


    std::string name;
    //constructor with default params using an initializer list
    object3d(std::vector<object3d*>* world,const double x = 0, const double y = 0, const double z = 0,
           const double mass = 1, const double radius = 1, std::string name = "")
        : x(x), y(y), z(z), mass(mass), radius(radius)
    {
        //checks if user has met the preconditions
        if (mass <= 0) {
            throw std::invalid_argument("mass must be positive");
        }
        if (radius <= 0) {
            throw std::invalid_argument("radius must be positive");
        }

        try
        {
            checkCollision(*world);
        }
        catch (const std::invalid_argument& e)
        {
            throw;
        }




        //keeping id system because it might be useful IDK

        id = ++totalIDs;   // gives object custom ID
        //gives object of name "unnamed" if name parameter is left blank
        this->name = name.empty() ? "unnamed" + std::to_string(id)
                                  : std::move(name);

        //adds object to list of objects
        objects[id] = this;



        world->push_back(this);


    }

    //normal functions

    //calculates euclidian distance
    double calculateDistance(const object3d &other) const {
        return sqrt(pow(this->x - other.x,2) + pow(this->y - other.y, 2) + pow(this->z - other.z, 2));
    }

    //return a distance vector from this to other

    glm::vec3 calculateDistanceVector(const object3d &other) const {
        const glm::vec3 distance(other.x-x, other.y-y, other.z-z);
        return distance;
    }

    //gets magnitude of newtonian force
    double calculateNewtonForceMagnitude(object3d &other) const {
        return (mass * other.mass * G)/(calculateDistance(other) * calculateDistance(other));
    }

    //return a force vector to another object
    glm::vec3 calculateNewtonForceVector(object3d &other) const {
        glm::vec3 force = glm::normalize(calculateDistanceVector(other));
        force*=calculateNewtonForceMagnitude(other);
        return force;
    }

    //returns the net force being exerted on each object
    glm::vec3 calculateGravitationalField() const {
        glm::vec3 total(0.0, 0.0, 0.0);
        //adds each force vector to total
        for (auto& [otherId, other] : objects) {
            if (otherId == id) continue;
            glm::vec3 force = calculateNewtonForceVector(*other);
            total+=force;
        }
        return total;
    }

    //returns net acceleration on the object
    glm::vec3 calculateAccelerationField() const {
        glm::vec3 force = calculateGravitationalField();
        force/=mass;
        return force;
    }

    //getter functions
    unsigned int getID() const {
        return id;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getZ() const {
        return z;
    }

    glm::vec3 getPos() const
    {
        return glm::vec3(getX(),getY(),getZ());
    }

    double getMass() const {
        return mass;
    }

    double getRadius() const{
        return radius;
    }

    //setter functions

    void setX(const std::vector<object3d*>& world,double n) {
        const double oldX = x;
        x = n;
        //throw exception if 2 objects overlap
        try
        {
            checkCollision(world);
        }
        catch (const std::invalid_argument& e)
        {
            x = oldX;
        }
    }

    void setY(const std::vector<object3d*>& world,double n) {
        const double oldY = y;
        y = n;
        //throw exception if 2 objects overlap
        try
        {
            checkCollision(world);
        }
        catch (const std::invalid_argument& e)
        {
            y = oldY;
        }
    }

    void setZ(const std::vector<object3d*>& world,double n) {
        const double oldZ = z;
        z = n;
        //throw exception if 2 objects overlap during
        try
        {
            checkCollision(world);
        }
        catch (const std::invalid_argument& e)
        {
            z=oldZ;
        }
    }

    void setRadius(const std::vector<object3d*>& world, const double n) {
        const double oldRadius = radius;
        radius = n;
        //throw exception if 2 objects overlap during
        try
        {
            checkCollision(world);
        }
        catch (const std::invalid_argument& e)
        {
            radius = oldRadius;
        }
    }

    void setMass(const double n) {
        mass = n;
    }



    /**
     * Throws invalid_argument error when failing collision check
     * @param world the world of objects to check collisions with
     */
    void checkCollision(const std::vector<object3d*>& world) const noexcept(false)
    {
        for (const object3d* other : world)
        {
            if (isOverlapping(*other))
            {
                std::stringstream errorMessage;
                errorMessage << "objects cannot overlap: overlapping with: " << other->name + " id: " <<  other->id;
                throw std::invalid_argument(errorMessage.str());
            }
        }
    }





    //operator overrides

    bool operator==(const object3d &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    //destructor
    ~object3d() {
        objects.erase(id);
        std::stringstream ss;
        ss << "Destroyed object: " << id << std::endl;
        logConsole(ss);
    }

    //static methods

    //gets object by ID
    static object3d& getObjectByID(const std::vector<object3d*>* world,const unsigned int num) {
        for (object3d* obj : *world)
        {
            if (obj->getID() == num)
            {
                return *obj;
            }
        }
        throw std::out_of_range("No such object");
    }


    static std::vector<glm::vec3> getAllObjectsPosition(const std::vector<object3d*>* world)
    {
        std::vector <glm::vec3> result;
        for (const object3d* obj : *world)
        {
            result.push_back(obj->getPos());
        }
        return result;
    }

    //deletes all objects
    static void deleteAllObjects(std::vector<object3d*>* world) {
        world->clear();
    }

    //perhaps change this to have custom point struct as an argument
    static double calculateMassDensity(const std::vector<object3d*>* world, const double xStart, const double yStart, const double zStart,
        const double xEnd, const double yEnd, const double zEnd) {
        double mass = 0;

        if (xStart > xEnd) {
            throw std::invalid_argument("Starting X value cannot be larger than ending X value.");
        }
        if (yStart > yEnd) {
            throw std::invalid_argument("Starting Y value cannot be larger than ending Y value.");
        }
        if (zStart > zEnd) {
            throw std::invalid_argument("Starting Z value cannot be larger than ending Z value.");
        }

        //adds mass of objects
        for (const object3d* obj: *world) {
            //checks if object is inside given bounds
            if (obj->x > xStart && obj->x < xEnd) {
                if (obj->y > yStart && obj->y < yEnd) {
                    if (obj->z > zStart && obj->z < zEnd) {
                        mass += obj->mass;
                    }
                }
            }
        }
        //Perhaps include calculation for circular segment IF NECESSARY

        const double volume = (xEnd - xStart) * (yEnd - yStart) * (zEnd - zStart);

        return mass/volume;

    }


};

