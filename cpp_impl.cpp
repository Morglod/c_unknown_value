#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cstdint>

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

// simple example

UnknownValue foo() {
    int a = 20;
    return return_unknown_value(a, 0);
}

void foo_example() {
    UnknownValue val1 = foo();
    CONSUME_UNKNOWN_VALUE(val1);
    printf("val1: %i\n", *(int*)val1.ptr);
}

// demo

enum {
    VALUE_TYPE_INT,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_C_STR,
};

UnknownValue foobazbar(int iteration) {
    if (iteration % 3 == 0) {
        int i = iteration;
        return return_unknown_value(i, VALUE_TYPE_INT);
    }
    if (iteration % 3 == 1) {
        float f = (float)iteration;
        return return_unknown_value(f, VALUE_TYPE_FLOAT);
    }

    const char* str = "hello world!";
    return return_unknown_value(str, VALUE_TYPE_C_STR);
}

// output:
// 0 | int = 0
// 1 | float = 1.000000
// 2 | str = hello world!
// 3 | int = 3
// 4 | float = 4.000000
// 5 | str = hello world!
// 6 | int = 6
// 7 | float = 7.000000
// 8 | str = hello world!
// 9 | int = 9

int main() {
    for (int i = 0; i < 10; ++i) {
        UnknownValue val1 = foobazbar(i);
        CONSUME_UNKNOWN_VALUE(val1);

        switch (val1.type) {
            case VALUE_TYPE_INT:
                printf("%i | int = %i\n", i, *(int*)val1.ptr);
                break;
            case VALUE_TYPE_FLOAT:
                printf("%i | float = %f\n", i, *(float*)val1.ptr);
                break;
            case VALUE_TYPE_C_STR:
                printf("%i | str = %s\n", i, *(const char**)val1.ptr);
                break;
        }
    }
    return 0;
}
