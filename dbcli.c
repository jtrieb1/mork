#include "src/coredb/db.h"
#include "src/coredb/tables/character.h"
#include "src/coredb/tables/description.h"
#include <stdio.h>

int location_create_menu(struct Database *db)
{
    printf("Create Location\n");

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    char description[MAX_DESCRIPTION];
    printf("Description: ");
    scanf("%s", description);

    struct DescriptionRecord *desc = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), description, 0);
    struct LocationRecord *record = LocationRecord_create(name, desc->id);
    Database_createLocation(db, record);

    DescriptionRecord_destroy(desc);

    return 1;
}

int location_list(struct Database *db)
{
    printf("List Locations\n");

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        struct LocationTable *ltable = Database_get(db, LOCATIONS);
        struct LocationRecord record = ltable->locations[i];
        printf("%d: %s\n", record.id, record.name);
    }

    return 1;
}

int location_update_menu(struct Database *db)
{
    printf("Update Location\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    struct LocationRecord *record = Database_getLocation(db, id);
    if (record == NULL)
    {
        printf("Location not found.\n");
        return 1;
    }

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    char description[MAX_DESCRIPTION];
    printf("Description: ");
    scanf("%s", description);

    struct DescriptionRecord *desc = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), description, 0);
    struct LocationRecord *new_record = LocationRecord_create(name, desc->id);
    Database_updateLocation(db, new_record, id);

    DescriptionRecord_destroy(desc);

    return 1;
}

int location_delete_menu(struct Database *db)
{
    printf("Delete Location\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    Database_deleteLocation(db, id);

    return 1;
}

int location_menu(struct Database *db)
{
    printf("Location Menu\n");
    printf("1. Create\n");
    printf("2. List\n");
    printf("3. Update\n");
    printf("4. Delete\n");
    printf("5. Quit\n");

    int choice = 0;
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            return location_create_menu(db);
        case 2:
            return location_list(db);
            break;
        case 3:
            return location_update_menu(db);
        case 4:
            return location_delete_menu(db);
            break;
        case 5:
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
    }

    return 1;
}

int character_create(struct Database *db)
{
    printf("Create Character\n");

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    unsigned char level = 0;
    printf("Level: ");
    scanf("%hhu", &level);

    unsigned short max_health = 0;
    printf("Max Health: ");
    scanf("%hu", &max_health);

    unsigned short max_mana = 0;
    printf("Max Mana: ");
    scanf("%hu", &max_mana);

    unsigned short strength = 0;
    printf("Strength: ");
    scanf("%hu", &strength);

    unsigned short dexterity = 0;
    printf("Dexterity: ");
    scanf("%hu", &dexterity);

    unsigned short intelligence = 0;
    printf("Intelligence: ");
    scanf("%hu", &intelligence);

    unsigned short wisdom = 0;
    printf("Wisdom: ");
    scanf("%hu", &wisdom);

    unsigned short charisma = 0;
    printf("Charisma: ");
    scanf("%hu", &charisma);

    unsigned short funkiness = 0;
    printf("Funkiness: ");
    scanf("%hu", &funkiness);

    struct CharacterRecord *record = CharacterRecord_create(
        name,
        level,
        max_health, max_health,
        max_mana, max_health,
        (unsigned char[6]){strength, dexterity, intelligence, wisdom, charisma, funkiness},
        6
    );

    Database_createCharacter(db, record);

    return 1;
}

int character_read(struct Database *db)
{
    printf("Read Character\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    struct CharacterRecord *record = Database_getCharacter(db, id);
    if (record == NULL)
    {
        printf("Character not found.\n");
        return 1;
    }

    printf("Name: %s\n", record->name);
    printf("Level: %hhu\n", record->level);
    printf("Health: %lu/%lu\n", GET_HEALTH(record->health_and_mana), GET_HEALTH(record->max_health_and_mana));
    printf("Mana: %lu/%lu\n", GET_MANA(record->health_and_mana), GET_MANA(record->max_health_and_mana));
    printf("Strength: %llu\n", GET_STAT(record->stats, 0));
    printf("Dexterity: %llu\n", GET_STAT(record->stats, 1));
    printf("Intelligence: %llu\n", GET_STAT(record->stats, 2));
    printf("Wisdom: %llu\n", GET_STAT(record->stats, 3));
    printf("Charisma: %llu\n", GET_STAT(record->stats, 4));
    printf("Funkiness: %llu\n", GET_STAT(record->stats, 5));

    return 1;
}

int character_update(struct Database *db)
{
    printf("Update Character\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    struct CharacterRecord *record = Database_getCharacter(db, id);
    if (record == NULL)
    {
        printf("Character not found.\n");
        return 1;
    }

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    unsigned char level = 0;
    printf("Level: ");
    scanf("%hhu", &level);

    unsigned short max_health = 0;
    printf("Max Health: ");
    scanf("%hu", &max_health);

    unsigned short max_mana = 0;
    printf("Max Mana: ");
    scanf("%hu", &max_mana);

    unsigned short strength = 0;
    printf("Strength: ");
    scanf("%hu", &strength);

    unsigned short dexterity = 0;
    printf("Dexterity: ");
    scanf("%hu", &dexterity);

    unsigned short intelligence = 0;
    printf("Intelligence: ");
    scanf("%hu", &intelligence);

    unsigned short wisdom = 0;
    printf("Wisdom: ");
    scanf("%hu", &wisdom);

    unsigned short charisma = 0;
    printf("Charisma: ");
    scanf("%hu", &charisma);

    unsigned short funkiness = 0;
    printf("Funkiness: ");
    scanf("%hu", &funkiness);

    struct CharacterRecord *new_record = CharacterRecord_create(
        name,
        level,
        max_health, max_health,
        max_mana, max_health,
        (unsigned char[6]){strength, dexterity, intelligence, wisdom, charisma, funkiness},
        6
    );

    Database_createInventory(db, name); // Make sure to create an inventory for the character

    Database_updateCharacter(db, new_record, id);

    return 1;
}

int character_delete(struct Database *db)
{
    printf("Delete Character\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    Database_deleteCharacter(db, id);

    return 1;
}

int character_menu(struct Database *db)
{
    printf("Character Menu\n");
    printf("1. Create\n");
    printf("2. Read\n");
    printf("3. Update\n");
    printf("4. Delete\n");
    printf("5. Quit\n");

    int choice = 0;
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            character_create(db);
            break;
        case 2:
            character_read(db);
            break;
        case 3:
            character_update(db);
            break;
        case 4:
            character_delete(db);
            break;
        case 5:
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
    }

    return 1;
}

int item_create(struct Database *db)
{
    printf("Create Item\n");

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    char description[MAX_DESCRIPTION];
    printf("Description: ");
    scanf("%s", description);

    struct DescriptionRecord *desc_record = DescriptionRecord_create(
        Database_getNextIndex(db, DESCRIPTION),
        description,
        0
    );

    struct ItemRecord *record = ItemRecord_create(
        Database_getNextIndex(db, ITEMS),
        name,
        desc_record->id
    );

    Database_createItem(db, record);

    return 1;
}

int item_read(struct Database *db)
{
    printf("Read Item\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    struct ItemRecord *record = Database_getItem(db, id);
    if (record == NULL)
    {
        printf("Item not found.\n");
        return 1;
    }

    struct DescriptionRecord *desc_record = Database_getDescription(db, record->description_id);
    if (desc_record == NULL)
    {
        printf("Description not found.\n");
        return 1;
    }

    printf("Name: %s\n", record->name);
    printf("Description: %s\n", desc_record->description);

    return 1;
}

int item_update(struct Database *db)
{
    printf("Update Item\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    struct ItemRecord *record = Database_getItem(db, id);
    if (record == NULL)
    {
        printf("Item not found.\n");
        return 1;
    }

    char name[100];
    printf("Name: ");
    scanf("%s", name);

    char description[MAX_DESCRIPTION];
    printf("Description: ");
    scanf("%s", description);

    struct DescriptionRecord *desc_record = DescriptionRecord_create(
        Database_getNextIndex(db, DESCRIPTION),
        description,
        0
    );

    struct ItemRecord *new_record = ItemRecord_create(
        Database_getNextIndex(db, ITEMS),
        name,
        desc_record->id
    );

    Database_updateItem(db, new_record, id);

    return 1;
}

int item_delete(struct Database *db)
{
    printf("Delete Item\n");

    unsigned short id;
    printf("ID: ");
    scanf("%hu", &id);

    Database_deleteItem(db, id);

    return 1;
}

int item_menu(struct Database *db)
{
    printf("Item Menu\n");
    printf("1. Create\n");
    printf("2. Read\n");
    printf("3. Update\n");
    printf("4. Delete\n");
    printf("5. Quit\n");

    int choice = 0;
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            item_create(db);
            break;
        case 2:
            item_read(db);
            break;
        case 3:
            item_update(db);
            break;
        case 4:
            item_delete(db);
            break;
        case 5:
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
    }

    return 1;
}


int table_menu(struct Database *db)
{
    printf("Table Menu\n");
    printf("1. Locations\n");
    printf("2. Characters\n");
    printf("3. Items\n");
    printf("4. Quit\n");

    int choice = 0;
    printf("> ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            location_menu(db);
            break;
        case 2:
            character_menu(db);
            break;
        case 3:
            item_menu(db);
            break;
        case 4:
            return 0;
        default:
            printf("Invalid choice.\n");
            break;
    }

    return 1;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <db file>\n", argv[0]);
        return 1;
    }

    struct Database *db = Database_create();
    Database_open(db, argv[1]);

    if (db == NULL)
    {
        printf("Failed to open database.\n");
        return 1;
    }

    printf("Database opened successfully.\n");

    int running = 1;
    while (running)
    {
        running = table_menu(db);

    }

    Database_destroy(db);
}
