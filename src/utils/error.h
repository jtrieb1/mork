#pragma once

enum MorkResult {
    MORK_OK = 0,

    // DB Errors
    MORK_ERROR_DB, // Generic database error
    MORK_ERROR_DB_NULL, // Database is NULL
    MORK_ERROR_DB_FILE_NULL, // Database file is NULL
    MORK_ERROR_DB_TABLE_NULL, // Database table is NULL
    MORK_ERROR_DB_RECORD_NULL, // Database record is NULL
    MORK_ERROR_DB_NOT_FOUND, // Database record not found
    MORK_ERROR_DB_FULL, // Database is full
    MORK_ERROR_DB_TABLE_FULL, // Database table is full
    MORK_ERROR_DB_FIELD_FULL, // Database field is full
    MORK_ERROR_DB_INVALID_ID, // ID was invalid (usually means ID was 0)
    MORK_ERROR_DB_INVALID_DATA, // Data was invalid
    MORK_ERROR_DB_INVALID_PATH, // Path was invalid

    MORK_ERROR_DB_FILE_SEEK, // Error seeking in file
    MORK_ERROR_DB_FILE_READ, // Error reading from file
    MORK_ERROR_DB_FILE_WRITE, // Error writing to file

    MORK_ERROR_DB_FAILED_CREATE, // Failed to create record

    // Model Errors
    MORK_ERROR_MODEL, // Generic model error
    MORK_ERROR_MODEL_ITEM_NULL, // Item is NULL
    MORK_ERROR_MODEL_INVENTORY_FULL, // Inventory is full
    MORK_ERROR_MODEL_INVENTORY_ITEM_NOT_FOUND, // Item not found in inventory
};