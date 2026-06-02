#ifndef MODULE2_ROBOTASSIGNMENT_HPP
#define MODULE2_ROBOTASSIGNMENT_HPP

#include "../types.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

struct RobotAssignment {
    int orderID;
    int robotID;
};

struct RobotQueue {
    Robot robots[MAX_ROBOTS];
    int front;
    int rear;
    int count;
};

void initQueue(RobotQueue& queue) {
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
}

bool isQueueEmpty(const RobotQueue& queue) {
    return queue.count == 0;
}

bool isQueueFull(const RobotQueue& queue) {
    return queue.count == MAX_ROBOTS;
}

void enqueueRobot(RobotQueue& queue, Robot robot) {
    if (isQueueFull(queue)) {
        cout << "Robot queue is full. Cannot enqueue robot " << robot.robotID << endl;
        return;
    }
    queue.robots[queue.rear] = robot;
    queue.rear = (queue.rear + 1) % MAX_ROBOTS;
    queue.count++;
}

Robot dequeueRobot(RobotQueue& queue) {
    if (isQueueEmpty(queue)) {
        Robot sentinel;
        sentinel.robotID = -1;
        sentinel.status = AVAILABLE;
        sentinel.taskCount = 0;
        return sentinel;
    }
    Robot robot = queue.robots[queue.front];
    queue.front = (queue.front + 1) % MAX_ROBOTS;
    queue.count--;
    return robot;
}

int loadRobotsFromFile(string filename, RobotQueue& queue) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open robot file: " << filename << endl;
        return 0;
    }

    int loaded = 0;
    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        int id;
        char comma;
        string statusStr;
        ss >> id >> comma >> statusStr;

        RobotStatus status;
        if (statusStr == "available") {
            status = AVAILABLE;
        } else if (statusStr == "busy") {
            status = BUSY;
        } else if (statusStr == "maintenance") {
            status = MAINTENANCE;
        } else {
            continue;
        }

        Robot robot;
        robot.robotID = id;
        robot.status = status;
        robot.taskCount = 0;

        if (status == AVAILABLE) {
            enqueueRobot(queue, robot);
            loaded++;
        }
    }

    file.close();
    return loaded;
}

Robot dequeueAvailableRobot(RobotQueue& queue) {
    while (!isQueueEmpty(queue)) {
        Robot robot = dequeueRobot(queue);

        if (robot.status == MAINTENANCE) {
            cout << "Robot " << robot.robotID
                 << " is under maintenance. Diverted out of queue." << endl;
            continue;
        }

        robot.status = BUSY;
        return robot;
    }

    Robot sentinel;
    sentinel.robotID = -1;
    sentinel.status = AVAILABLE;
    sentinel.taskCount = 0;
    return sentinel;
}

void returnRobot(RobotQueue& queue, Robot& robot) {
    if (robot.status == MAINTENANCE) {
        cout << "Robot " << robot.robotID
             << " is under maintenance. Not returned to queue." << endl;
        return;
    }

    robot.status = AVAILABLE;
    robot.taskCount++;
    enqueueRobot(queue, robot);
}

void setRobotMaintenance(RobotQueue& queue, int robotID) {
    for (int i = 0; i < queue.count; i++) {
        int idx = (queue.front + i) % MAX_ROBOTS;
        if (queue.robots[idx].robotID == robotID) {
            queue.robots[idx].status = MAINTENANCE;
            cout << "Robot " << robotID << " set to maintenance." << endl;
            return;
        }
    }

    cout << "Robot " << robotID << " not found in available queue."
         << " It may be busy or does not exist." << endl;
}

void clearRobotMaintenance(RobotQueue& queue, int robotID) {
    Robot robot;
    robot.robotID = robotID;
    robot.status = AVAILABLE;
    robot.taskCount = 0;
    enqueueRobot(queue, robot);
    cout << "Robot " << robotID << " cleared from maintenance and returned to queue." << endl;
}

void displayAvailableRobots(const RobotQueue& queue) {
    if (isQueueEmpty(queue)) {
        cout << "No available robots." << endl;
        return;
    }

    cout << "Available robots:" << endl;
    for (int i = 0; i < queue.count; i++) {
        int idx = (queue.front + i) % MAX_ROBOTS;
        cout << "  Robot " << queue.robots[idx].robotID
             << " | Tasks completed: " << queue.robots[idx].taskCount << endl;
    }
}

void displayAssignments(const RobotAssignment assignments[], int count) {
    if (count == 0) {
        cout << "No assignments yet." << endl;
        return;
    }

    cout << "Robot assignments:" << endl;
    for (int i = 0; i < count; i++) {
        cout << "  Order " << assignments[i].orderID
             << " -> Robot " << assignments[i].robotID << endl;
    }
}

void displayStatusOverview(RobotQueue& queue,
                           const Robot allRobots[], int totalRobots,
                           const RobotAssignment assignments[], int assignCount) {
    cout << "=== Robot Status Overview ===" << endl;
    displayAvailableRobots(queue);
    cout << endl;
    displayAssignments(assignments, assignCount);
    cout << endl;

    cout << "All robots:" << endl;
    for (int i = 0; i < totalRobots; i++) {
        string statusStr;
        if (allRobots[i].status == AVAILABLE) {
            statusStr = "available";
        } else if (allRobots[i].status == BUSY) {
            statusStr = "busy";
        } else if (allRobots[i].status == MAINTENANCE) {
            statusStr = "maintenance";
        }
        cout << "  Robot " << allRobots[i].robotID
             << " | Status: " << statusStr
             << " | Tasks: " << allRobots[i].taskCount << endl;
    }
    cout << "=============================" << endl;
}

#endif
    