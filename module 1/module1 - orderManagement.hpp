#ifndef ORDER_MANAGEMENT_HPP   
#define ORDER_MANAGEMENT_HPP   

#include "types.hpp"   


struct OrderQueue {
    Order orders[MAX_ORDERS];    
    int   frontIndex;            
    int   rearIndex;            
    int   count;                 

    Order assigned[MAX_ORDERS];  
    int   assignedCount;         
};

void  initQueue(OrderQueue& queue);                      
bool  isEmpty(OrderQueue& queue);                         
bool  isFull(OrderQueue& queue);                        
void  enqueueOrder(OrderQueue& queue, Order newOrder);   
Order dequeueOrder(OrderQueue& queue);                  
void  loadOrders(OrderQueue& queue, string filename);     
void  displayPending(OrderQueue& queue);                  
void  displayAssigned(OrderQueue& queue);                 
void  displayCurrent(OrderQueue& queue);                  

#endif 
