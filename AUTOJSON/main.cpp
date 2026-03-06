/**
 * JsonTransformAutoTest.cpp
 * 终版全量测试，对应 JsonTransformAuto.h（含根节点数组新接口）
 * 兼容 VS2013 / GCC 4.8.5
 *
 * 规则：所有使用 DECLARE__JSON__AUTO__TRANSFORM 的结构体必须在文件作用域定义。
 */

#include "JsonTransformAuto.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>

// ============================================================
// 辅助宏 & 统计
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

static bool feq(double a, double b, double eps = 1e-5) { return fabs(a-b) < eps; }

// ============================================================
// 所有结构体（必须文件作用域）
// ============================================================

// --- 全基础类型 ---
struct PrimitiveModel {
    int8_t i8; uint8_t u8; int16_t i16; uint16_t u16;
    int32_t i32; uint32_t u32; int64_t i64; uint64_t u64;
    float f32; double f64; bool b; std::string str;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(PrimitiveModel,
    i8, u8, i16, u16, i32, u32, i64, u64, f32, f64, b, str)

// --- 嵌套对象 ---
struct Address {
    std::string city;
    int32_t zip;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Address, city, zip)

struct Person {
    std::string name;
    int32_t age;
    Address addr;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age, addr)

// --- 三层嵌套 + 对象数组 ---
struct Employee {
    std::string name;
    int32_t level;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Employee, name, level)

struct Department {
    std::string name;
    std::vector<Employee> employees;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Department, name, employees)

struct Company {
    std::string name;
    std::vector<Department> departments;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Company, name, departments)

// --- vector<vector<int>> ---
struct Matrix {
    std::string name;
    std::vector<std::vector<int32_t>> rows;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, name, rows)

// --- 类型容错 ---
struct ToleranceModel {
    int32_t intFromStr;
    double  dblFromStr;
    bool    boolFromStr;
    std::string strFromInt;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(ToleranceModel, intFromStr, dblFromStr, boolFromStr, strFromInt)

// --- 单字段 ---
struct SingleField {
    int32_t value;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(SingleField, value)

// --- bool 专项 ---
struct BoolModel {
    bool a, b, c, d;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BoolModel, a, b, c, d)

// --- int vector ---
struct IntListModel {
    std::string tag;
    std::vector<int32_t> nums;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(IntListModel, tag, nums)

// --- string vector ---
struct StrListModel {
    std::string tag;
    std::vector<std::string> strs;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(StrListModel, tag, strs)

// --- 边界值 ---
struct BoundaryModel {
    int64_t minI64, maxI64;
    uint64_t maxU64;
    double pi;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BoundaryModel, minI64, maxI64, maxU64, pi)

// --- 嵌套对象内含数组 + 多 vector 并存 ---
struct TaggedAddress {
    std::string city;
    int32_t zip;
    std::vector<std::string> tags;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(TaggedAddress, city, zip, tags)

struct RichPerson {
    std::string name;
    int32_t age;
    TaggedAddress addr;
    std::vector<std::string> hobbies;
    std::vector<int32_t> scores;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(RichPerson, name, age, addr, hobbies, scores)

// --- vector<vector<对象>> ---
struct Point {
    int32_t x, y;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Point, x, y)

struct PolyLine {
    std::string name;
    std::vector<std::vector<Point>> segments;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(PolyLine, name, segments)

// --- 特殊字符串 ---
struct SpecialStrModel {
    std::string empty, withSpaces, chinese, escaped;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(SpecialStrModel, empty, withSpaces, chinese, escaped)

// --- 非法字符串容错 ---
struct ToleranceEdge {
    int8_t i8Overflow;
    int32_t fromBadStr;
    double fromBadDbl;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(ToleranceEdge, i8Overflow, fromBadStr, fromBadDbl)

// --- 零值专项 ---
struct ZeroModel {
    int32_t i; double d; bool b; std::string s;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(ZeroModel, i, d, b, s)

// --- 极大/极小浮点数 ---
struct FloatEdgeModel {
    double maxVal, minPos, negMax;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(FloatEdgeModel, maxVal, minPos, negMax)

// ============================================================
// ==================== 测试函数 ==============================
// ============================================================

// ------ T01：全基础类型 JSON → 模型 ------
void test_primitive_from_json()
{
    printf("\n=== T01: primitive from_json ===\n");
    const char* json =
        "{\"i8\":-100,\"u8\":200,\"i16\":-30000,\"u16\":60000,"
        "\"i32\":-2000000,\"u32\":4000000,\"i64\":-9000000000,"
        "\"u64\":18000000000,\"f32\":1.5,\"f64\":3.14159265,"
        "\"b\":true,\"str\":\"hello\"}";
    PrimitiveModel m;
    bool ok = m.transform_from_json(std::string(json));
    CHECK(ok);
    CHECK(m.i8 == -100);        CHECK(m.u8 == 200);
    CHECK(m.i16 == -30000);     CHECK(m.u16 == 60000);
    CHECK(m.i32 == -2000000);   CHECK(m.u32 == 4000000U);
    CHECK(m.i64 == (int64_t)(-9000000000LL));
    CHECK(m.u64 == (uint64_t)(18000000000ULL));
    CHECK(feq(m.f32, 1.5f));    CHECK(feq(m.f64, 3.14159265));
    CHECK(m.b == true);         CHECK(m.str == "hello");
}

// ------ T02：全基础类型 往返 ------
void test_primitive_roundtrip()
{
    printf("\n=== T02: primitive roundtrip ===\n");
    PrimitiveModel src;
    src.i8=-1; src.u8=255; src.i16=-32768; src.u16=65535;
    src.i32=-1000000; src.u32=3000000U;
    src.i64=-1234567890123LL; src.u64=9876543210ULL;
    src.f32=2.718f; src.f64=1.41421356; src.b=false; src.str="world";
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    PrimitiveModel dst;
    CHECK(dst.transform_from_json(json));
    CHECK(dst.i8==src.i8);   CHECK(dst.u8==src.u8);
    CHECK(dst.i16==src.i16); CHECK(dst.u16==src.u16);
    CHECK(dst.i32==src.i32); CHECK(dst.u32==src.u32);
    CHECK(dst.i64==src.i64); CHECK(dst.u64==src.u64);
    CHECK(feq(dst.f32,src.f32,1e-3)); CHECK(feq(dst.f64,src.f64));
    CHECK(dst.b==src.b);     CHECK(dst.str==src.str);
}

// ------ T03：嵌套对象 JSON → 模型 ------
void test_nested_from_json()
{
    printf("\n=== T03: nested object from_json ===\n");
    Person p;
    CHECK(p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"Beijing\",\"zip\":100000}}")));
    CHECK(p.name=="Alice"); CHECK(p.age==30);
    CHECK(p.addr.city=="Beijing"); CHECK(p.addr.zip==100000);
}

// ------ T04：嵌套对象 往返 ------
void test_nested_roundtrip()
{
    printf("\n=== T04: nested object roundtrip ===\n");
    Person src; src.name="Bob"; src.age=25; src.addr.city="Shanghai"; src.addr.zip=200000;
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    Person dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.name==src.name); CHECK(dst.age==src.age);
    CHECK(dst.addr.city==src.addr.city); CHECK(dst.addr.zip==src.addr.zip);
}

// ------ T05：三层嵌套 + 对象数组 JSON → 模型 ------
void test_deep_nested_from_json()
{
    printf("\n=== T05: deep nested array from_json ===\n");
    Company c;
    CHECK(c.transform_from_json(std::string(
        "{\"name\":\"TechCorp\",\"departments\":["
        "{\"name\":\"Eng\",\"employees\":[{\"name\":\"Tom\",\"level\":3},{\"name\":\"Jerry\",\"level\":2}]},"
        "{\"name\":\"Mkt\",\"employees\":[{\"name\":\"Kate\",\"level\":4}]}]}")));
    CHECK(c.name=="TechCorp");
    CHECK(c.departments.size()==2);
    CHECK(c.departments[0].name=="Eng");
    CHECK(c.departments[0].employees[0].name=="Tom");  CHECK(c.departments[0].employees[0].level==3);
    CHECK(c.departments[0].employees[1].name=="Jerry"); CHECK(c.departments[0].employees[1].level==2);
    CHECK(c.departments[1].name=="Mkt");
    CHECK(c.departments[1].employees[0].name=="Kate");  CHECK(c.departments[1].employees[0].level==4);
}

// ------ T06：三层嵌套 往返 ------
void test_deep_nested_roundtrip()
{
    printf("\n=== T06: deep nested array roundtrip ===\n");
    Company src; src.name="TestCo";
    Department d1; d1.name="R&D";
    Employee e1; e1.name="Alice"; e1.level=5; d1.employees.push_back(e1);
    Employee e2; e2.name="Bob";   e2.level=3; d1.employees.push_back(e2);
    src.departments.push_back(d1);
    Department d2; d2.name="HR";
    Employee e3; e3.name="Carol"; e3.level=2; d2.employees.push_back(e3);
    src.departments.push_back(d2);
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    Company dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.departments[0].name=="R&D");
    CHECK(dst.departments[0].employees[0].name=="Alice"); CHECK(dst.departments[0].employees[0].level==5);
    CHECK(dst.departments[0].employees[1].name=="Bob");
    CHECK(dst.departments[1].name=="HR");
    CHECK(dst.departments[1].employees[0].name=="Carol");
}

// ------ T07：vector<vector<int>> 往返 ------
void test_vector_of_vector_int()
{
    printf("\n=== T07: vector<vector<int>> ===\n");
    Matrix m;
    CHECK(m.transform_from_json(std::string("{\"name\":\"mat\",\"rows\":[[1,2,3],[4,5,6],[7,8,9]]}")));
    CHECK(m.rows.size()==3);
    CHECK(m.rows[0][0]==1); CHECK(m.rows[1][1]==5); CHECK(m.rows[2][2]==9);

    Matrix src; src.name="id";
    std::vector<int32_t> r0; r0.push_back(1); r0.push_back(0); src.rows.push_back(r0);
    std::vector<int32_t> r1; r1.push_back(0); r1.push_back(1); src.rows.push_back(r1);
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    Matrix dst; dst.transform_from_json(json);
    CHECK(dst.rows[0][0]==1 && dst.rows[0][1]==0);
    CHECK(dst.rows[1][0]==0 && dst.rows[1][1]==1);
}

// ------ T08：vector<vector<对象>> 往返 ------
void test_vector_of_vector_object()
{
    printf("\n=== T08: vector<vector<object>> roundtrip ===\n");
    PolyLine src; src.name="poly";
    std::vector<Point> seg0;
    Point p0; p0.x=0; p0.y=0; seg0.push_back(p0);
    Point p1; p1.x=1; p1.y=2; seg0.push_back(p1);
    std::vector<Point> seg1;
    Point p2; p2.x=10; p2.y=20; seg1.push_back(p2);
    Point p3; p3.x=30; p3.y=40; seg1.push_back(p3);
    src.segments.push_back(seg0);
    src.segments.push_back(seg1);
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    PolyLine dst; dst.transform_from_json(json);
    CHECK(dst.segments.size()==2);
    CHECK(dst.segments[0][0].x==0  && dst.segments[0][0].y==0);
    CHECK(dst.segments[0][1].x==1  && dst.segments[0][1].y==2);
    CHECK(dst.segments[1][0].x==10 && dst.segments[1][0].y==20);
    CHECK(dst.segments[1][1].x==30 && dst.segments[1][1].y==40);
}

// ------ T09：嵌套对象内含数组 + 多 vector 并存 ------
void test_nested_with_vectors()
{
    printf("\n=== T09: nested object with multiple vectors ===\n");
    RichPerson src; src.name="Dave"; src.age=35;
    src.addr.city="Guangzhou"; src.addr.zip=510000;
    src.addr.tags.push_back("home"); src.addr.tags.push_back("work");
    src.hobbies.push_back("reading"); src.hobbies.push_back("coding");
    src.scores.push_back(95); src.scores.push_back(87); src.scores.push_back(92);
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    RichPerson dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.name=="Dave"); CHECK(dst.age==35);
    CHECK(dst.addr.city=="Guangzhou"); CHECK(dst.addr.zip==510000);
    CHECK(dst.addr.tags.size()==2);
    CHECK(dst.addr.tags[0]=="home"); CHECK(dst.addr.tags[1]=="work");
    CHECK(dst.hobbies.size()==2);
    CHECK(dst.hobbies[0]=="reading"); CHECK(dst.hobbies[1]=="coding");
    CHECK(dst.scores.size()==3);
    CHECK(dst.scores[0]==95); CHECK(dst.scores[1]==87); CHECK(dst.scores[2]==92);
}

// ------ T10：类型容错（字符串↔数字/bool）------
void test_type_tolerance()
{
    printf("\n=== T10: type tolerance ===\n");
    ToleranceModel m;
    m.transform_from_json(std::string(
        "{\"intFromStr\":\"42\",\"dblFromStr\":\"3.14\","
        "\"boolFromStr\":\"true\",\"strFromInt\":100}"));
    CHECK(m.intFromStr==42);
    CHECK(feq(m.dblFromStr,3.14));
    CHECK(m.boolFromStr==true);
    CHECK(m.strFromInt=="100");
}

// ------ T11：非法字符串转数字 → 返回 0，不崩溃 ------
void test_bad_string_to_number()
{
    printf("\n=== T11: bad string to number ===\n");
    ToleranceEdge m; m.fromBadStr=0; m.fromBadDbl=0.0;
    m.transform_from_json(std::string(
        "{\"i8Overflow\":9999,\"fromBadStr\":\"abc\",\"fromBadDbl\":\"xyz\"}"));
    CHECK(true);                       // 不崩溃
    CHECK(m.fromBadStr==0);
    CHECK(feq(m.fromBadDbl,0.0));
}

// ------ T12：缺失字段保持原值 ------
void test_missing_fields()
{
    printf("\n=== T12: missing fields keep original value ===\n");
    Person p; p.age=99; p.addr.city="X"; p.addr.zip=1;
    p.transform_from_json(std::string("{\"name\":\"Ghost\"}"));
    CHECK(p.name=="Ghost");
    CHECK(p.age==99);
    CHECK(p.addr.city=="X"); CHECK(p.addr.zip==1);
}

// ------ T13：深层嵌套缺失字段 ------
void test_deep_missing_fields()
{
    printf("\n=== T13: deep nested missing fields ===\n");
    Person p; p.addr.zip=12345;
    p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\"}}"));
    CHECK(p.name=="Alice"); CHECK(p.age==30);
    CHECK(p.addr.city=="BJ");
    CHECK(p.addr.zip==12345);  // zip 缺失，保持原值
}

// ------ T14：空数组 ------
void test_empty_array()
{
    printf("\n=== T14: empty array ===\n");
    // 空对象数组
    Company c;
    c.transform_from_json(std::string("{\"name\":\"e\",\"departments\":[]}"));
    CHECK(c.name=="e"); CHECK(c.departments.size()==0);
    // 空 int 数组
    IntListModel m;
    m.transform_from_json(std::string("{\"tag\":\"t\",\"nums\":[]}"));
    CHECK(m.nums.size()==0);
    // 空 string 数组
    StrListModel s;
    s.transform_from_json(std::string("{\"tag\":\"t\",\"strs\":[]}"));
    CHECK(s.strs.size()==0);
    // 空数组往返
    IntListModel src; src.tag="empty";
    IntListModel dst; dst.transform_from_json(src.transform_to_json());
    CHECK(dst.nums.size()==0);
}

// ------ T15：单字段结构体 ------
void test_single_field()
{
    printf("\n=== T15: single field ===\n");
    SingleField s; s.value=0;
    s.transform_from_json(std::string("{\"value\":777}"));
    CHECK(s.value==777);
    s.value=999;
    SingleField s2; s2.transform_from_json(s.transform_to_json());
    CHECK(s2.value==999);
}

// ------ T16：非法 JSON ------
void test_invalid_json()
{
    printf("\n=== T16: invalid json ===\n");
    Person p;
    // 非法 JSON → false
    CHECK(p.transform_from_json(std::string("{not valid}")) == false);
    // 空字符串 → false
    CHECK(p.transform_from_json(std::string("")) == false);
    // 根节点是数组（单对象接口）→ false（新行为：IsObject 检查）
    CHECK(p.transform_from_json(std::string("[{\"name\":\"x\",\"age\":1,\"addr\":{\"city\":\"c\",\"zip\":1}}]")) == false);
}

// ------ T17：bool 各种来源 ------
void test_bool_various()
{
    printf("\n=== T17: bool various sources ===\n");
    BoolModel m;
    // JSON bool 字面量
    m.transform_from_json(std::string("{\"a\":true,\"b\":false,\"c\":true,\"d\":false}"));
    CHECK(m.a==true); CHECK(m.b==false);
    // 字符串 "true"/"false"
    m.transform_from_json(std::string("{\"a\":\"true\",\"b\":\"false\",\"c\":\"true\",\"d\":\"false\"}"));
    CHECK(m.a==true); CHECK(m.b==false);
    // 字符串 "1"/"0"
    m.transform_from_json(std::string("{\"a\":\"1\",\"b\":\"0\",\"c\":\"1\",\"d\":\"0\"}"));
    CHECK(m.a==true); CHECK(m.b==false); CHECK(m.c==true); CHECK(m.d==false);
    // 往返
    m.a=true; m.b=false; m.c=true; m.d=false;
    BoolModel m2; m2.transform_from_json(m.transform_to_json());
    CHECK(m2.a==true); CHECK(m2.b==false); CHECK(m2.c==true); CHECK(m2.d==false);
}

// ------ T18：int vector 往返 ------
void test_int_vector()
{
    printf("\n=== T18: int vector ===\n");
    IntListModel src; src.tag="nums";
    src.nums.push_back(10); src.nums.push_back(20); src.nums.push_back(30);
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    IntListModel dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.tag=="nums");
    CHECK(dst.nums.size()==3);
    CHECK(dst.nums[0]==10); CHECK(dst.nums[1]==20); CHECK(dst.nums[2]==30);
}

// ------ T19：string vector 往返 ------
void test_string_vector()
{
    printf("\n=== T19: string vector ===\n");
    StrListModel src; src.tag="strs";
    src.strs.push_back("apple"); src.strs.push_back("banana"); src.strs.push_back("cherry");
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    StrListModel dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.strs.size()==3);
    CHECK(dst.strs[0]=="apple"); CHECK(dst.strs[1]=="banana"); CHECK(dst.strs[2]=="cherry");
}

// ------ T20：边界值（INT64_MIN/MAX, UINT64_MAX）------
void test_boundary_values()
{
    printf("\n=== T20: boundary values ===\n");
    BoundaryModel src;
    src.minI64=(-9223372036854775807LL-1);
    src.maxI64=9223372036854775807LL;
    src.maxU64=18446744073709551615ULL;
    src.pi=3.141592653589793;
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    BoundaryModel dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.minI64==src.minI64); CHECK(dst.maxI64==src.maxI64);
    CHECK(dst.maxU64==src.maxU64);
    CHECK(feq(dst.pi,src.pi,1e-10));
}

// ------ T21：特殊字符串（空串、中文、换行转义）------
void test_special_strings()
{
    printf("\n=== T21: special strings ===\n");
    SpecialStrModel src;
    src.empty=""; src.withSpaces="hello world";
    src.chinese="\xe4\xb8\xad\xe6\x96\x87";  // UTF-8: 中文
    src.escaped="line1\nline2";
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    SpecialStrModel dst; CHECK(dst.transform_from_json(json));
    CHECK(dst.empty==""); CHECK(dst.withSpaces=="hello world");
    CHECK(dst.chinese==src.chinese); CHECK(dst.escaped==src.escaped);
}

// ------ T22：JSON 字段顺序打乱 ------
void test_field_order_shuffled()
{
    printf("\n=== T22: shuffled field order ===\n");
    Address a;
    a.transform_from_json(std::string("{\"zip\":888,\"city\":\"SZ\"}"));
    CHECK(a.city=="SZ"); CHECK(a.zip==888);
    Person p;
    p.transform_from_json(std::string("{\"age\":20,\"addr\":{\"zip\":99,\"city\":\"X\"},\"name\":\"Z\"}"));
    CHECK(p.name=="Z"); CHECK(p.age==20);
    CHECK(p.addr.city=="X"); CHECK(p.addr.zip==99);
}

// ------ T23：序列化幂等性 ------
void test_to_json_idempotent()
{
    printf("\n=== T23: to_json idempotent ===\n");
    Person p; p.name="Eve"; p.age=28; p.addr.city="CD"; p.addr.zip=610000;
    CHECK(p.transform_to_json()==p.transform_to_json());
    // 含 vector 的结构体也验证
    RichPerson rp; rp.name="X"; rp.age=1; rp.addr.city="Y"; rp.addr.zip=0;
    rp.scores.push_back(1); rp.scores.push_back(2);
    CHECK(rp.transform_to_json()==rp.transform_to_json());
}

// ------ T24：transform_to_json(Value&, Allocator&) 带参重载 ------
void test_to_json_value_overload()
{
    printf("\n=== T24: to_json(Value&, Allocator&) overload ===\n");
    Address src; src.city="Wuhan"; src.zip=430000;
    rapidjson::Document doc; doc.SetObject();
    auto& alloc = doc.GetAllocator();
    rapidjson::Value addrVal(rapidjson::kObjectType);
    src.transform_to_json(addrVal, alloc);
    rapidjson::Value key; key.SetString("addr", alloc);
    doc.AddMember(key, addrVal, alloc);
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    doc.Accept(writer);
    std::string json = buf.GetString();
    printf("  JSON: %s\n", json.c_str());
    rapidjson::Document doc2; doc2.Parse(json.c_str());
    CHECK(!doc2.HasParseError());
    Address dst; dst.transform_from_json(doc2["addr"]);
    CHECK(dst.city=="Wuhan"); CHECK(dst.zip==430000);
}

// ------ T25：多 vector 字段互不干扰 ------
void test_multiple_vectors()
{
    printf("\n=== T25: multiple vector fields ===\n");
    RichPerson src; src.name="Frank"; src.age=40;
    src.addr.city="HZ"; src.addr.zip=310000;
    src.addr.tags.push_back("office");
    src.hobbies.push_back("swim"); src.hobbies.push_back("hike");
    src.scores.push_back(80); src.scores.push_back(90);
    RichPerson dst; dst.transform_from_json(src.transform_to_json());
    CHECK(dst.addr.tags.size()==1);  CHECK(dst.addr.tags[0]=="office");
    CHECK(dst.hobbies.size()==2);    CHECK(dst.hobbies[0]=="swim");
    CHECK(dst.scores.size()==2);     CHECK(dst.scores[1]==90);
}

// ------ T26：JSON 含多余未知字段 ------
void test_extra_fields()
{
    printf("\n=== T26: extra unknown fields in json ===\n");
    Person p;
    p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\",\"zip\":100},"
        "\"unknown\":\"xxx\",\"extra\":999,\"nested\":{\"a\":1}}"));
    CHECK(p.name=="Alice"); CHECK(p.age==30); CHECK(p.addr.city=="BJ");
    CHECK(true); // 不崩溃
}

// ------ T27：JSON 字段值为 null ------
void test_null_field_value()
{
    printf("\n=== T27: null field value ===\n");
    Person p; p.name="orig"; p.age=30;
    p.transform_from_json(std::string(
        "{\"name\":null,\"age\":30,\"addr\":{\"city\":\"X\",\"zip\":1}}"));
    CHECK(p.age==30);
    CHECK(true); // 不崩溃
}

// ------ T28：数组元素含 null ------
void test_null_in_array()
{
    printf("\n=== T28: null element in array ===\n");
    IntListModel m;
    m.transform_from_json(std::string("{\"tag\":\"t\",\"nums\":[1,null,3]}"));
    CHECK(true); // 不崩溃
}

// ------ T29：嵌套对象字段值类型错误（期望 object，给了字符串）------
void test_wrong_type_for_object_field()
{
    printf("\n=== T29: wrong type for object field ===\n");
    Person p; p.addr.city="orig"; p.addr.zip=1;
    p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":\"not_an_object\"}"));
    CHECK(p.name=="Alice"); CHECK(p.age==30);
    CHECK(p.addr.city=="orig"); // addr 解析失败，保持原值
    CHECK(true);
}

// ------ T30：数组字段值类型错误（期望 array，给了数字）------
void test_wrong_type_for_array_field()
{
    printf("\n=== T30: wrong type for array field ===\n");
    IntListModel m;
    m.transform_from_json(std::string("{\"tag\":\"t\",\"nums\":123}"));
    CHECK(m.tag=="t");
    CHECK(m.nums.size()==0); // 保持空
    CHECK(true);
}

// ------ T31：极大/极小浮点数 往返 ------
void test_float_edge_values()
{
    printf("\n=== T31: float edge values ===\n");
    FloatEdgeModel src;
    src.maxVal=std::numeric_limits<double>::max();
    src.minPos=std::numeric_limits<double>::min();
    src.negMax=-std::numeric_limits<double>::max();
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    FloatEdgeModel dst; dst.transform_from_json(json);
    CHECK(dst.maxVal > 0); CHECK(dst.minPos > 0); CHECK(dst.negMax < 0);
}

// ------ T32：零值字段序列化不丢失 ------
void test_zero_values()
{
    printf("\n=== T32: zero values not lost ===\n");
    ZeroModel src; src.i=0; src.d=0.0; src.b=false; src.s="";
    std::string json = src.transform_to_json();
    printf("  JSON: %s\n", json.c_str());
    ZeroModel dst; dst.i=99; dst.d=3.14; dst.b=true; dst.s="x";
    dst.transform_from_json(json);
    CHECK(dst.i==0); CHECK(feq(dst.d,0.0));
    CHECK(dst.b==false); CHECK(dst.s=="");
}

// ------ T33：同一对象反复解析，第二次完整覆盖第一次 ------
void test_reparse_overwrite()
{
    printf("\n=== T33: reparse overwrites previous values ===\n");
    Person p;
    p.transform_from_json(std::string("{\"name\":\"First\",\"age\":10,\"addr\":{\"city\":\"A\",\"zip\":1}}"));
    CHECK(p.name=="First"); CHECK(p.age==10);
    p.transform_from_json(std::string("{\"name\":\"Second\",\"age\":20,\"addr\":{\"city\":\"B\",\"zip\":2}}"));
    CHECK(p.name=="Second"); CHECK(p.age==20);
    CHECK(p.addr.city=="B"); CHECK(p.addr.zip==2);
}

// ------ T34：vector 反序列化是覆盖而非追加 ------
void test_vector_reparse_overwrite()
{
    printf("\n=== T34: vector reparse overwrites not appends ===\n");
    IntListModel m;
    m.nums.push_back(1); m.nums.push_back(2); m.nums.push_back(3);
    m.transform_from_json(std::string("{\"tag\":\"t\",\"nums\":[4,5]}"));
    CHECK(m.nums.size()==2);
    CHECK(m.nums[0]==4); CHECK(m.nums[1]==5);
}

// ============================================================
// T35~T43: 根节点为数组（新接口：transform_from_json / transform_to_json）
// ============================================================

// ------ T35：vector<reflect对象> 根数组 往返 ------
void test_array_root_object_roundtrip()
{
    printf("\n=== T35: array root - vector<object> roundtrip ===\n");
    std::vector<Person> src;
    Person p1; p1.name="Alice"; p1.age=20; p1.addr.city="BJ"; p1.addr.zip=100;
    Person p2; p2.name="Bob";   p2.age=30; p2.addr.city="SH"; p2.addr.zip=200;
    Person p3; p3.name="Carol"; p3.age=25; p3.addr.city="GZ"; p3.addr.zip=300;
    src.push_back(p1); src.push_back(p2); src.push_back(p3);

    std::string json = auto_json::transform_to_json(src);
    printf("  JSON: %s\n", json.c_str());

    std::vector<Person> dst;
    bool ok = auto_json::transform_from_json(dst, json);
    CHECK(ok);
    CHECK(dst.size()==3);
    CHECK(dst[0].name=="Alice"); CHECK(dst[0].age==20); CHECK(dst[0].addr.city=="BJ");
    CHECK(dst[1].name=="Bob");   CHECK(dst[1].age==30); CHECK(dst[1].addr.city=="SH");
    CHECK(dst[2].name=="Carol"); CHECK(dst[2].age==25); CHECK(dst[2].addr.city=="GZ");
}

// ------ T36：vector<int> 根数组 往返 ------
void test_array_root_int_roundtrip()
{
    printf("\n=== T36: array root - vector<int> roundtrip ===\n");
    std::vector<int32_t> src;
    src.push_back(10); src.push_back(20); src.push_back(30);
    std::string json = auto_json::transform_to_json(src);
    printf("  JSON: %s\n", json.c_str());
    std::vector<int32_t> dst;
    CHECK(auto_json::transform_from_json(dst, json));
    CHECK(dst.size()==3);
    CHECK(dst[0]==10); CHECK(dst[1]==20); CHECK(dst[2]==30);
}

// ------ T37：vector<string> 根数组 往返（含空串）------
void test_array_root_string_roundtrip()
{
    printf("\n=== T37: array root - vector<string> roundtrip ===\n");
    std::vector<std::string> src;
    src.push_back("hello"); src.push_back("world"); src.push_back("");
    std::string json = auto_json::transform_to_json(src);
    printf("  JSON: %s\n", json.c_str());
    std::vector<std::string> dst;
    CHECK(auto_json::transform_from_json(dst, json));
    CHECK(dst.size()==3);
    CHECK(dst[0]=="hello"); CHECK(dst[1]=="world"); CHECK(dst[2]=="");
}

// ------ T38：空数组根节点 往返 ------
void test_array_root_empty()
{
    printf("\n=== T38: array root - empty array ===\n");
    std::vector<Person> src;
    std::string json = auto_json::transform_to_json(src);
    printf("  JSON: %s\n", json.c_str());
    CHECK(json=="[]");
    std::vector<Person> dst;
    CHECK(auto_json::transform_from_json(dst, json));
    CHECK(dst.size()==0);
}

// ------ T39：根节点是对象，用数组接口解析 → 返回 false ------
void test_array_root_given_object_json()
{
    printf("\n=== T39: array root - object json returns false ===\n");
    std::vector<Person> dst;
    bool ok = auto_json::transform_from_json(dst,
        std::string("{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\",\"zip\":1}}"));
    CHECK(ok==false);
    CHECK(dst.size()==0);
}

// ------ T40：根节点数组 + 非法 JSON → 返回 false ------
void test_array_root_invalid_json()
{
    printf("\n=== T40: array root - invalid json returns false ===\n");
    std::vector<int32_t> dst;
    CHECK(auto_json::transform_from_json(dst, std::string("[1,2,")) == false);
    CHECK(auto_json::transform_from_json(dst, std::string("")) == false);
}

// ------ T41：vector<深层嵌套对象> 根数组 往返 ------
void test_array_root_deep_object()
{
    printf("\n=== T41: array root - deep nested objects ===\n");
    std::vector<Company> src;
    Company c1; c1.name="Corp1";
    Department d1; d1.name="Eng";
    Employee e1; e1.name="Tom"; e1.level=3; d1.employees.push_back(e1);
    c1.departments.push_back(d1);
    src.push_back(c1);
    Company c2; c2.name="Corp2";
    Department d2; d2.name="HR";
    Employee e2; e2.name="Jane"; e2.level=2; d2.employees.push_back(e2);
    c2.departments.push_back(d2);
    src.push_back(c2);

    std::string json = auto_json::transform_to_json(src);
    printf("  JSON: %s\n", json.c_str());

    std::vector<Company> dst;
    CHECK(auto_json::transform_from_json(dst, json));
    CHECK(dst.size()==2);
    CHECK(dst[0].name=="Corp1");
    CHECK(dst[0].departments[0].name=="Eng");
    CHECK(dst[0].departments[0].employees[0].name=="Tom");
    CHECK(dst[0].departments[0].employees[0].level==3);
    CHECK(dst[1].name=="Corp2");
    CHECK(dst[1].departments[0].employees[0].name=="Jane");
}

// ------ T42：根数组 反复解析是覆盖而非追加 ------
void test_array_root_reparse_overwrite()
{
    printf("\n=== T42: array root - reparse overwrites ===\n");
    std::vector<int32_t> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    auto_json::transform_from_json(v, std::string("[4,5]"));
    CHECK(v.size()==2);
    CHECK(v[0]==4); CHECK(v[1]==5);
}

// ------ T43：根数组对象，根节点给了数字数组（类型不匹配）------
void test_array_root_type_mismatch()
{
    printf("\n=== T43: array root - int array into object vector ===\n");
    // 期望 vector<Person>，但 JSON 是 [1,2,3]（纯数字数组）
    // 每个元素 IsObject() 会返回 false，from_json 返回 false，不崩溃
    std::vector<Person> dst;
    auto_json::transform_from_json(dst, std::string("[1,2,3]"));
    CHECK(true); // 不崩溃即通过
}

// ============================================================
// main
// ============================================================
int main()
{
    printf("============================================\n");
    printf("  JsonTransformAuto Full Test Suite\n");
    printf("============================================\n");

    test_primitive_from_json();             // T01
    test_primitive_roundtrip();             // T02
    test_nested_from_json();                // T03
    test_nested_roundtrip();                // T04
    test_deep_nested_from_json();           // T05
    test_deep_nested_roundtrip();           // T06
    test_vector_of_vector_int();            // T07
    test_vector_of_vector_object();         // T08
    test_nested_with_vectors();             // T09
    test_type_tolerance();                  // T10
    test_bad_string_to_number();            // T11
    test_missing_fields();                  // T12
    test_deep_missing_fields();             // T13
    test_empty_array();                     // T14
    test_single_field();                    // T15
    test_invalid_json();                    // T16
    test_bool_various();                    // T17
    test_int_vector();                      // T18
    test_string_vector();                   // T19
    test_boundary_values();                 // T20
    test_special_strings();                 // T21
    test_field_order_shuffled();            // T22
    test_to_json_idempotent();              // T23
    test_to_json_value_overload();          // T24
    test_multiple_vectors();                // T25
    test_extra_fields();                    // T26
    test_null_field_value();                // T27
    test_null_in_array();                   // T28
    test_wrong_type_for_object_field();     // T29
    test_wrong_type_for_array_field();      // T30
    test_float_edge_values();               // T31
    test_zero_values();                     // T32
    test_reparse_overwrite();               // T33
    test_vector_reparse_overwrite();        // T34
    test_array_root_object_roundtrip();     // T35
    test_array_root_int_roundtrip();        // T36
    test_array_root_string_roundtrip();     // T37
    test_array_root_empty();                // T38
    test_array_root_given_object_json();    // T39
    test_array_root_invalid_json();         // T40
    test_array_root_deep_object();          // T41
    test_array_root_reparse_overwrite();    // T42
    test_array_root_type_mismatch();        // T43

    printf("\n============================================\n");
    printf("  PASS: %d   FAIL: %d   TOTAL: %d\n",
        s_passCount, s_failCount, s_passCount + s_failCount);
    printf("============================================\n");

    return s_failCount == 0 ? 0 : 1;
}
