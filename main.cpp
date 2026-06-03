// Compilation (from project root):
//   g++ -std=c++17 -I. main.cpp "module 5/module5 - WarehouseLayout.cpp" -o warehouse

#include "types.hpp"
#include "module 1/module1 - orderManagement.hpp"
#include "module 2/module2 - robotAssignment.hpp"
#include "module 3/module3 - navigationTracking.hpp"
#include "module 4/module4 - itemSearch.hpp"
#include "module 5/module5 - warehouseLayout.hpp"

int main() {
    OrderQueue     orderQueue;
    initQueue(orderQueue);

    RobotQueue     robotQueue;
    initQueue(robotQueue);

    ItemSearchBST  itemBST;
    WarehouseLayout warehouse;
    NavigationTracking nav;

    loadOrders(orderQueue, "assets/orders.csv");
    loadRobotsFromFile("assets/robots.csv", robotQueue);
    itemBST.loadFromCSV("assets/items.csv");
    warehouse.buildFromFile("assets/items.csv");

     // printing status
    cout << "\n=== WAREHOUSE ROBOT NAVIGATION SYSTEM ===" << endl;

    cout << "\n Module 1: Pending Orders " << endl;
    displayPending(orderQueue);

    cout << "\n Module 2: Available Robots " << endl;
    displayAvailableRobots(robotQueue);

    cout << "\n Module 4: Item Inventory (sorted by ID) " << endl;
    itemBST.displayInOrder();

    cout << "\n Module 5: Warehouse Layout " << endl;
    warehouse.displayLayout();


    RobotAssignment assignments[MAX_ORDERS];
    int assignCount = 0;

    // goes through each order and does the entire life cycle module 1 -> 2 -> 4 -> 5 -> 3
    while (!isEmpty(orderQueue)) {
        cout << "\n========================================" << endl;

        Order order = dequeueOrder(orderQueue);
        cout << "[M1] Order " << order.orderID << " dequeued (Item " << order.itemID << ")" << endl;

        Robot robot = dequeueAvailableRobot(robotQueue);
        if (robot.robotID == -1) {
            cout << "[M2] No available robots. Order " << order.orderID << " cannot be processed this cycle." << endl;
            continue;
        }
        cout << "[M2] Robot " << robot.robotID << " assigned to Order " << order.orderID << endl;

        if (assignCount < MAX_ORDERS) {
            assignments[assignCount].orderID = order.orderID;
            assignments[assignCount].robotID = robot.robotID;
            assignCount++;
        }

        Item* item = itemBST.searchByID(order.itemID);
        if (item == nullptr) {
            cout << "[M4] Item " << order.itemID << " not found in inventory. Skipping order." << endl;
            returnRobot(robotQueue, robot);
            continue;
        }
        cout << "[M4] Item \"" << item->name << "\" located at"
             << " Zone " << item->location.zone
             << ", Aisle " << item->location.aisle
             << ", Shelf " << item->location.shelf << endl;

        Route route;
        if (!warehouse.generateRoute(item->location, route)) {
            cout << "[M5] Route generation failed. Skipping order." << endl;
            returnRobot(robotQueue, robot);
            continue;
        }
        cout << "[M5] Route generated (" << route.count << " stops)" << endl;

        cout << "\n[M3] Out journey (Robot " << robot.robotID << ") " << endl;
        nav.setRobot(robot);
        nav.setRoute(route);
        nav.navigate();

        cout << "\n[M3] Return journey (Robot " << robot.robotID << ")" << endl;
        nav.returnHome();

        returnRobot(robotQueue, robot);
        cout << "[M2] Robot " << robot.robotID << " returned to queue (tasks completed: " << robot.taskCount + 1 << ")" << endl;

        completeOrder(orderQueue, order);
        cout << "[M1] Order " << order.orderID << " marked as COMPLETED." << endl;
    }

    cout << "\n========================================" << endl;
    cout << "All orders processed." << endl;

    // testing for obstacles if have then just go back home
    cout << "\n========================================" << endl;
    cout << "[M3] Obstacle Test" << endl;
    Location obstacle = {2, 3, 0};
    Item* testItem = itemBST.searchByID(34);
    if (testItem != nullptr) {
        Route testRoute;
        if (warehouse.generateRoute(testItem->location, testRoute)) {
            Robot testRobot;
            testRobot.robotID = 99;
            testRobot.status = AVAILABLE;
            testRobot.taskCount = 0;
            nav.setRobot(testRobot);
            nav.setRoute(testRoute);
            cout << "Obstacle placed at Zone " << obstacle.zone
                 << ", Aisle " << obstacle.aisle
                 << ", Shelf " << obstacle.shelf << endl;
            nav.navigate(obstacle);
        }
    }

    // final display of everything
    cout << "\n Module 1: Assigned Orders History " << endl;
    displayAssigned(orderQueue);

    cout << "\n Module 1: Completed Orders History " << endl;
    displayCompleted(orderQueue);

    cout << "\n Module 2: Final Assignment Log " << endl;
    displayAssignments(assignments, assignCount);

    cout << "\n Module 2: Final Robot Pool " << endl;
    displayAvailableRobots(robotQueue);

    return 0;
}
