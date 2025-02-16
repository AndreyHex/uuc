#ifndef uuc_val_table_h
#define uuc_val_table_h

#include "uuc_type.h"
#include <stdint.h>
#include <stddef.h>

typedef enum {
    NODE_EMPTY,
    NODE_ENTRY,
    NODE_BUCKET,
} NodeType;

typedef struct {
    UucString *key;
    Value value;
} Entry;

typedef struct {  
    uint32_t size;
    uint32_t capacity;
    Entry *entries;
} Bucket;

typedef struct {
    NodeType type;
    union {
        Entry entry;
        Bucket bucket;
    } as;
} TabNode;

typedef struct {
    size_t size;
    size_t node_capacity;
    TabNode *nodes;
} UucValTable;

UucValTable uuc_init_val_table(size_t init_capacity);
// return 1 if value was found
int uuc_val_table_get(UucValTable *table, UucString *key, Value *v);
// returns 1 if value was overridden
int uuc_val_table_put(UucValTable *table, UucString *key, Value value);
// returs 1 if value was removed
int uuc_val_table_remove(UucValTable *table, UucString *key);

void uuc_val_table_free(UucValTable *table);
void uuc_val_table_dump(UucValTable *table);
void uuc_val_table_summary_dump(UucValTable *table);

#endif
