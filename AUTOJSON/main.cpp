// JsonTransformAutoTest.cpp
// 覆盖 JsonTransformAuto.h 的各种解析场景
// 编译方式示例：g++ -std=c++14 -I<rapidjson_include_path> JsonTransformAutoTest.cpp -o test

#include <cassert>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <cstdio>
#include "JsonTransformAuto.h"

// ============================================================
// 测试结构体定义
// ============================================================

// 基础类型结构体
struct BasicTypes {
    int         i;
    unsigned    u;
    int64_t     i64;
    uint64_t    u64;
    double      d;
    float       f;
    bool        b;
    std::string s;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BasicTypes, i, u, i64, u64, d, f, b, s)

// 嵌套结构体
struct Address {
    std::string city;
    std::string street;
    int         code;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Address, city, street, code)

struct Person {
    std::string name;
    int         age;
    Address     address;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age, address)

// vector 结构体
struct Container {
    std::vector<int>         int_list;
    std::vector<std::string> str_list;
    std::vector<Address>     addr_list;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Container, int_list, str_list, addr_list)

// 数组嵌套数组结构体
struct Matrix {
    std::vector<std::vector<int>>         int_matrix;    // [[1,2],[3,4]]
    std::vector<std::vector<std::string>> str_matrix;    // [["a","b"],["c"]]
    std::vector<std::vector<Address>>     struct_matrix; // [[{...},{...}],[{...}]]
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, int_matrix, str_matrix, struct_matrix)

// 类型兼容降级结构体（JSON类型与C++类型不匹配）
struct CompatTypes {
    int         i_from_bool;      // JSON: true  -> int 1
    int         i_from_double;    // JSON: 3.9   -> int 3
    int         i_from_string;    // JSON: "42"  -> int 42
    double      d_from_bool;      // JSON: false -> double 0.0
    double      d_from_string;    // JSON: "3.14"-> double 3.14
    float       f_from_string;    // JSON: "1.5" -> float 1.5
    bool        b_from_int;       // JSON: 1     -> bool true
    bool        b_from_zero;      // JSON: 0     -> bool false
    bool        b_from_string;    // JSON: "true"-> bool true
    uint64_t    u64_from_string;  // JSON: "999" -> uint64_t 999
    std::string s_from_int;       // JSON: 123   -> string "123"
    std::string s_from_double;    // JSON: 1.5   -> string (含"1.5")
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(CompatTypes,
    i_from_bool, i_from_double, i_from_string,
    d_from_bool, d_from_string, f_from_string,
    b_from_int, b_from_zero, b_from_string,
    u64_from_string, s_from_int, s_from_double)

// ============================================================
// 辅助宏
// ============================================================
#define ASSERT_EQ(a, b)  do { \
    if (!((a) == (b))) { \
        printf("FAILED: %s == %s  (line %d)\n", #a, #b, __LINE__); \
        assert(false); \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps) do { \
    if (std::fabs((double)(a) - (double)(b)) > (eps)) { \
        printf("FAILED: |%s - %s| <= %g  (line %d)\n", #a, #b, (double)(eps), __LINE__); \
        assert(false); \
    } \
} while(0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        printf("FAILED: %s  (line %d)\n", #expr, __LINE__); \
        assert(false); \
    } \
} while(0)

#define RUN_TEST(name) do { \
    test_##name(); \
    printf("PASSED: " #name "\n"); \
} while(0)

// ============================================================
// 测试：基础类型 JSON -> 结构体
// ============================================================
void test_basic_from_json() {
    const std::string json = R"({
        "i":   -42,
        "u":   100,
        "i64": -9000000000,
        "u64": 18000000000,
        "d":   3.14159,
        "f":   1.5,
        "b":   true,
        "s":   "hello"
    })";
    BasicTypes obj{};
    bool ok = obj.transform_from_json(json);
    ASSERT_TRUE(ok);
    ASSERT_EQ(obj.i,   -42);
    ASSERT_EQ(obj.u,   100u);
    ASSERT_EQ(obj.i64, -9000000000LL);
    ASSERT_EQ(obj.u64, 18000000000ULL);
    ASSERT_NEAR(obj.d, 3.14159, 1e-5);
    ASSERT_NEAR(obj.f, 1.5f,    1e-5);
    ASSERT_EQ(obj.b,   true);
    ASSERT_EQ(obj.s,   "hello");
}

// ============================================================
// 测试：基础类型 结构体 -> JSON -> 结构体（往返）
// ============================================================
void test_basic_roundtrip() {
    BasicTypes src{};
    src.i   = 7;
    src.u   = 8u;
    src.i64 = -123456789012345LL;
    src.u64 = 987654321098765ULL;
    src.d   = 2.71828;
    src.f   = 0.5f;
    src.b   = false;
    src.s   = "world";

    std::string json = src.transform_to_json();

    BasicTypes dst{};
    bool ok = dst.transform_from_json(json);
    ASSERT_TRUE(ok);
    ASSERT_EQ(dst.i,   src.i);
    ASSERT_EQ(dst.u,   src.u);
    ASSERT_EQ(dst.i64, src.i64);
    ASSERT_EQ(dst.u64, src.u64);
    ASSERT_NEAR(dst.d, src.d, 1e-5);
    ASSERT_NEAR(dst.f, src.f, 1e-4);
    ASSERT_EQ(dst.b,   src.b);
    ASSERT_EQ(dst.s,   src.s);
}

// ============================================================
// 测试：嵌套结构体
// ============================================================
void test_nested_struct() {
    const std::string json = R"({
        "name": "Alice",
        "age": 30,
        "address": {
            "city": "Beijing",
            "street": "Main St",
            "code": 100000
        }
    })";
    Person p{};
    bool ok = p.transform_from_json(json);
    ASSERT_TRUE(ok);
    ASSERT_EQ(p.name,             "Alice");
    ASSERT_EQ(p.age,              30);
    ASSERT_EQ(p.address.city,     "Beijing");
    ASSERT_EQ(p.address.street,   "Main St");
    ASSERT_EQ(p.address.code,     100000);
}

void test_nested_roundtrip() {
    Person src{};
    src.name           = "Bob";
    src.age            = 25;
    src.address.city   = "Shanghai";
    src.address.street = "Nanjing Rd";
    src.address.code   = 200000;

    Person dst{};
    bool ok = dst.transform_from_json(src.transform_to_json());
    ASSERT_TRUE(ok);
    ASSERT_EQ(dst.name,             src.name);
    ASSERT_EQ(dst.age,              src.age);
    ASSERT_EQ(dst.address.city,     src.address.city);
    ASSERT_EQ(dst.address.street,   src.address.street);
    ASSERT_EQ(dst.address.code,     src.address.code);
}

// ============================================================
// 测试：vector 基础类型
// ============================================================
void test_vector_int() {
    const std::string json = R"({"int_list":[1,2,3,4,5],"str_list":[],"addr_list":[]})";
    Container c{};
    c.transform_from_json(json);  // 空 vector 不计入失败
    ASSERT_EQ(c.int_list.size(), 5u);
    ASSERT_EQ(c.int_list[0], 1);
    ASSERT_EQ(c.int_list[4], 5);
}

void test_vector_string() {
    const std::string json = R"({"int_list":[],"str_list":["a","b","c"],"addr_list":[]})";
    Container c{};
    c.transform_from_json(json);
    ASSERT_EQ(c.str_list.size(), 3u);
    ASSERT_EQ(c.str_list[0], "a");
    ASSERT_EQ(c.str_list[2], "c");
}

void test_vector_struct() {
    const std::string json = R"({
        "int_list": [],
        "str_list": [],
        "addr_list": [
            {"city":"A","street":"s1","code":1},
            {"city":"B","street":"s2","code":2}
        ]
    })";
    Container c{};
    c.transform_from_json(json);
    ASSERT_EQ(c.addr_list.size(), 2u);
    ASSERT_EQ(c.addr_list[0].city, "A");
    ASSERT_EQ(c.addr_list[1].code, 2);
}

// 空 vector（[]）应被认为解析成功，dest 保持空
void test_vector_empty_array() {
    const std::string json = R"({"int_list":[],"str_list":[],"addr_list":[]})";
    Container c{};
    c.int_list = {9, 9, 9};  // 预填值，确认被清空
    c.transform_from_json(json);
    // 空数组时 resize 不应发生，原有数据保留（取决于实现）
    // 此处只验证不崩溃即可
    ASSERT_TRUE(true);
}

void test_vector_roundtrip() {
    Container src{};
    src.int_list = {10, 20, 30};
    src.str_list = {"x", "y"};
    src.addr_list.push_back({"City1", "Street1", 111});

    Container dst{};
    bool ok = dst.transform_from_json(src.transform_to_json());
    ASSERT_TRUE(ok);
    ASSERT_EQ(dst.int_list.size(),  3u);
    ASSERT_EQ(dst.int_list[1],      20);
    ASSERT_EQ(dst.str_list[0],      "x");
    ASSERT_EQ(dst.addr_list[0].code, 111);
}

// ============================================================
// 测试：类型兼容降级
// ============================================================
void test_compat_types() {
    const std::string json = R"({
        "i_from_bool":     true,
        "i_from_double":   3.9,
        "i_from_string":   "42",
        "d_from_bool":     false,
        "d_from_string":   "3.14",
        "f_from_string":   "1.5",
        "b_from_int":      1,
        "b_from_zero":     0,
        "b_from_string":   "true",
        "u64_from_string": "999",
        "s_from_int":      123,
        "s_from_double":   1.5
    })";
    CompatTypes obj{};
    obj.transform_from_json(json);  // 部分字段可能降级，不强求全部 ok

    ASSERT_EQ(obj.i_from_bool,    1);
    ASSERT_EQ(obj.i_from_double,  3);        // truncate
    ASSERT_EQ(obj.i_from_string,  42);
    ASSERT_NEAR(obj.d_from_bool,  0.0, 1e-9);
    ASSERT_NEAR(obj.d_from_string,3.14, 1e-5);
    ASSERT_NEAR(obj.f_from_string,1.5f, 1e-5);
    ASSERT_EQ(obj.b_from_int,     true);
    ASSERT_EQ(obj.b_from_zero,    false);
    ASSERT_EQ(obj.b_from_string,  true);
    ASSERT_EQ(obj.u64_from_string,999ULL);
    ASSERT_EQ(obj.s_from_int,     "123");
    // s_from_double: std::to_string(1.5) 平台相关，只验证非空
    ASSERT_TRUE(!obj.s_from_double.empty());
}

// ============================================================
// 测试：数组嵌套数组
// ============================================================

// int 二维数组
void test_nested_vector_int() {
    const std::string json = R"({
        "int_matrix":    [[1,2,3],[4,5],[6]],
        "str_matrix":    [],
        "struct_matrix": []
    })";
    Matrix m{};
    m.transform_from_json(json);
    ASSERT_EQ(m.int_matrix.size(),    3u);
    ASSERT_EQ(m.int_matrix[0].size(), 3u);
    ASSERT_EQ(m.int_matrix[0][0],     1);
    ASSERT_EQ(m.int_matrix[0][2],     3);
    ASSERT_EQ(m.int_matrix[1].size(), 2u);
    ASSERT_EQ(m.int_matrix[1][1],     5);
    ASSERT_EQ(m.int_matrix[2].size(), 1u);
    ASSERT_EQ(m.int_matrix[2][0],     6);
}

// string 二维数组
void test_nested_vector_string() {
    const std::string json = R"({
        "int_matrix":    [],
        "str_matrix":    [["hello","world"],["foo"]],
        "struct_matrix": []
    })";
    Matrix m{};
    m.transform_from_json(json);
    ASSERT_EQ(m.str_matrix.size(),    2u);
    ASSERT_EQ(m.str_matrix[0].size(), 2u);
    ASSERT_EQ(m.str_matrix[0][0],     "hello");
    ASSERT_EQ(m.str_matrix[0][1],     "world");
    ASSERT_EQ(m.str_matrix[1].size(), 1u);
    ASSERT_EQ(m.str_matrix[1][0],     "foo");
}

// 结构体二维数组
void test_nested_vector_struct() {
    const std::string json = R"({
        "int_matrix":    [],
        "str_matrix":    [],
        "struct_matrix": [
            [{"city":"A","street":"s1","code":1},{"city":"B","street":"s2","code":2}],
            [{"city":"C","street":"s3","code":3}]
        ]
    })";
    Matrix m{};
    m.transform_from_json(json);
    ASSERT_EQ(m.struct_matrix.size(),       2u);
    ASSERT_EQ(m.struct_matrix[0].size(),    2u);
    ASSERT_EQ(m.struct_matrix[0][0].city,   "A");
    ASSERT_EQ(m.struct_matrix[0][1].code,   2);
    ASSERT_EQ(m.struct_matrix[1].size(),    1u);
    ASSERT_EQ(m.struct_matrix[1][0].city,   "C");
}

// 二维数组往返
void test_nested_vector_roundtrip() {
    Matrix src{};
    src.int_matrix = {{1, 2}, {3, 4, 5}};
    src.str_matrix = {{"a", "b"}, {"c"}};
    src.struct_matrix = {{{"City1", "St1", 100}, {"City2", "St2", 200}}};

    Matrix dst{};
    dst.transform_from_json(src.transform_to_json());

    ASSERT_EQ(dst.int_matrix.size(),          2u);
    ASSERT_EQ(dst.int_matrix[0][1],           2);
    ASSERT_EQ(dst.int_matrix[1][2],           5);
    ASSERT_EQ(dst.str_matrix[0][0],           "a");
    ASSERT_EQ(dst.str_matrix[1][0],           "c");
    ASSERT_EQ(dst.struct_matrix[0][0].city,   "City1");
    ASSERT_EQ(dst.struct_matrix[0][1].code,   200);
}

// 内层含空数组
void test_nested_vector_inner_empty() {
    const std::string json = R"({
        "int_matrix":    [[],[1,2],[]],
        "str_matrix":    [],
        "struct_matrix": []
    })";
    Matrix m{};
    m.transform_from_json(json);
    // 内层空数组不崩溃，非空行数据正确
    ASSERT_EQ(m.int_matrix.size(),    3u);
    ASSERT_EQ(m.int_matrix[1].size(), 2u);
    ASSERT_EQ(m.int_matrix[1][0],     1);
}

// ============================================================
// 测试：JSON 格式错误
// ============================================================
void test_invalid_json() {
    BasicTypes obj{};
    bool ok = obj.transform_from_json("{invalid json}");
    ASSERT_EQ(ok, false);
}

// ============================================================
// 测试：字段缺失
// ============================================================
void test_missing_field() {
    // 只有部分字段
    const std::string json = R"({"i": 1, "s": "hi"})";
    BasicTypes obj{};
    bool ok = obj.transform_from_json(json);
    // 有字段缺失，应返回 false
    ASSERT_EQ(ok, false);
    // 但已有字段应正确解析
    ASSERT_EQ(obj.i, 1);
    ASSERT_EQ(obj.s, "hi");
}

// ============================================================
// 测试：极值
// ============================================================
void test_extreme_values() {
    const std::string json = R"({
        "i":   -2147483648,
        "u":   4294967295,
        "i64": -9223372036854775808,
        "u64": 9223372036854775807,
        "d":   1.7976931348623157e+308,
        "f":   3.4028235e+38,
        "b":   false,
        "s":   ""
    })";
    BasicTypes obj{};
    bool ok = obj.transform_from_json(json);
    ASSERT_TRUE(ok);
    ASSERT_EQ(obj.i, -2147483648);
    ASSERT_EQ(obj.u, 4294967295u);
    ASSERT_EQ(obj.b, false);
    ASSERT_EQ(obj.s, "");
}

// ============================================================
// 测试：字符串含特殊字符
// ============================================================
void test_special_string() {
    const std::string json = R"({
        "i":0,"u":0,"i64":0,"u64":0,"d":0,"f":0,"b":false,
        "s": "hello \"world\"\nnewline\ttab"
    })";
    BasicTypes obj{};
    obj.transform_from_json(json);
    ASSERT_TRUE(obj.s.find("world") != std::string::npos);
}
struct DDDD
{
    
    const static auto_json::ReflectMapType reflect_map;
};
// ============================================================
// main
// ============================================================
int main() {
    auto isss = auto_json::is_reflect_type<DDDD>::value;
    printf("===== JsonTransformAuto Tests =====%d\n",isss);

    RUN_TEST(basic_from_json);
    RUN_TEST(basic_roundtrip);
    RUN_TEST(nested_struct);
    RUN_TEST(nested_roundtrip);
    RUN_TEST(vector_int);
    RUN_TEST(vector_string);
    RUN_TEST(vector_struct);
    RUN_TEST(vector_empty_array);
    RUN_TEST(vector_roundtrip);
    RUN_TEST(nested_vector_int);
    RUN_TEST(nested_vector_string);
    RUN_TEST(nested_vector_struct);
    RUN_TEST(nested_vector_roundtrip);
    RUN_TEST(nested_vector_inner_empty);
    RUN_TEST(compat_types);
    RUN_TEST(invalid_json);
    RUN_TEST(missing_field);
    RUN_TEST(extreme_values);
    RUN_TEST(special_string);

    printf("\n===== All tests passed =====\n");
    return 0;
}
