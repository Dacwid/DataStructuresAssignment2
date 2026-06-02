#ifndef ORDER_MANAGEMENT_HPP  
#define ORDER_MANAGEMENT_HPP

#include "types.hpp"         
#include <fstream>            
#include <iostream>           


struct OrderQueue {
    Order orders[MAX_ORDERS];  
    int frontIndex;           
    int rearIndex;            
    int count;               


    Order assigned[MAX_ORDERS]; 
    int   assignedCount;        
};


inline void initQueue(OrderQueue& queue) {
    queue.frontIndex    = 0;  
    queue.rearIndex     = 0; 
    queue.count         = 0;
    queue.assignedCount = 0;
}

inline bool isEmpty(OrderQueue& queue) {
    return queue.count == 0;
}

inline bool isFull(OrderQueue& queue) {
    return queue.count == MAX_ORDERS;
}


inline void enqueueOrder(OrderQueue& queue, Order newOrder) {
    if (isFull(queue)) {                                         
        cout << "Error: Queue is full. Cannot add Order "
             << newOrder.orderID << "." << endl;                 
        return;                                                  
    }

    queue.orders[queue.rearIndex] = newOrder;            
    queue.rearIndex = (queue.rearIndex + 1) % MAX_ORDERS;
    queue.count++;                                         
}

inline Order dequeueOrder(OrderQueue& queue) {
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

inline void loadOrders(OrderQueue& queue, string filename) {
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

inline void displayPending(OrderQueue& queue) {
    if (isEmpty(queue)) {                      
        cout << "No pending orders." << endl;
        return;
    }

    cout << "--- Pending Orders (" << queue.count << ") ---" << endl;

    int index = queue.frontIndex;              
    for (int i = 0; i < queue.count; i++) {   
        cout << "  [" << (i + 1) << "] Order " << queue.orders[index].orderID
             << " | Item "                     << queue.orders[index].itemID << endl;
        index = (index + 1) % MAX_ORDERS;     
    }
}


inline void displayAssigned(OrderQueue& queue) {
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

inline void displayCurrent(OrderQueue& queue) {
    if (isEmpty(queue)) {                               
        cout << "No order currently waiting to be processed." << endl;
        return;
    }

    cout << "Next order to process: Order "
         << queue.orders[queue.frontIndex].orderID     
         << " | Item "
         << queue.orders[queue.frontIndex].itemID << endl;
}

#endif
