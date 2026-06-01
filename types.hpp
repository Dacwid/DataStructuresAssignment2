#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

using namespace std;

const int MAX_ORDERS   = 100;
const int MAX_ROBOTS   = 10;
const int MAX_ROUTE    = 50;
const int MAX_ITEMS    = 100;
const int MAX_CHILDREN = 20;


struct Location {
    int zone;
    int aisle;
    int shelf;
};


enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum OrderStatus {
    PENDING,
    ASSIGNED,
    COMPLETED
};

struct Order {
    int orderID;
    int itemID;
    OrderStatus status;
};

enum RobotStatus {
    AVAILABLE,
    BUSY,
    MAINTENANCE
};

struct Robot {
    int robotID;
    RobotStatus status;
    int taskCount;
};

struct Item {
    int itemID;
    string name;
    Location location;
};


struct Route {
    Location stops[MAX_ROUTE];
    int count;
};

#endif