# c_unknown_value

Experiments on idea of moving any type value without heap allocations.

## Why?

May be it could be used in intepretators

## Current working example:

```cpp
enum ValueType {
  ValueType_INT,
  ValueType_FLOAT
};

UnknownValue foo(ValueType vt) {
    if (vt == ValueType_INT) {
      int a = 20;
      return return_unknown_value(a, ValueType_INT);
    } else {
      float a = 12.34f;
      return return_unknown_value(a, ValueType_FLOAT);
    }
}

int main() {
    UnknownValue val1 = foo(ValueType_INT);
    CONSUME_UNKNOWN_VALUE(val1);

    UnknownValue val2 = foo(ValueType_FLOAT);
    CONSUME_UNKNOWN_VALUE(val2);

    printf("val1 type=%lu value=%i\n", val1.type, *(int*)val1.ptr);
    printf("val2 type=%lu value=%f\n", val2.type, *(float*)val2.ptr);

    // output:
    // val1 type=0 value=20
    // val2 type=1 value=12.340000

    return 0;
}
```
