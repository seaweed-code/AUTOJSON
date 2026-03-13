# C++ 自动JSON 转换
 基于`rapidjson`

为类注入运行时反射机制，使得可以自动像高级语言一样，自动JSON转成模型，模型转JSON，而不需逐个字段手动转。

## 特性

###### 支持：

- `double`    `float`   ` bool` `int8_t` `uint8_t` `int16_t` `uint16_t`  `int32_t` `uint32_t`  `int64_t`   `uint64_t`   `std::string` 
- enum 自定义枚举
- 支持对象嵌套
- `std::vector< T >` ，T 可以是上面的基础类型，或者自定义对象。
- `std::vector< std::vector<T> >` 数组嵌套数组

###### 不支持：

- `std::vector<bool>` 因为C++模版库对`std::vector<bool> `进行了特化，请使用:  `std::vector<int8_t>` 之类的代替

## 依赖

- RapidJSON 头文件在 include 路径中
- C++11 或更高版本

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

### 

## 错误处理与边界情况

| 场景                                     | 行为                                                   |
| ---------------------------------------- | ------------------------------------------------------ |
| JSON 字符串非法                          | 返回 `false`，结构体不变                               |
| 根节点是数组（单对象接口）               | 返回 `false`，结构体不变                               |
| 根节点是对象（数组接口）                 | 返回 `false`，vector 不变                              |
| JSON 中字段缺失                          | 字段保留原值，函数返回 `false`                         |
| JSON 中有多余/未知字段                   | 静默忽略                                               |
| 字段值为 `null`                          | 基础类型字段赋零值/默认值；嵌套对象字段保留原值        |
| 字段期望 Object，JSON 给了 array/string  | 嵌套字段保留原值                                       |
| 字段期望 array，JSON 给了 number/object  | vector 字段不变                                        |
| 数组元素类型不匹配                       | 元素保留零值/默认值，不崩溃                            |
| 字符串 `"42"` → 整数字段                 | 使用 `std::stoll`/`std::stod` 解析；非法字符串返回 `0` |
| 数字 → `std::string` 字段                | 使用 `std::to_string` 转换                             |
| JSON `true`/`false` → `std::string` 字段 | 返回空字符串（不是 `"true"`/`"false"`）                |
| vector 字段的 JSON 值为空数组 `[]`       | vector 被 resize 为 0，函数返回 `true`                 |

**单个结构体最多支持 20 个字段**，超出时请拆分为嵌套结构体。



## 类型转换规则

当 JSON 类型与字段类型不完全一致时，库会尝试合理的自动转换。

| JSON 值            | 目标 C++ 类型    | 结果                                 |
| ------------------ | ---------------- | ------------------------------------ |
| `"42"`             | 任意整数         | `42`（通过 `std::stoll`）            |
| `"3.14"`           | `float`/`double` | `3.14`（通过 `std::stod`）           |
| `"true"` 或 `"1"`  | `bool`           | `true`                               |
| `"false"` 或 `"0"` | `bool`           | `false`                              |
| `1` / `0`          | `bool`           | `true` / `false`                     |
| `42`               | `std::string`    | `"42"`（通过 `std::to_string`）      |
| `3.14`             | `std::string`    | `"3.14..."`（通过 `std::to_string`） |
| `true` / `false`   | `std::string`    | `""`（空字符串，不支持转换）         |
| `"not_a_number"`   | 任意数字         | `0`（捕获异常后返回默认值）          |

