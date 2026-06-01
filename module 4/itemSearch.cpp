#include "itemSearch.hpp"
#include <iostream>
int main()
{
    ItemSearchBST bst;

    bst.loadFromCSV("../assets/items.csv");

    bst.displayInOrder();

    // Item *itemId = bst.searchByID(55);
    // if (itemId != nullptr)
    // {
    //     cout << "Found item: " << itemId->name << endl;
    //     cout << "Location: "
    //          << itemId->location.zone << ", "
    //          << itemId->location.aisle << ", "
    //          << itemId->location.shelf << endl;
    // }
    // else
    // {
    //     cout << "Item not found" << endl;
    // }

    // Item *itemName = bst.searchByName("widget");
    // if (itemName != nullptr)
    // {
    //     cout << "Found item: " << itemName->name << endl;
    //     cout << "Location: "
    //          << itemName->location.zone << ", "
    //          << itemName->location.aisle << ", "
    //          << itemName->location.shelf << endl;
    // }
    // else
    // {
    //     cout << "Item not found" << endl;
    // }

    Location loc;
    loc.zone = 2;
    loc.aisle = 5;
    loc.shelf = 1;
    cout << "\n\n delete and update\n";
    bst.deleteItem(12);
    bst.updateItem(55, "Masturbator Cup", loc);
    bst.displayInOrder();

    return 0;
}
