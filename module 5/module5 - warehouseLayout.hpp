#ifndef WAREHOUSELAYOUT_HPP
#define WAREHOUSELAYOUT_HPP

#include "types.hpp"

struct WarehouseNode {
    int  sectionId;                     // zone/aisle/shelf number
    char level;                         // 'R'=Root, 'Z'=Zone, 'A'=Aisle, 'S'=Shelf
    WarehouseNode* child[MAX_CHILDREN]; 
    int  childCount;                    

    WarehouseNode(int id, char lvl);
};

// Builds and queries the warehouse hierarchy (zone->aisle->shelf)
class WarehouseLayout {
private:
    WarehouseNode* root;

    void destroyTree(WarehouseNode* node);

    WarehouseNode* findOrCreateChild(WarehouseNode* parent, int id, char level);

    void insertPath(int zone, int aisle, int shelf);

    bool collectRouteDFS(WarehouseNode* current, Location target,
                         Location path[], int& pathCount);

    void displayHelper(WarehouseNode* node, int depth);

public:
    WarehouseLayout();
    ~WarehouseLayout();

    // Parse items.csv and build the tree
    void buildFromFile(const char* filename);

    bool generateRoute(Location target, Route& outRoute);

    void displayLayout();
};

#endif