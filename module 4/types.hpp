#ifndef TYPES_HPP
#define TYPES_HPP

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
    std::string name;
    Location location;
};

#endif