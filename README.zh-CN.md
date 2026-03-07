# JsonTransformAuto

[English](./README.md)

纯头文件 C++ JSON 序列化/反序列化库，零样板代码。只需在结构体中声明两行宏，即可自动获得完整的 JSON 支持 —— 无需手动映射字段，无需代码生成，无需外部工具。

基于 [RapidJSON](https://github.com/Tencent/rapidjson)。兼容 **C++11**、**VS2013** 和 **GCC 4.8.5**。

---

## 特性

- 纯头文件 —— 只需 `#include "JsonTransformAuto.h"`
- 任意结构体声明两行宏即可使用
- 支持所有 C++ 基础类型、`std::string`、嵌套结构体、`std::vector<T>`（包括嵌套 vector）
- 类型自动转换：字符串 `"42"` → `int`，数字 `100` → `std::string` 等
- 容错处理 —— 格式错误或类型不匹配的 JSON 不会导致崩溃
- 缺失字段保留结构体原有值
- 通过函数模板支持根节点为数组的 JSON

---

## 依赖

- RapidJSON 头文件在 include 路径中
- C++11 或更高版本

---

## 快速上手

**1. 声明结构体**

```cpp
#include "JsonTransformAuto.h"

struct Person {
    std::string name;
    int32_t     age;
    DECLARE__JSON__AUTO__TRANSFORM   // 注入序列化/反序列化方法
};

// 在文件作用域（不能在函数内部）
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age)
```

**2. 从 JSON 字符串反序列化**

```cpp
Person p;
bool ok = p.transform_from_json("{\"name\":\"Alice\",\"age\":30}");
// ok == true, p.name == "Alice", p.age == 30
```

**3. 序列化为 JSON 字符串**

```cpp
Person p;
p.name = "Bob";
p.age  = 25;
std::string json = p.transform_to_json();
// json == {"name":"Bob","age":25}
```

---

## 支持的字段类型

| 类型 | 说明 |
|---|---|
| `int8_t`、`uint8_t` | 序列化为 JSON 整数 |
| `int16_t`、`uint16_t` | 序列化为 JSON 整数 |
| `int32_t`、`uint32_t` | 序列化为 JSON 整数 |
| `int64_t`、`uint64_t` | 序列化为 JSON 整数 |
| `float` | 序列化为 JSON double |
| `double` | 序列化为 JSON double |
| `bool` | 序列化为 JSON `true`/`false` |
| `std::string` | 序列化为 JSON 字符串 |
| 嵌套结构体 | 也必须声明两行宏 |
| `std::vector<T>` | 序列化为 JSON 数组；`T` 可以是任意支持的类型，包括结构体或 `std::vector<T>` |

> ⚠️ **不支持 `std::vector<bool>`。** C++ 标准对 `std::vector<bool>` 做了特化，内部使用 bit 压缩存储，元素没有独立的内存地址，无法与库内部的 `reinterpret_cast` 配合使用。请改用 `std::vector<uint8_t>`，用 `0` 和 `1` 分别表示 `false` 和 `true`。
>
> ```cpp
> // ❌ 不要使用
> std::vector<bool> flags;
>
> // ✅ 请使用这个替代
> std::vector<uint8_t> flags;  // 0 = false, 1 = true
> ```

> **限制：** 单个结构体最多支持 **20 个字段**。

---

## API 参考

### `DECLARE__JSON__AUTO__TRANSFORM` 注入的方法

```cpp
// 从 JSON 字符串反序列化。所有字段都找到时返回 true。
bool transform_from_json(const std::string& json);

// 从已有的 rapidjson::Value 反序列化（必须是 Object 类型）。
bool transform_from_json(const rapidjson::Value& value);

// 序列化为 JSON 字符串。
std::string transform_to_json();

// 序列化到已有的 rapidjson::Value 和 allocator 中。
// 适用于将当前对象嵌入更大的 Document。
void transform_to_json(rapidjson::Value& value,
                       rapidjson::Document::AllocatorType& allocator);
```

### 根节点为数组时的全局函数

位于 `auto_json` 命名空间，处理根节点为数组的 JSON。

```cpp
// 将 JSON 数组字符串反序列化到 vector 中。
// T 可以是 reflect 类型、基础类型、std::string 或 std::vector<T>。
// JSON 无效或根节点不是数组时返回 false。
template<typename T>
bool auto_json::transform_from_json(std::vector<T>& dest,
                                    const std::string& json);

// 将 vector 序列化为 JSON 数组字符串。
template<typename T>
std::string auto_json::transform_to_json(const std::vector<T>& src);
```

---

## 示例

### 嵌套结构体

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

// 使用
Person p;
p.transform_from_json(
    "{\"name\":\"Alice\",\"age\":30,"
    "\"addr\":{\"city\":\"Beijing\",\"zip\":100000}}");
```

### 结构体包含 vector

```cpp
struct Department {
    std::string           name;
    std::vector<Employee> employees;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Department, name, employees)
```

### 嵌套 vector

```cpp
struct Matrix {
    std::string                       label;
    std::vector<std::vector<int32_t>> rows;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, label, rows)
```

### 根节点为数组

```cpp
std::vector<Person> people;

// 反序列化
bool ok = auto_json::transform_from_json(people, "[{...},{...}]");

// 序列化
std::string json = auto_json::transform_to_json(people);
```

### 嵌入更大的 Document

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

## 错误处理与边界情况

| 场景 | 行为 |
|---|---|
| JSON 字符串非法 | 返回 `false`，结构体不变 |
| 根节点是数组（单对象接口） | 返回 `false`，结构体不变 |
| 根节点是对象（数组接口） | 返回 `false`，vector 不变 |
| JSON 中字段缺失 | 字段保留原值，函数返回 `false` |
| JSON 中有多余/未知字段 | 静默忽略 |
| 字段值为 `null` | 基础类型字段赋零值/默认值；嵌套对象字段保留原值 |
| 字段期望 Object，JSON 给了 array/string | 嵌套字段保留原值 |
| 字段期望 array，JSON 给了 number/object | vector 字段不变 |
| 数组元素类型不匹配 | 元素保留零值/默认值，不崩溃 |
| 字符串 `"42"` → 整数字段 | 使用 `std::stoll`/`std::stod` 解析；非法字符串返回 `0` |
| 数字 → `std::string` 字段 | 使用 `std::to_string` 转换 |
| JSON `true`/`false` → `std::string` 字段 | 返回空字符串（不是 `"true"`/`"false"`） |
| vector 字段的 JSON 值为空数组 `[]` | vector 被 resize 为 0，函数返回 `true` |

---

## 注意事项

**结构体必须在文件作用域或命名空间作用域声明。**

`DECLARE__JSON__AUTO__TRANSFORM` 会注入一个 `static` 成员，C++ 不允许局部结构体（函数内部定义的结构体）拥有静态成员。

```cpp
// ✅ 正确 —— 文件作用域
struct Foo {
    int x;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Foo, x)

// ❌ 错误 —— 局部作用域
void bar() {
    struct Foo { int x; DECLARE__JSON__AUTO__TRANSFORM };  // 编译错误
}
```

**每个结构体只能出现一次 `IMPLEMENT__JSON__AUTO__TRANSFORM`**，通常放在 `.cpp` 文件中（纯头文件项目也可以放在头文件里）。

**单个结构体最多支持 20 个字段**，超出时请拆分为嵌套结构体。

---

## 类型转换规则

当 JSON 类型与字段类型不完全一致时，库会尝试合理的自动转换。

| JSON 值 | 目标 C++ 类型 | 结果 |
|---|---|---|
| `"42"` | 任意整数 | `42`（通过 `std::stoll`） |
| `"3.14"` | `float`/`double` | `3.14`（通过 `std::stod`） |
| `"true"` 或 `"1"` | `bool` | `true` |
| `"false"` 或 `"0"` | `bool` | `false` |
| `1` / `0` | `bool` | `true` / `false` |
| `42` | `std::string` | `"42"`（通过 `std::to_string`） |
| `3.14` | `std::string` | `"3.14..."`（通过 `std::to_string`） |
| `true` / `false` | `std::string` | `""`（空字符串，不支持转换） |
| `"not_a_number"` | 任意数字 | `0`（捕获异常后返回默认值） |
