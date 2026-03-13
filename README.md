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

## 
