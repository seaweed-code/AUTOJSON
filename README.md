# JsonTransformAuto

[中文文档](./README.zh-CN.md)

A header-only C++ library for zero-boilerplate JSON serialization and deserialization. Declare a two-line macro in your struct and you get full JSON support automatically — no manual field mapping, no code generation, no external tools.

Built on [RapidJSON](https://github.com/Tencent/rapidjson). Compatible with **C++11**, **VS2013**, and **GCC 4.8.5**.

---

## Features

- Header-only — just `#include "JsonTransformAuto.h"`
- Works on any struct with a two-macro declaration
- Supports all C++ primitive types, `std::string`, nested structs, and `std::vector<T>` (including nested vectors)
- Type coercion: string `"42"` → `int`, number `100` → `std::string`, etc.
- Graceful error handling — malformed or mismatched JSON never crashes
- Missing fields preserve the original value in the struct
- Supports root-level JSON arrays via free function templates

---

## Requirements

- RapidJSON headers on the include path
- C++11 or later

---

## Quick Start

**1. Declare your struct**

```cpp
#include "JsonTransformAuto.h"

struct Person {
    std::string name;
    int32_t     age;
    DECLARE__JSON__AUTO__TRANSFORM   // inject serialize/deserialize methods
};

// At file scope (not inside a function)
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age)
```

**2. Deserialize from JSON string**

```cpp
Person p;
bool ok = p.transform_from_json("{\"name\":\"Alice\",\"age\":30}");
// ok == true, p.name == "Alice", p.age == 30
```

**3. Serialize to JSON string**

```cpp
Person p;
p.name = "Bob";
p.age  = 25;
std::string json = p.transform_to_json();
// json == {"name":"Bob","age":25}
```

---

## Supported Field Types

| Type | Notes |
|---|---|
| `int8_t`, `uint8_t` | Serialized as JSON integer |
| `int16_t`, `uint16_t` | Serialized as JSON integer |
| `int32_t`, `uint32_t` | Serialized as JSON integer |
| `int64_t`, `uint64_t` | Serialized as JSON integer |
| `float` | Serialized as JSON double |
| `double` | Serialized as JSON double |
| `bool` | Serialized as JSON `true`/`false` |
| `std::string` | Serialized as JSON string |
| Nested struct | Must also declare the two macros |
| `std::vector<T>` | Serialized as JSON array; `T` can be any supported type, including another struct or `std::vector<T>` |

> ⚠️ **`std::vector<bool>` is not supported.** The C++ standard specializes `std::vector<bool>` as a bit-packed container, which means its elements do not have addressable memory and cannot be used with `reinterpret_cast`. Use `std::vector<uint8_t>` instead — values `0` and `1` map cleanly to `false` and `true`.
>
> ```cpp
> // ❌ Do not use
> std::vector<bool> flags;
>
> // ✅ Use this instead
> std::vector<uint8_t> flags;  // 0 = false, 1 = true
> ```

> **Limit:** A single struct can have at most **20 fields**.

---

## API Reference

### Methods injected by `DECLARE__JSON__AUTO__TRANSFORM`

```cpp
// Deserialize from JSON string. Returns true if all fields were found.
bool transform_from_json(const std::string& json);

// Deserialize from an existing rapidjson::Value (must be an Object).
bool transform_from_json(const rapidjson::Value& value);

// Serialize to a JSON string.
std::string transform_to_json();

// Serialize into an existing rapidjson::Value and allocator.
// Useful for embedding this object inside a larger document.
void transform_to_json(rapidjson::Value& value,
                       rapidjson::Document::AllocatorType& allocator);
```

### Free functions for root-level JSON arrays

These live in the `auto_json` namespace and handle JSON whose root node is an array.

```cpp
// Deserialize a JSON array string into a vector.
// T can be a reflect type, a primitive, std::string, or std::vector<T>.
// Returns false if the JSON is invalid or the root is not an array.
template<typename T>
bool auto_json::transform_from_json(std::vector<T>& dest,
                                    const std::string& json);

// Serialize a vector into a JSON array string.
template<typename T>
std::string auto_json::transform_to_json(const std::vector<T>& src);
```

---

## Examples

### Nested structs

```cpp
struct Address {
    std::string city;
    int32_t     zip;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Address, city, zip)

struct Person {
    std::string name;
    int32_t     age;
    Address     addr;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age, addr)

// Usage
Person p;
p.transform_from_json(
    "{\"name\":\"Alice\",\"age\":30,"
    "\"addr\":{\"city\":\"Beijing\",\"zip\":100000}}");
```

### Struct containing a vector

```cpp
struct Department {
    std::string           name;
    std::vector<Employee> employees;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Department, name, employees)
```

### Nested vectors

```cpp
struct Matrix {
    std::string                       label;
    std::vector<std::vector<int32_t>> rows;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, label, rows)
```

### Root-level JSON array

```cpp
std::vector<Person> people;

// Deserialize
bool ok = auto_json::transform_from_json(people, "[{...},{...}]");

// Serialize
std::string json = auto_json::transform_to_json(people);
```

### Embed into a larger document

```cpp
Address addr;
addr.city = "Wuhan";
addr.zip  = 430000;

rapidjson::Document doc;
doc.SetObject();
auto& alloc = doc.GetAllocator();

rapidjson::Value addrVal(rapidjson::kObjectType);
addr.transform_to_json(addrVal, alloc);

doc.AddMember("addr", addrVal, alloc);
```

---

## Error Handling and Edge Cases

| Situation | Behavior |
|---|---|
| Invalid JSON string | Returns `false`, struct is unchanged |
| Root node is an array (single-object API) | Returns `false`, struct is unchanged |
| Root node is an object (array API) | Returns `false`, vector is unchanged |
| Missing field in JSON | Field keeps its original value; function returns `false` |
| Extra / unknown fields in JSON | Silently ignored |
| Field value is `null` | Primitive fields receive their zero/default value; nested object fields keep their original value |
| Field expects object, JSON gives array/string | Nested field keeps its original value |
| Field expects array, JSON gives number/object | Vector field is unchanged |
| Array element type mismatch | Element keeps its zero/default value; no crash |
| String `"42"` → integer field | Parsed with `std::stoll` / `std::stod`; bad strings return `0` |
| Number → `std::string` field | Converted with `std::to_string` |
| `bool` JSON literal → `std::string` field | Returns empty string (not `"true"`/`"false"`) |
| Empty array `[]` in a vector field | Vector is resized to 0; function returns `true` |

---

## Important Notes

**Structs must be declared at file or namespace scope.**

`DECLARE__JSON__AUTO__TRANSFORM` injects a `static` data member. C++ does not allow static members inside local (function-scoped) structs.

```cpp
// ✅ Correct — file scope
struct Foo {
    int x;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Foo, x)

// ❌ Wrong — local scope
void bar() {
    struct Foo { int x; DECLARE__JSON__AUTO__TRANSFORM };  // compile error
}
```

**`IMPLEMENT__JSON__AUTO__TRANSFORM` must appear exactly once per struct**, typically in a `.cpp` file (or a header if the project is header-only).

**Maximum 20 fields per struct.** Split larger structs into nested ones if needed.

---

## Type Coercion Rules

The library attempts reasonable conversions when the JSON type does not exactly match the field type.

| JSON value | Target C++ type | Result |
|---|---|---|
| `"42"` | any integer | `42` (via `std::stoll`) |
| `"3.14"` | `float`/`double` | `3.14` (via `std::stod`) |
| `"true"` or `"1"` | `bool` | `true` |
| `"false"` or `"0"` | `bool` | `false` |
| `1` / `0` | `bool` | `true` / `false` |
| `42` | `std::string` | `"42"` (via `std::to_string`) |
| `3.14` | `std::string` | `"3.14..."` (via `std::to_string`) |
| `true` / `false` | `std::string` | `""` (empty, not supported) |
| `"not_a_number"` | any number | `0` (exception caught) |
