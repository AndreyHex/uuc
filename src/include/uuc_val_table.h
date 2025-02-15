#ifndef uuc_val_table_h
#define uuc_val_table_h

#include "uuc_type.h"
#include <stddef.h>

typedef struct {
    UucString *key;
    Value value;
} Entry;

typedef struct {
    size_t size;
    size_t capacity;
    Entry *entries;
} UucValTable;

UucValTable uuc_init_val_table(size_t init_capacity);
Entry *uuc_val_table_get(UucValTable *table, UucString *key);
// returns 1 if values was overridden
int uuc_val_table_put(UucValTable *table, UucString *key, Value value);
// returns 1 if value was removed
int uuc_val_table_remove(UucValTable *table, UucString *key);

void uuc_val_table_free(UucValTable *table);
void uuc_val_table_dump(UucValTable *table);

#endif
