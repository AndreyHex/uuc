#include "../include/uuc_val_table.h"
#include "../include/uuc_memory.h"
#include "../include/uuc_log.h"
#include "../include/uuc_type_print.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UUC_TABLE_MAX_LOAD 0.7
#define UUC_TABLE_MIN_LOAD 0.1
#define UUC_TABLE_BUCKET_INIT_CAP 8

void uuc_val_table_increase(UucValTable *table);


Entry *uuc_val_table_get_(TabNode *nodes, UucString *key, size_t cap);
void uuc_val_table_change_capacity_(UucValTable *table, size_t new_cap);
void uuc_val_table_bucket_create_(TabNode *node, Entry entry);
// returns 1 if entry was overridden
int uuc_val_table_bucket_put_(TabNode *node, Entry entry);
void uuc_val_table_free_nodes_(TabNode *nodes, size_t amount);

UucValTable uuc_init_val_table(size_t init_capacity) {
    TabNode *p = malloc(sizeof(TabNode) * init_capacity);
    for(int i = 0; i < init_capacity; i++) p[i].type = NODE_EMPTY;
    return (UucValTable){ 
        .size = 0,
        .node_capacity = init_capacity,
        .nodes = p,
    };
}

int uuc_val_table_put(UucValTable *table, UucString *key, Value val) {
    if(table->size / (float)table->node_capacity > UUC_TABLE_MAX_LOAD) {
        LOG_TRACE("Hashtable current size: '%ld' load: '%f'. Increasing capacity.\n", table->size, table->size / (float)table->node_capacity);
        size_t new_cap = INCREASE_CAPACITY(table->node_capacity);
        uuc_val_table_change_capacity_(table, new_cap);
    }
    size_t index = key->hash % table->node_capacity;
    TabNode *n = &table->nodes[index];
    if(n->type == NODE_EMPTY) {
        *n = (TabNode){
            .type = NODE_ENTRY, 
            .as ={.entry = (Entry){ .key = key, .value = val}}
        };
        table->size++;
        return 0;
    }
    if(n->type == NODE_ENTRY) {
        Entry *e = &n->as.entry;
        if(e->key == key || 0 == strcmp(e->key->content, key->content)) {
            *e = (Entry){.key = key, .value = val};
            return 1;
        } else {
            uuc_val_table_bucket_create_(n, (Entry){.key = key, .value = val});
            table->size++;
            return 0;
        }
    }
    if(n->type == NODE_BUCKET) {
        int overridden = uuc_val_table_bucket_put_(n, (Entry){.key = key, .value = val});
        if(overridden) return 1;
        else {
            table->size++;
            return 0;
        };
    }
    return 0; // should never happen
}

void uuc_val_table_bucket_create_(TabNode *node, Entry entry) {
    Entry *e = malloc(sizeof(Entry)*UUC_TABLE_BUCKET_INIT_CAP);
    Entry prev_e = node->as.entry;
    Bucket b = { .size = 2, .capacity = UUC_TABLE_BUCKET_INIT_CAP, .entries = e };
    b.entries[0] = prev_e;
    b.entries[1] = entry;
    node->type = NODE_BUCKET;
    node->as.bucket = b;
}

int uuc_val_table_bucket_put_(TabNode *node, Entry entry) {
    Bucket *b = &node->as.bucket;
    for(int i = 0; i < b->size; i++) {
        Entry *e = &b->entries[i];
        if(e->key == entry.key || !strcmp(e->key->content, entry.key->content)) {
            *e = entry;
            return 1;
        }
    }
    if(b->size == b->capacity) {
        uint32_t new_cap = INCREASE_CAPACITY(b->capacity);
        b->entries = INCREASE_ARRAY(Entry, b->entries, b->capacity, new_cap);
        b->capacity = new_cap;
    }
    b->entries[b->size] = entry;
    b->size++;
    return 0;
}

int uuc_val_table_get(UucValTable *table, UucString *key, Value *v) {
    size_t capacity = table->node_capacity;
    Entry *e = uuc_val_table_get_(table->nodes, key, capacity);
    if(e != NULL && e->key != NULL) {
        *v = e->value;
        return 1;
    } else return 0;
}

int uuc_val_table_remove__(UucValTable *table, UucString *key) {
    size_t index = key->hash % table->node_capacity;
    TabNode *e = &table->nodes[index];
    switch(e->type) {
        case NODE_EMPTY: return 0;
        case NODE_ENTRY: {
            e->as.entry.key = NULL;
            e->type = NODE_EMPTY;
            table->size--;
            return 1;
        }
        case NODE_BUCKET: {
            int r = 0;
            Bucket *b = &e->as.bucket;
            for(int i = 0; i < b->size; i++) {
                Entry *e = &b->entries[i];
                if(e->key == key || strcmp(key->content, e->key->content) == 0) {
                    r = 1;
                    if(i == b->size-1) b->size--; // last -- just decrement
                    else { // swap last -> this pos
                        *e = b->entries[b->size-1];
                        b->size--;
                    }
                    break;
                }
            }
            if(b->size == 1) {
                e->type = NODE_ENTRY;
                e->as.entry = b->entries[0];
                free(b->entries);
            }
            if(r) table->size--;
            return r;
        }
        default: return 0;
    }
}

int uuc_val_table_remove(UucValTable *table, UucString *key) {
    int r = uuc_val_table_remove__(table, key);
    if((table->size > 32) && table->size / (float)table->node_capacity < UUC_TABLE_MIN_LOAD) {
        LOG_TRACE("Hashtable current size: '%ld' load: '%f'. Decreasing capacity.\n", table->size, table->size / (float)table->node_capacity);
        uuc_val_table_change_capacity_(table, table->node_capacity / 4);
    }
    return r;
}

void uuc_val_table_change_capacity_(UucValTable *table, size_t new_cap) {
    TabNode *new_n = malloc(sizeof(TabNode) * new_cap);
    TabNode *old_n = table->nodes;
    for(int i = 0; i < new_cap; i++) new_n[i].type = NODE_EMPTY;
    table->nodes = new_n;
    size_t old_cap = table->node_capacity;
    table->node_capacity = new_cap;
    table->size = 0;
    for(int i = 0; i < old_cap; i++) {
        TabNode *n = &old_n[i];
        if(n->type == NODE_EMPTY) continue;
        if(n->type == NODE_ENTRY) {
            Entry e = n->as.entry;
            uuc_val_table_put(table, e.key, e.value);
        } else if(n->type == NODE_BUCKET) {
            Bucket b = n->as.bucket;
            for(int i = 0; i < b.size; i++) {
                Entry e = b.entries[i];
                if(e.key != NULL) uuc_val_table_put(table, e.key, e.value);
            }
        }
    }
    uuc_val_table_free_nodes_(old_n, old_cap);
}

Entry *uuc_val_table_get_(TabNode *nodes, UucString *key, size_t cap) {
    size_t index = key->hash % cap;
    TabNode *e = &nodes[index];
    if(e->type == NODE_EMPTY) return NULL;
    if(e->type == NODE_ENTRY) return &e->as.entry;
    if(e->type == NODE_BUCKET) {
        Bucket *b = &e->as.bucket;
        for(int i = 0; i < b->size; i++) {
            Entry *e = &b->entries[i];
            if(e->key == key || strcmp(key->content, e->key->content) == 0)
                return e;
        }
    }
    return NULL;
}

void uuc_val_table_free_nodes_(TabNode *nodes, size_t amount) {
    for(int i = 0; i < amount; i++) {
        TabNode *n = &nodes[i];
        if(n->type == NODE_BUCKET) free(n->as.bucket.entries);
    }
}

void uuc_val_table_free(UucValTable *table) {
    uuc_val_table_free_nodes_(table->nodes, table->node_capacity);
    free(table->nodes);
}

void uuc_val_table_dump_entry_(Entry *e) {
    if(e->key == NULL) {
        printf("!    NULL -> \n"); return;
    }
    printf("%5s -> ", e->key->content);
    uuc_val_print(e->value);
    printf("\n");
}

void uuc_val_table_dump_node(TabNode *node) {
    if(node->type == NODE_ENTRY) {
        Entry e = (Entry)node->as.entry;
        uuc_val_table_dump_entry_(&e);
    } else if(node->type == NODE_BUCKET) {
        Bucket b = (Bucket)node->as.bucket;
        for(int i = 0; i < b.size; i++) {
            uuc_val_table_dump_entry_(&b.entries[i]);
        }
    }
}

void uuc_val_table_dump(UucValTable *table) {
    printf("====== HASH TABLE DUMP ======\n");
    printf("Nodes: %ld Size: %ld Load: %f\n", table->node_capacity, table->size, table->size / (float)table->node_capacity);
    for(int i = 0; i < table->node_capacity; i++) {
        TabNode *r = &table->nodes[i];
        uuc_val_table_dump_node(r);
    }
    printf("====== =============== ======\n");
}

void uuc_val_table_summary_dump(UucValTable *table) {
    int b_size = 0;
    int b_items = 0;
    int empty = 0;
    int entries = 0;
    for(int i = 0; i < table->node_capacity; i++) {
        TabNode n = table->nodes[i];
        if(n.type == NODE_BUCKET) {
            b_size++;
            b_items += n.as.bucket.size;
        } else if(n.type == NODE_ENTRY) entries++;
        else if(n.type == NODE_EMPTY) empty++;
    }
    printf("Summary hashtable dump\n");
    printf("  Size: %ld\n", table->size);
    printf("  Capacity: %ld\n", table->node_capacity);
    printf("  Load: %f\n", table->size / (float)table->node_capacity);
    printf("  Empty nodes: %d\n", empty);
    printf("  Entry nodes: %d\n", entries);
    printf("  Buckets: %d\n", b_size);
    printf("  Items in buckes: %d\n", b_items);
}
