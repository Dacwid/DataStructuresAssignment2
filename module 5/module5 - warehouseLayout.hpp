#ifndef WAREHOUSELAYOUT_HPP
#define WAREHOUSELAYOUT_HPP

#include "types.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
using namespace std;

//Node in the N-ary warehouse tree
struct WarehouseNode {
    int  sectionId;                     // zone/aisle/shelf number
    char level;                         // 'R'=Root, 'Z'=Zone, 'A'=Aisle, 'S'=Shelf
    WarehouseNode* child[MAX_CHILDREN]; 
    int  childCount;                    

    WarehouseNode(int id, char lvl) {
        sectionId = id;
        level = lvl;
        childCount = 0;
        for (int i = 0; i < MAX_CHILDREN; i++) {
            child[i] = nullptr;
        }
    }
};

//Builds and queries the warehouse hierarchy
class WarehouseLayout {
private:
    WarehouseNode* root;

    //Recursively delete all nodes
    void destroyTree(WarehouseNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < node->childCount; i++) {
            destroyTree(node->child[i]);
        }
        delete node;
    }

    //Find child with matching id
    WarehouseNode* findOrCreateChild(WarehouseNode* parent, int id, char lvl) {
        if (parent == nullptr) return nullptr;

        //Check if child already exists
        for (int i = 0; i < parent->childCount; i++) {
            if (parent->child[i]->sectionId == id) {
                return parent->child[i];
            }
        }

        if (parent->childCount >= MAX_CHILDREN) {
            cout << "Error: MAX_CHILDREN exceeded for node " << parent->sectionId
                 << " (level " << parent->level << "). Skipping insertion." << endl;
            return nullptr;
        }

        //Create new child
        WarehouseNode* newNode = new WarehouseNode(id, lvl);
        parent->child[parent->childCount] = newNode;
        parent->childCount++;
        return newNode;
    }

    //Insert a path into the tree
    void insertPath(int zone, int aisle, int shelf) {
        WarehouseNode* zoneNode = findOrCreateChild(root, zone, 'Z');
        if (zoneNode == nullptr) return;

        WarehouseNode* aisleNode = findOrCreateChild(zoneNode, aisle, 'A');
        if (aisleNode == nullptr) return;

        findOrCreateChild(aisleNode, shelf, 'S');
    }

    //DFS from current node to find target shelf, fills path[] on success
    bool collectRouteDFS(WarehouseNode* current, Location target,
                         Location path[], int& pathCount) {
        if (current == nullptr) return false;

        Location curLoc;
        if (current->level == 'R') {
            curLoc = {0, 0, 0};
        } else if (current->level == 'Z') {
            curLoc = {current->sectionId, 0, 0};
        } else if (current->level == 'A') {
            //Inherit zone from parent (last node in path)
            curLoc = {path[pathCount - 1].zone, current->sectionId, 0};
        } else if (current->level == 'S') {
            //Inherit zone and aisle from parent
            curLoc = {path[pathCount - 1].zone, path[pathCount - 1].aisle, current->sectionId};
        }

        //Add node to path
        if (pathCount >= MAX_ROUTE) return false;
        path[pathCount] = curLoc;
        pathCount++;

        //Check target shelf reached
        if (current->level == 'S' &&
            curLoc.zone == target.zone &&
            curLoc.aisle == target.aisle &&
            curLoc.shelf == target.shelf) {
            return true;
        }

        //Search children for the target
        if (current->level == 'R') {
            //Next level is zone
            for (int i = 0; i < current->childCount; i++) {
                if (current->child[i]->sectionId == target.zone) {
                    if (collectRouteDFS(current->child[i], target, path, pathCount))
                        return true;
                }
            }
        } else if (current->level == 'Z') {
            //Next level is aisle
            for (int i = 0; i < current->childCount; i++) {
                if (current->child[i]->sectionId == target.aisle) {
                    if (collectRouteDFS(current->child[i], target, path, pathCount))
                        return true;
                }
            }
        } else if (current->level == 'A') {
            //Next level is shelf
            for (int i = 0; i < current->childCount; i++) {
                if (current->child[i]->sectionId == target.shelf) {
                    if (collectRouteDFS(current->child[i], target, path, pathCount))
                        return true;
                }
            }
        }

        //Backtrack if not found
        pathCount--;
        return false;
    }

    //Recursive display
    void displayHelper(WarehouseNode* node, int depth) {
        if (node == nullptr) return;

        for (int i = 0; i < depth; i++) {
            cout << "    ";
        }

        const char* levelName = "";
        switch (node->level) {
            case 'R': levelName = "Warehouse"; break;
            case 'Z': levelName = "Zone"; break;
            case 'A': levelName = "Aisle"; break;
            case 'S': levelName = "Shelf"; break;
        }
        cout << levelName << " " << node->sectionId << endl;

        //Recurse into children
        for (int i = 0; i < node->childCount; i++) {
            displayHelper(node->child[i], depth + 1);
        }
    }

public:
    //Create empty tree
    WarehouseLayout() {
        root = new WarehouseNode(0, 'R');
    }

    //Destroy entire tree
    ~WarehouseLayout() {
        destroyTree(root);
    }

    //Parse items.csv and build the tree
    void buildFromFile(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file \"" << filename << "\"." << endl;
            return;
        }

        char line[256];
        while (file.getline(line, 256)) {
            if (strlen(line) == 0) continue;

            int itemID, zone, aisle, shelf;
            char name[64];

            //Format: itemID,name,zone,aisle,shelf
            if (sscanf(line, "%d,%63[^,],%d,%d,%d", &itemID, name, &zone, &aisle, &shelf) == 5) {
                insertPath(zone, aisle, shelf);
            } else {
                cout << "Warning: Could not parse line: " << line << endl;
            }
        }

        file.close();
    }

    //Generate route from root to target shelf
    bool generateRoute(Location target, Route& outRoute) {
        outRoute.count = 0;

        if (root->childCount == 0) return false;

        Location path[MAX_ROUTE];
        int pathCount = 0;

        if (collectRouteDFS(root, target, path, pathCount)) {
            for (int i = 0; i < pathCount; i++) {
                outRoute.stops[i] = path[i];
            }
            outRoute.count = pathCount;
            return true;
        }

        return false;
    }

    //Print the full warehouse hierarchy
    void displayLayout() {
        if (root->childCount == 0) {
            cout << "Warehouse layout is empty (no items loaded)." << endl;
            return;
        }
        displayHelper(root, 0);
    }
};

#endif