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

#if defined(_MSC_VER) && !defined(__clang__)
#define GET_RSP(_RESULT) __asm { mov _RESULT, rsp };
#else
#if defined(_WIN64)
#define GET_RSP(_RESULT) \
    __asm__ volatile ( \
        "movq %0, rsp\n"  \
        : "=r" (_RESULT)  \
        :  \
    );
#else
#define GET_RSP(_RESULT) \
    __asm__ volatile ( \
        "movq %%rsp, %0\n"  \
        : "=r" (_RESULT)  \
        :  \
    );
#endif
#endif

#define RETURN_UNKNOWN_VALUE(_VALUE, _TYPE, _TMP_NAME) \
    char* value_ptr##_TMP_NAME; \
    GET_RSP(value_ptr##_TMP_NAME); \
    value_ptr##_TMP_NAME -= ((2048 + sizeof(_VALUE) * 2) + 15) & ~15; \
    memcpy(value_ptr##_TMP_NAME, &(_VALUE), sizeof(_VALUE)); \
    return (struct UnknownValue) { .ptr = value_ptr##_TMP_NAME, .size = sizeof(_VALUE), .type=(_TYPE) };

#define CONSUME_UNKNOWN_VALUE(_VALUE, _TMP_NAME) \
    void* tmp_unknown_value##_TMP_NAME = alloca((_VALUE).size); \
    memcpy(tmp_unknown_value##_TMP_NAME, (_VALUE).ptr, (_VALUE).size); \
    (_VALUE).ptr = tmp_unknown_value##_TMP_NAME;

UnknownValue foo() {
    int a = 20;

    RETURN_UNKNOWN_VALUE(a, 0, a);
}

void foo_example() {
    UnknownValue val1 = foo();
    CONSUME_UNKNOWN_VALUE(val1, val1);
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
        RETURN_UNKNOWN_VALUE(i, VALUE_TYPE_INT, __LINE__);
    }
    if (iteration % 3 == 1) {
        float f = (float)iteration;
        RETURN_UNKNOWN_VALUE(f, VALUE_TYPE_FLOAT, __LINE__);
    }

    const char* str = "hello world!";
    RETURN_UNKNOWN_VALUE(str, VALUE_TYPE_C_STR, __LINE__);
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
        CONSUME_UNKNOWN_VALUE(val1, __LINE__);

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
