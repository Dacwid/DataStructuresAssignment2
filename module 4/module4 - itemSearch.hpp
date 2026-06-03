#ifndef ITEMSEARCH_HPP
#define ITEMSEARCH_HPP

#include "types.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

struct BSTNode
{
    Item data;
    BSTNode *left;
    BSTNode *right;

    BSTNode(const Item &item) 
    {
        data.itemID = item.itemID;
        data.name = item.name;
        data.location = item.location;
        left = nullptr;
        right = nullptr;
    }
};

class ItemSearchBST
{
private:
    BSTNode *root;

    bool insertItem(const Item &item)
    {
        BSTNode *newNode = new BSTNode(item);

        if (root == nullptr)
        {
            root = newNode;
            return true;
        }

        BSTNode *current = root;

        while (true)
        {
            if (item.itemID < current->data.itemID)
            {
                if (current->left == nullptr)
                {
                    current->left = newNode;
                    return true;
                }

                current = current->left;
            }
            else if (item.itemID > current->data.itemID)
            {
                if (current->right == nullptr)
                {
                    current->right = newNode;
                    return true;
                }

                current = current->right;
            }
            else
            {
                delete newNode;
                return false; // duplicate itemID
            }
        };
    }

    // in order
    void inOrderTraversal(BSTNode *node)
    {
        if (node == nullptr)
        {
            return;
        }

        inOrderTraversal(node->left);

        cout << "Item ID: " << node->data.itemID
             << ", Name: " << node->data.name
             << ", Location: ("
             << node->data.location.zone << ", "
             << node->data.location.aisle << ", "
             << node->data.location.shelf << ")"
             << endl;

        inOrderTraversal(node->right);
    };

    // pre order
    Item *searchByNameHelper(BSTNode *node, const string &name)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (node->data.name == name)
        {
            return &node->data;
        }

        Item *leftResult = searchByNameHelper(node->left, name);
        if (leftResult != nullptr)
        {
            return leftResult;
        }

        return searchByNameHelper(node->right, name);
    }

    
    BSTNode *deleteHelper(BSTNode *node, int itemID)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (itemID < node->data.itemID)
        {
            node->left = deleteHelper(node->left, itemID);
        }

        else if (itemID > node->data.itemID)
        {
            node->right = deleteHelper(node->right, itemID);
        }
        else
        {

            if (node->left == nullptr)
            {
                BSTNode *temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr)
            {
                BSTNode *temp = node->left;
                delete node;
                return temp;
            }

            BSTNode *temp = findMin(node->right);

            node->data.itemID = temp->data.itemID;
            node->data.name = temp->data.name;
            node->data.location = temp->data.location;

            node->right = deleteHelper(node->right, temp->data.itemID);
        }

        return node;
    }

    BSTNode *findMin(BSTNode *node)
    {
        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    // post order
    void deleteTree(BSTNode *node)
    {
        if (node == nullptr)
        {
            return;
        }

        deleteTree(node->left);

        deleteTree(node->right);

        delete node;
    }

public:
    ItemSearchBST()
    {
        root = nullptr;
    }

    bool loadFromCSV(const string &filename)
    {
        ifstream file(filename);

        if (!file.is_open())
        {
            cout << "Failed to open file." << endl;
            return false;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);
            string token;

            Item item;

            getline(ss, token, ',');
            item.itemID = stoi(token);

            getline(ss, item.name, ',');

            getline(ss, token, ',');
            item.location.zone = stoi(token);

            getline(ss, token, ',');
            item.location.aisle = stoi(token);

            getline(ss, token, ',');
            item.location.shelf = stoi(token);

            insertItem(item);
        }

        file.close();
        return true;
    }

    void displayInOrder()
    {
        if (root == nullptr)
        {
            cout << "No items in BST." << endl;
            return;
        }

        inOrderTraversal(root);
    }

    // pre order
    Item *searchByID(int itemID)
    {
        BSTNode *current = root;

        while (current != nullptr)
        {
            if (itemID == current->data.itemID)
            {
                return &current->data;
            }
            else if (itemID < current->data.itemID)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        return nullptr;
    }

    Item *searchByName(const string &name)
    {
        return searchByNameHelper(root, name);
    }

    bool deleteItem(int itemID)
    {
        if (searchByID(itemID) == nullptr)
        {
            return false;
        }

        root = deleteHelper(root, itemID);
        return true;
    }

    bool updateItem(int itemID, const string &newName, Location newLocation)
    {
        Item *item = searchByID(itemID);

        if (item == nullptr)
        {
            cout << "Item not found." << endl;
            return false;
        }

        item->name = newName;
        item->location = newLocation;

        return true;
    }

    ~ItemSearchBST()
    {
        deleteTree(root);
        root = nullptr;
    }
};

#endif