#include "../include/uuc_val_table.h"
#include "../include/uuc_memory.h"
#include <stdlib.h>

#define UUC_TABLE_MAX_LOAD 0.7

void uuc_val_table_increase(UucValTable *table);
Entry *uuc_val_table_get_(Entry *entries, UucString *key, size_t cap);

UucValTable uuc_init_val_table(size_t init_capacity) {
    Entry *p = malloc(sizeof(Entry) * init_capacity);
    return (UucValTable){ 
        .size = 0,
        .capacity = init_capacity,
        .entries = p,
    };
}

int uuc_val_table_put(UucValTable *table, UucString *key, Value val) {
    Entry *e = uuc_val_table_get(table, key);
    int new = e == NULL || e->key == NULL;
    if(new) table->size++;
    *e = (Entry){ .key = key, .value = val };
    return new;
}

Entry *uuc_val_table_get(UucValTable *table, UucString *key) {
    size_t capacity = table->capacity;
    return uuc_val_table_get_(table->entries, key, capacity);
}

int uuc_val_table_remove(UucValTable *table, UucString *key) {
    Entry *e = uuc_val_table_get(table, key);
    if(e == 0 || e->key == NULL) return 0;
    e->key = NULL;
    table->size--;
    return 1;
}

void uuc_val_table_increase(UucValTable *table) {
    size_t new_cap = INCREASE_CAPACITY(table->capacity);
    Entry *new_e = malloc(sizeof(Entry) * new_cap);
    Entry *old_e = table->entries;
    table->entries = new_e;
    table->capacity = new_cap;
    for(int i = 0; i < table->capacity; i++) {
        Entry *e = &old_e[i];
        if(e->key != NULL) uuc_val_table_put(table, e->key, e->value);
    }
    free(old_e);
}

Entry *uuc_val_table_get_(Entry *entries, UucString *key, size_t cap) {
    size_t index = key->hash % cap;
    while(1) {
        Entry *e = &entries[index];
        if(e->key == key || e->key == NULL) return e;
        index = (index + 1) % cap;
    }
    return NULL;
}

void uuc_val_table_free(UucValTable *table) {
    free(table->entries);
}

void uuc_val_table_dump(UucValTable *table) {
    for(int i = 0; i < table->capacity; i++) {
        Entry *r = &table->entries[i];
        if(r->key == NULL) continue;
        printf("%10s -> ", r->key->content);
        type_print(r->value);
        printf("\n");
    }
}
