#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../include/uuc_string.h"

// creating new string object copying given length from src
UucString *uuc_copy_string(const char *src, size_t len) {
    UucString *r = malloc(sizeof(UucString) + sizeof(char[len+1])); // + '\0'
    memcpy(r->content, src, len);
    r->uuc_obj.type = OBJ_STRING;
    r->content[len] = '\0';
    r->length = len + 1;
    r->hash = uuc_hash_string(r);
    return r;
}

UucString *uuc_create_string(char *content) {
    return uuc_copy_string(content, strlen(content));
}

UucString *uuc_concate_strings(UucString *left, UucString *right) {
    size_t s = left->length + right->length - 1; // - one '\0'
    UucString *r = malloc(sizeof(UucString) + sizeof(char[s]));
    memcpy(r->content, left->content, left->length);
    memcpy(&r->content[left->length-1], right->content, right->length);
    r->uuc_obj.type = OBJ_STRING;
    r->length = s;
    r->hash = uuc_hash_string(r);
    return r;
}

void uuc_free_string(UucString *str) {
    free(str);
}

// FNV-1a 64bit
uint64_t uuc_hash_string(UucString *string) {
    uint64_t h = 14695981039346656037u;
    for(int i = 0; i < string->length; i++) {
        h ^= (uint8_t)string->content[i];
        h *= 1099511628211;
    }
    return h;
}
