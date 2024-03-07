#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#define ENTITY_TYPE_NAME_MAX 64

typedef enum {
    PLAYER_PEASANT,
    PLAYER_KNIGHT,
    PLAYER_PRIEST,
    GOBLIN,
    WOOD_ELF,
    HORSE
}BaseType;

typedef enum{
    AI_MONSTER,
    AI_NEUTRAL_ANIMAL,
    AI_TRADER
}AIProfile;

typedef struct AttributesTag{

    BaseType type;
    char type_name[64];
    
    int level;
    int xp;

    int cur_hp;
    int total_hp;
    
    int strength;
    int intelligence;
    int charisma;
    int agility;
    int perception;
    int zeal;
    int favor;
}Attributes;


#endif /* ATTRIBUTES_H */
