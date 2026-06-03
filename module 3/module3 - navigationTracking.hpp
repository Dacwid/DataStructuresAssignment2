#ifndef MODULE3_NAVIGATIONTRACKING_HPP
#define MODULE3_NAVIGATIONTRACKING_HPP

#include "../types.hpp"

// Navigation Tracking class to actually move the robot through the route and back home
class NavigationTracking {
private:
    // attributes used for navigation and stack data structure
    Direction stack[MAX_ROUTE];
    int stackTop;
    Route currentRoute;
    int currentLocationIndex;
    Robot currentRobot;

public:
    // constructor
    NavigationTracking() {
        stackTop = -1;
        currentLocationIndex = 0;
    }

    // setters
    void setRoute(const Route& route) {
        currentRoute = route;
        currentLocationIndex = 0;
    }

    void setRobot(const Robot& robot) {
        currentRobot = robot;
    }

    // push to stack the direction for later inverse navigation
    void pushDirection(Direction direction) {
        if (stackTop < MAX_ROUTE - 1) {
            stack[++stackTop] = direction;
        } else {
            // guard for max stack
            cout << "Stack is full: Cannot push more directions." << endl;
        }
    }

    // pop from stack the direction for later inverse navigation
    Direction popDirection() {
        if (stackTop >= 0) {
            return stack[stackTop--];
        } else {
            // guard for empty stack
            cout << "Stack is empty: No directions to pop." << endl;
            return FORWARD;
        }
    }

    // checks if stack is empty or not
    bool isStackEmpty() {
        return stackTop == -1;
    }

    // function to get the opposite direction for later inverse navigation
    Direction inverseDirection(Direction direction) {
        switch (direction) {
            case FORWARD:
                return BACKWARD;
            case BACKWARD:
                return FORWARD;
            case LEFT:
                return RIGHT;
            case RIGHT:
                return LEFT;
            case UP:
                return DOWN;
            case DOWN:
                return UP;
            default:
                return FORWARD;
        }
    }

    // print direction taken
    void printDirectionTaken (Direction direction) {
        cout << "Robot " << currentRobot.robotID << " moved ";
        switch (direction) {
            case FORWARD:
                cout << "FORWARD";
                break;
            case BACKWARD:
                cout << "BACKWARD";
                break;
            case LEFT:
                cout << "LEFT";
                break;
            case RIGHT:
                cout << "RIGHT";
                break;
            case UP:
                cout << "UP";
                break;
            case DOWN:
                cout << "DOWN";
                break;
        } 
        cout << " to Location (Zone: " << currentRoute.stops[currentLocationIndex].zone
             << ", Aisle: " << currentRoute.stops[currentLocationIndex].aisle
             << ", Shelf: " << currentRoute.stops[currentLocationIndex].shelf << ")" << endl;
    }

    // logic to check direction based on current and previous location
    Direction determineDirection(Location previous, Location current) {
        if (current.zone > previous.zone) {
            return FORWARD;
        } else if (current.zone < previous.zone) {
            return BACKWARD;
        } else if (current.aisle > previous.aisle) {
            return RIGHT;
        } else if (current.aisle < previous.aisle) {
            return LEFT;
        } else if (current.shelf > previous.shelf) {
            return UP; 
        } else if (current.shelf < previous.shelf) {
            return DOWN;
        } else {
            return FORWARD;
        }
    }

     // logic to actually navigate based on the route and also stores the direction taken in the stack for later inverse navigation
    void navigate(Location obstacle = {-1, -1, -1}) {
        stackTop = -1;
        if (currentRoute.count == 0) {
            cout << "No route set for navigation." << endl;
            return;
        }

        // go through each stop
        for (int i = 0; i < currentRoute.count; i++) {
            // sets current location
            Location currentLocation = currentRoute.stops[i];
            currentLocationIndex = i;
            if (i > 0) {
                // using previous and current location, use determinDirection function to get the direction
                Location previousLocation = currentRoute.stops[i - 1];
                // check if corrdinate is default arg then no need to skip
                if (obstacle.zone == -1 && obstacle.aisle == -1 && obstacle.shelf == -1) {
                    continue;
                } else if (currentLocation.zone == obstacle.zone && currentLocation.aisle == obstacle.aisle && currentLocation.shelf == obstacle.shelf) {
                    cout << "Obstacle detected at Location (Zone: " << currentLocation.zone
                         << ", Aisle: " << currentLocation.aisle
                         << ", Shelf: " << currentLocation.shelf << "). Going back home" << endl;
                    returnHome();
                    return;
                }
                Direction direction = determineDirection(previousLocation, currentLocation);
                pushDirection(direction);
                printDirectionTaken(direction);
            // this is for root node
            } else if (i == 0) {
                cout << "Starting at Location (Zone: " << currentLocation.zone
                     << ", Aisle: " << currentLocation.aisle
                     << ", Shelf: " << currentLocation.shelf << ")" << endl;
            }
        }
        cout << "Robot " << currentRobot.robotID << " has reached its destination." << endl;
    }
    // inverse navigation logic
    void returnHome () {
        // guard for empty stack
        if (isStackEmpty()) {
            cout << "No route to return from." << endl;
            return;
        }

        // goes through each direction in the stack
        while (!isStackEmpty()) {
            if (currentLocationIndex > 0) {
                currentLocationIndex--;
            }
            Direction direction = popDirection();
            // inverse the direction and prints it
            Direction inverse = inverseDirection(direction);
            printDirectionTaken(inverse);
        }
        cout << "Robot " << currentRobot.robotID << " has returned home." << endl;
    }
};

#endif