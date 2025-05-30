# c_unknown_value

Experiments on idea of moving any type value without heap allocations.

## Why?

May be it could be used in intepretators

* its 2x-3x times faster than heap allocation
* 7x times faster than std::any
* works with -O3

## Current working example:

```cpp
#include "unknown_value.hpp"
#include <stdio.h>

enum ValueType {
  ValueType_INT,
  ValueType_FLOAT,
  ValueType_std_string,
  ValueType_std_vector
};

UnknownValue foo(ValueType vt) {
    if (vt == ValueType_INT) {
      int a = 20;
      return return_unknown_value(a, ValueType_INT);
    }
    if (vt == ValueType_FLOAT) {
      float a = 12.34f;
      return return_unknown_value(a, ValueType_FLOAT);
    }
    if (vt == ValueType_std_string) {
      std::string str = "hello world";
      return return_unknown_value(str, ValueType_std_string);
    }
    if (vt == ValueType_std_vector) {
      std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
      return return_unknown_value(vec, ValueType_std_vector);
    }
}

int main() {
    UnknownValue val1 = foo(ValueType_INT);
    CONSUME_UNKNOWN_VALUE(val1);

    UnknownValue val2 = foo(ValueType_FLOAT);
    CONSUME_UNKNOWN_VALUE(val2);
    
    UnknownValue val3 = foo(ValueType_std_string);
    CONSUME_UNKNOWN_VALUE(val3);
    std::string val3_str = *(std::string*)val3.ptr;

    UnknownValue val4 = foo(ValueType_std_vector);
    CONSUME_UNKNOWN_VALUE(val4);
    std::vector<int> val4_vec = *(std::vector<int>*)val4.ptr;

    printf("val1 type=%lu value=%i\n", val1.type, *(int*)val1.ptr);
    printf("val2 type=%lu value=%f\n", val2.type, *(float*)val2.ptr);
    printf("val3 type=%lu value=%s\n", val3.type, val3_str.c_str());
    printf("val4 type=%lu value={ %i %i %i %i %i %i }\n", val4.type, val4_vec[0], val4_vec[1], val4_vec[2], val4_vec[3], val4_vec[4], val4_vec[5]);

    // Output
    // val1 type=0 value=20
    // val2 type=1 value=12.340000
    // val3 type=2 value=hello world
    // val4 type=3 value={ 1 2 3 4 5 6 }

    return 0;
}

```
