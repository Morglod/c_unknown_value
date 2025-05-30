// example:
//
// UnknownValue foo() {
//     int a = 20;
//     return return_unknown_value(a, 0);
// }
//
// void foo_example() {
//     UnknownValue val1 = foo();
//     CONSUME_UNKNOWN_VALUE(val1);
//     printf("val1: %i\n", *(int*)val1.ptr);
// }

#pragma once

#include <memory>

struct UnknownValue {
    void* ptr;
    size_t size;
    size_t type;
};
typedef struct UnknownValue UnknownValue;

template<typename T>
__attribute__((noinline, noclone))
UnknownValue return_unknown_value(T value, size_t type) {
    volatile char data[2048 + sizeof(T)];
    __asm__ volatile ("" ::: "memory");
    *((volatile T*)((volatile char*)data + 2048)) = value;
    return (struct UnknownValue) { .ptr = (void*)((char*)data + 2048), .size = sizeof(T), .type=type };
}

#define CONSUME_UNKNOWN_VALUE(_VAR_NAME) \
    void* tmp_unknown_value##_VAR_NAME = alloca((_VAR_NAME).size); \
    memcpy(tmp_unknown_value##_VAR_NAME, (_VAR_NAME).ptr, (_VAR_NAME).size); \
    (_VAR_NAME).ptr = tmp_unknown_value##_VAR_NAME; \
    __asm__ volatile ("" ::: "memory");
