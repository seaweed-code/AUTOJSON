/**
 * JsonTransformAutoTest.cpp
 * 全面覆盖测试：所有基础类型、嵌套对象、数组、数组嵌套数组、容错等
 * 兼容 VS2013 / GCC 4.8.5
 *
 * 注意：所有使用 DECLARE__JSON__AUTO__TRANSFORM 的结构体
 *       必须定义在文件作用域，不能在函数内部。
 */

#include "JsonTransformAuto.h"
#include <cstdio>
#include <cmath>
#include <cstring>

// ============================================================
// 辅助宏
// ============================================================
#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            printf("[FAIL] %s  (line %d)\n", #expr, __LINE__); \
            s_failCount++; \
        } else { \
            printf("[PASS] %s\n", #expr); \
            s_passCount++; \
        } \
    } while(0)

static int s_passCount = 0;
static int s_failCount = 0;

static bool floatEq(double a, double b, double eps = 1e-5)
{
    return fabs(a - b) < eps;
}

// ============================================================
// 所有测试用结构体统一定义在文件作用域
// ============================================================

// --- 1. 全基础类型 ---
struct PrimitiveModel
{
    int8_t      i8;
    uint8_t     u8;
    int16_t     i16;
    uint16_t    u16;
    int32_t     i32;
    uint32_t    u32;
    int64_t     i64;
    uint64_t    u64;
    float       f32;
    double      f64;
    bool        b;
    std::string str;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(PrimitiveModel,
    i8, u8, i16, u16, i32, u32, i64, u64, f32, f64, b, str)

// --- 2. 嵌套对象 ---
struct Address
{
    std::string city;
    int32_t     zip;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Address, city, zip)

struct Person
{
    std::string name;
    int32_t     age;
    Address     addr;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age, addr)

// --- 3. 三层嵌套 + 对象数组 ---
struct Employee
{
    std::string name;
    int32_t     level;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Employee, name, level)

struct Department
{
    std::string           name;
    std::vector<Employee> employees;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Department, name, employees)

struct Company
{
    std::string             name;
    std::vector<Department> departments;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Company, name, departments)

// --- 4. 数组嵌套数组 vector<vector<int32_t>> ---
struct Matrix
{
    std::string                       name;
    std::vector<std::vector<int32_t>> rows;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, name, rows)

// --- 5. 类型容错 ---
struct ToleranceModel
{
    int32_t     intFromStr;
    double      dblFromStr;
    bool        boolFromStr;
    std::string strFromInt;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(ToleranceModel, intFromStr, dblFromStr, boolFromStr, strFromInt)

// --- 6. 单字段 ---
struct SingleField
{
    int32_t value;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(SingleField, value)

// --- 7. bool 专项测试 ---
struct BoolModel
{
    bool a;
    bool b;
    bool c;
    bool d;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BoolModel, a, b, c, d)

// --- 8. int vector ---
struct IntListModel
{
    std::string          tag;
    std::vector<int32_t> nums;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(IntListModel, tag, nums)

// --- 9. string vector ---
struct StrListModel
{
    std::string              tag;
    std::vector<std::string> strs;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(StrListModel, tag, strs)

// --- 10. 边界值 ---
struct BoundaryModel
{
    int64_t  minI64;
    int64_t  maxI64;
    uint64_t maxU64;
    double   pi;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BoundaryModel, minI64, maxI64, maxU64, pi)


// ============================================================
// 测试函数
// ============================================================

// 测试1：全基础类型 JSON → 模型
void test_primitive_from_json()
{
    printf("\n=== test_primitive_from_json ===\n");
    const char* json =
        "{\"i8\":-100,\"u8\":200,\"i16\":-30000,\"u16\":60000,"
        "\"i32\":-2000000,\"u32\":4000000,\"i64\":-9000000000,"
        "\"u64\":18000000000,\"f32\":1.5,\"f64\":3.14159265,"
        "\"b\":true,\"str\":\"hello\"}";

    PrimitiveModel m;
    bool ok = m.transform_from_json(std::string(json));
    CHECK(ok);
    CHECK(m.i8  == -100);
    CHECK(m.u8  == 200);
    CHECK(m.i16 == -30000);
    CHECK(m.u16 == 60000);
    CHECK(m.i32 == -2000000);
    CHECK(m.u32 == 4000000U);
    CHECK(m.i64 == (int64_t)(-9000000000LL));
    CHECK(m.u64 == (uint64_t)(18000000000ULL));
    CHECK(floatEq(m.f32, 1.5f));
    CHECK(floatEq(m.f64, 3.14159265));
    CHECK(m.b   == true);
    CHECK(m.str == "hello");
}

// 测试2：全基础类型 往返
void test_primitive_roundtrip()
{
    printf("\n=== test_primitive_roundtrip ===\n");
    PrimitiveModel src;
    src.i8  = -1;
    src.u8  = 255;
    src.i16 = -32768;
    src.u16 = 65535;
    src.i32 = -1000000;
    src.u32 = 3000000U;
    src.i64 = -1234567890123LL;
    src.u64 = 9876543210ULL;
    src.f32 = 2.718f;
    src.f64 = 1.41421356;
    src.b   = false;
    src.str = "world";

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    PrimitiveModel dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.i8  == src.i8);
    CHECK(dst.u8  == src.u8);
    CHECK(dst.i16 == src.i16);
    CHECK(dst.u16 == src.u16);
    CHECK(dst.i32 == src.i32);
    CHECK(dst.u32 == src.u32);
    CHECK(dst.i64 == src.i64);
    CHECK(dst.u64 == src.u64);
    CHECK(floatEq(dst.f32, src.f32, 1e-3));
    CHECK(floatEq(dst.f64, src.f64));
    CHECK(dst.b   == src.b);
    CHECK(dst.str == src.str);
}

// 测试3：嵌套对象 JSON → 模型
void test_nested_from_json()
{
    printf("\n=== test_nested_from_json ===\n");
    const char* json =
        "{\"name\":\"Alice\",\"age\":30,"
        "\"addr\":{\"city\":\"Beijing\",\"zip\":100000}}";

    Person p;
    bool ok = p.transform_from_json(std::string(json));
    CHECK(ok);
    CHECK(p.name      == "Alice");
    CHECK(p.age       == 30);
    CHECK(p.addr.city == "Beijing");
    CHECK(p.addr.zip  == 100000);
}

// 测试4：嵌套对象 往返
void test_nested_roundtrip()
{
    printf("\n=== test_nested_roundtrip ===\n");
    Person src;
    src.name      = "Bob";
    src.age       = 25;
    src.addr.city = "Shanghai";
    src.addr.zip  = 200000;

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    Person dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.name      == src.name);
    CHECK(dst.age       == src.age);
    CHECK(dst.addr.city == src.addr.city);
    CHECK(dst.addr.zip  == src.addr.zip);
}

// 测试5：三层嵌套 + 对象数组 JSON → 模型
void test_deep_nested_array_from_json()
{
    printf("\n=== test_deep_nested_array_from_json ===\n");
    const char* json =
        "{\"name\":\"TechCorp\",\"departments\":["
        "{\"name\":\"Engineering\",\"employees\":["
        "{\"name\":\"Tom\",\"level\":3},"
        "{\"name\":\"Jerry\",\"level\":2}]},"
        "{\"name\":\"Marketing\",\"employees\":["
        "{\"name\":\"Kate\",\"level\":4}]}]}";

    Company c;
    bool ok = c.transform_from_json(std::string(json));
    CHECK(ok);
    CHECK(c.name == "TechCorp");
    CHECK(c.departments.size() == 2);
    CHECK(c.departments[0].name == "Engineering");
    CHECK(c.departments[0].employees.size() == 2);
    CHECK(c.departments[0].employees[0].name == "Tom");
    CHECK(c.departments[0].employees[0].level == 3);
    CHECK(c.departments[0].employees[1].name == "Jerry");
    CHECK(c.departments[0].employees[1].level == 2);
    CHECK(c.departments[1].name == "Marketing");
    CHECK(c.departments[1].employees.size() == 1);
    CHECK(c.departments[1].employees[0].name == "Kate");
    CHECK(c.departments[1].employees[0].level == 4);
}

// 测试6：三层嵌套 往返
void test_deep_nested_array_roundtrip()
{
    printf("\n=== test_deep_nested_array_roundtrip ===\n");
    Company src;
    src.name = "TestCo";

    Department d1;
    d1.name = "R&D";
    Employee e1; e1.name = "Alice"; e1.level = 5;
    Employee e2; e2.name = "Bob";   e2.level = 3;
    d1.employees.push_back(e1);
    d1.employees.push_back(e2);
    src.departments.push_back(d1);

    Department d2;
    d2.name = "HR";
    Employee e3; e3.name = "Carol"; e3.level = 2;
    d2.employees.push_back(e3);
    src.departments.push_back(d2);

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    Company dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.name == src.name);
    CHECK(dst.departments.size() == src.departments.size());
    CHECK(dst.departments[0].name == "R&D");
    CHECK(dst.departments[0].employees.size() == 2);
    CHECK(dst.departments[0].employees[0].name == "Alice");
    CHECK(dst.departments[0].employees[0].level == 5);
    CHECK(dst.departments[1].name == "HR");
    CHECK(dst.departments[1].employees[0].name == "Carol");
}

// 测试7：数组嵌套数组 JSON → 模型
void test_vector_of_vector_from_json()
{
    printf("\n=== test_vector_of_vector_from_json ===\n");
    const char* json =
        "{\"name\":\"mat\",\"rows\":[[1,2,3],[4,5,6],[7,8,9]]}";

    Matrix m;
    bool ok = m.transform_from_json(std::string(json));
    CHECK(ok);
    CHECK(m.name == "mat");
    CHECK(m.rows.size() == 3);
    CHECK(m.rows[0].size() == 3);
    CHECK(m.rows[0][0] == 1);
    CHECK(m.rows[0][2] == 3);
    CHECK(m.rows[1][1] == 5);
    CHECK(m.rows[2][2] == 9);
}

// 测试8：数组嵌套数组 往返
void test_vector_of_vector_roundtrip()
{
    printf("\n=== test_vector_of_vector_roundtrip ===\n");
    Matrix src;
    src.name = "identity";
    std::vector<int32_t> r0; r0.push_back(1); r0.push_back(0); r0.push_back(0);
    std::vector<int32_t> r1; r1.push_back(0); r1.push_back(1); r1.push_back(0);
    std::vector<int32_t> r2; r2.push_back(0); r2.push_back(0); r2.push_back(1);
    src.rows.push_back(r0);
    src.rows.push_back(r1);
    src.rows.push_back(r2);

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    Matrix dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.name == "identity");
    CHECK(dst.rows.size() == 3);
    CHECK(dst.rows[0][0] == 1 && dst.rows[0][1] == 0);
    CHECK(dst.rows[1][1] == 1 && dst.rows[1][0] == 0);
    CHECK(dst.rows[2][2] == 1 && dst.rows[2][0] == 0);
}

// 测试9：类型容错（字符串↔数值/bool）
void test_type_tolerance()
{
    printf("\n=== test_type_tolerance ===\n");
    const char* json =
        "{\"intFromStr\":\"42\",\"dblFromStr\":\"3.14\","
        "\"boolFromStr\":\"true\",\"strFromInt\":100}";

    ToleranceModel m;
    m.transform_from_json(std::string(json));
    CHECK(m.intFromStr  == 42);
    CHECK(floatEq(m.dblFromStr, 3.14));
    CHECK(m.boolFromStr == true);
    CHECK(m.strFromInt  == "100");
}

// 测试10：缺失字段保持原值
void test_missing_fields()
{
    printf("\n=== test_missing_fields ===\n");
    Person p;
    p.age       = 99;
    p.addr.city = "X";
    p.addr.zip  = 1;

    p.transform_from_json(std::string("{\"name\":\"Ghost\"}"));
    CHECK(p.name      == "Ghost");
    CHECK(p.age       == 99);
    CHECK(p.addr.city == "X");
}

// 测试11：空数组
void test_empty_array()
{
    printf("\n=== test_empty_array ===\n");
    Company c;
    c.transform_from_json(std::string("{\"name\":\"empty\",\"departments\":[]}"));
    CHECK(c.name == "empty");
    CHECK(c.departments.size() == 0);
}

// 测试12：单字段结构体
void test_single_field()
{
    printf("\n=== test_single_field ===\n");
    SingleField s;
    s.value = 0;
    s.transform_from_json(std::string("{\"value\":777}"));
    CHECK(s.value == 777);

    s.value = 999;
    std::string json = s.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    SingleField s2;
    s2.transform_from_json(json);
    CHECK(s2.value == 999);
}

// 测试13：非法 JSON
void test_invalid_json()
{
    printf("\n=== test_invalid_json ===\n");
    Person p;
    bool ok = p.transform_from_json(std::string("{not valid json!!!}"));
    CHECK(ok == false);
}

// 测试14：bool 各种来源
void test_bool_various()
{
    printf("\n=== test_bool_various ===\n");

    // JSON bool 字面量
    BoolModel m;
    m.transform_from_json(std::string(
        "{\"a\":true,\"b\":false,\"c\":true,\"d\":false}"));
    CHECK(m.a == true);
    CHECK(m.b == false);
    CHECK(m.c == true);
    CHECK(m.d == false);

    // 字符串 "true" / "false"
    m.transform_from_json(std::string(
        "{\"a\":\"true\",\"b\":\"false\",\"c\":\"true\",\"d\":\"false\"}"));
    CHECK(m.a == true);
    CHECK(m.b == false);

    // 字符串 "1" / "0"
    m.transform_from_json(std::string(
        "{\"a\":\"1\",\"b\":\"0\",\"c\":\"1\",\"d\":\"0\"}"));
    CHECK(m.a == true);
    CHECK(m.b == false);

    // 往返
    m.a = true; m.b = false; m.c = true; m.d = false;
    std::string json = m.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    BoolModel m2;
    m2.transform_from_json(json);
    CHECK(m2.a == true);
    CHECK(m2.b == false);
    CHECK(m2.c == true);
    CHECK(m2.d == false);
}

// 测试15：int vector
void test_int_vector()
{
    printf("\n=== test_int_vector ===\n");
    IntListModel src;
    src.tag = "nums";
    src.nums.push_back(10);
    src.nums.push_back(20);
    src.nums.push_back(30);

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    IntListModel dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.tag == "nums");
    CHECK(dst.nums.size() == 3);
    CHECK(dst.nums[0] == 10);
    CHECK(dst.nums[1] == 20);
    CHECK(dst.nums[2] == 30);

    // 直接从 JSON 字符串
    IntListModel m;
    m.transform_from_json(std::string("{\"tag\":\"t\",\"nums\":[1,2,3,4,5]}"));
    CHECK(m.nums.size() == 5);
    CHECK(m.nums[4] == 5);
}

// 测试16：string vector
void test_string_vector()
{
    printf("\n=== test_string_vector ===\n");
    StrListModel src;
    src.tag = "strs";
    src.strs.push_back("apple");
    src.strs.push_back("banana");
    src.strs.push_back("cherry");

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    StrListModel dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.tag == "strs");
    CHECK(dst.strs.size() == 3);
    CHECK(dst.strs[0] == "apple");
    CHECK(dst.strs[1] == "banana");
    CHECK(dst.strs[2] == "cherry");
}

// 测试17：边界值（INT64_MIN/MAX, UINT64_MAX）
void test_boundary_values()
{
    printf("\n=== test_boundary_values ===\n");
    BoundaryModel src;
    src.minI64 = (-9223372036854775807LL - 1);
    src.maxI64 = 9223372036854775807LL;
    src.maxU64 = 18446744073709551615ULL;
    src.pi     = 3.141592653589793;

    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());

    BoundaryModel dst;
    bool ok = dst.transform_from_json(json);
    CHECK(ok);
    CHECK(dst.minI64 == src.minI64);
    CHECK(dst.maxI64 == src.maxI64);
    CHECK(dst.maxU64 == src.maxU64);
    CHECK(floatEq(dst.pi, src.pi, 1e-10));
}

// ============================================================
// main
// ============================================================
int main()
{
    printf("============================================\n");
    printf("  JsonTransformAuto Test Suite\n");
    printf("============================================\n");

    test_primitive_from_json();
    test_primitive_roundtrip();
    test_nested_from_json();
    test_nested_roundtrip();
    test_deep_nested_array_from_json();
    test_deep_nested_array_roundtrip();
    test_vector_of_vector_from_json();
    test_vector_of_vector_roundtrip();
    test_type_tolerance();
    test_missing_fields();
    test_empty_array();
    test_single_field();
    test_invalid_json();
    test_bool_various();
    test_int_vector();
    test_string_vector();
    test_boundary_values();

    printf("\n============================================\n");
    printf("  PASS: %d   FAIL: %d\n", s_passCount, s_failCount);
    printf("============================================\n");

    return s_failCount == 0 ? 0 : 1;
}
