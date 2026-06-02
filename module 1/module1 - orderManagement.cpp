#include "module1 - orderManagement.hpp"  
#include <fstream>                         
#include <iostream>                         
using namespace std;                        


void initQueue(OrderQueue& queue) {
    queue.frontIndex    = 0; 
    queue.rearIndex     = 0;  
    queue.count         = 0;  
    queue.assignedCount = 0;  
}


bool isEmpty(OrderQueue& queue) {
    return queue.count == 0;  
}


bool isFull(OrderQueue& queue) {
    return queue.count == MAX_ORDERS;  
}


void enqueueOrder(OrderQueue& queue, Order newOrder) {
    if (isFull(queue)) {                                          
        cout << "Queue is full, cannot add Order "
             << newOrder.orderID << "." << endl;                  
        return;                                                   
    }

    queue.orders[queue.rearIndex] = newOrder;              
    queue.rearIndex = (queue.rearIndex + 1) % MAX_ORDERS;  
    queue.count++;                                         
}


Order dequeueOrder(OrderQueue& queue) {
    if (isEmpty(queue)) {                             
        cout << "Error: Queue is empty. No order to dequeue." << endl;
        Order empty = {-1, -1, PENDING};               
        return empty;
    }

    Order front = queue.orders[queue.frontIndex];     
    front.status = ASSIGNED;                           

    if (queue.assignedCount < MAX_ORDERS) {            
        queue.assigned[queue.assignedCount] = front;  
        queue.assignedCount++;                         
    }

    queue.frontIndex = (queue.frontIndex + 1) % MAX_ORDERS; 
    queue.count--;                                          

    return front;  
}


void loadOrders(OrderQueue& queue, string filename) {
    ifstream file(filename);        
    if (!file.is_open()) {         
        cout << "Error: Could not open file '" << filename << "'." << endl;
        return;                     
    }

    int  id, item;  
    char comma;     

    while (file >> id >> comma >> item) {     
        Order newOrder = {id, item, PENDING}; 
        enqueueOrder(queue, newOrder);        
    }

    file.close();  
}


void displayPending(OrderQueue& queue) {
    if (isEmpty(queue)) {                      // nothing to print
        cout << "No pending orders." << endl;
        return;
    }

    cout << "Pending Orders (" << queue.count << ")" << endl;

    int index = queue.frontIndex;              
    for (int i = 0; i < queue.count; i++) {   
        cout << "  [" << (i + 1) << "] Order " << queue.orders[index].orderID
             << " | Item "                     << queue.orders[index].itemID << endl;
        index = (index + 1) % MAX_ORDERS;     
}


void displayAssigned(OrderQueue& queue) {
    if (queue.assignedCount == 0) {          
        cout << "No assigned orders yet." << endl;
        return;
    }

    cout << "--- Assigned Orders (" << queue.assignedCount << ") ---" << endl;
    for (int i = 0; i < queue.assignedCount; i++) {       
        cout << "  [" << (i + 1) << "] Order " << queue.assigned[i].orderID
             << " | Item "                                << queue.assigned[i].itemID << endl;
    }
}


void displayCurrent(OrderQueue& queue) {
    if (isEmpty(queue)) {                              
        cout << "No order currently waiting to be processed." << endl;
        return;
    }

    cout << "Next order to process: Order "
         << queue.orders[queue.frontIndex].orderID    
         << " | Item "
         << queue.orders[queue.frontIndex].itemID << endl;
}
