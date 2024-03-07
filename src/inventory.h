#ifndef INVENTORY_H
#define INVENTORY_H


typedef enum {
    DAGGER,
    BOW
}ItemType;

typedef struct InventoryNodetag{
    ItemType type;
    int qty;
    InventoryNode *prev;
    InventoryNode *next;
}InventoryNode;


#endif /* INVENTORY_H */
