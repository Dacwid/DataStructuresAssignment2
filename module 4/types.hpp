#ifndef TYPES_HPP
#define TYPES_HPP
using namespace std;
#include <string>

struct Location
{
    int zone;
    int aisle;
    int shelf;
};

struct Item
{
    int itemID;
    string name;
    Location location;
};

#endif