#include <stdio.h>
#include "JsonTransformAuto.h"

// 1. 普通 enum（底层类型默认 int）
enum Direction {
    North = 0,
    South = 1,
    East  = 2,
    West  = 3
};

// 2. enum class（强类型）
enum class Color {
    Red   = 0,
    Green = 1,
    Blue  = 2
};

// 3. enum class 指定底层类型 uint8_t
enum class Status : uint8_t {
    Inactive = 0,
    Active   = 1,
    Pending  = 2
};

// 测试结构体：包含三种枚举
struct Player {
    std::string name;
    Direction   direction;
    Color       color;
    Status      status;
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(Player, name, direction, color, status)

// 测试结构体：枚举在 vector 里
struct Scene {
    std::string         scene_name;
    std::vector<Color>  palette;
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(Scene, scene_name, palette)

// ---- 工具函数 ----
static int s_pass = 0;
static int s_fail = 0;

#define CHECK(cond, msg) \
    do { \
        if (cond) { printf("[PASS] %s\n", msg); s_pass++; } \
        else      { printf("[FAIL] %s\n", msg); s_fail++; } \
    } while(0)

int main()
{
    printf("========== Enum Transform Tests ==========\n\n");

    // --------------------------------------------------
    // 1. from_json：普通字段
    // --------------------------------------------------
    {
        Player p;
        std::string json = R"({"name":"hero","direction":2,"color":1,"status":2})";
        bool ok = p.transform_from_json(json);
        CHECK(ok,                          "1.1 from_json returns true");
        CHECK(p.name      == "hero",       "1.2 string field");
        CHECK(p.direction == East,         "1.3 plain enum Direction");
        CHECK(p.color     == Color::Green, "1.4 enum class Color");
        CHECK(p.status    == Status::Pending, "1.5 enum class Status:uint8_t");
    }

    // --------------------------------------------------
    // 2. to_json：普通字段
    // --------------------------------------------------
    {
        Player p;
        p.name      = "hero";
        p.direction = West;
        p.color     = Color::Blue;
        p.status    = Status::Active;
        std::string json = p.transform_to_json();
        printf("2.0 to_json output: %s\n", json.c_str());
        // 反解回来验证
        Player p2;
        p2.transform_from_json(json);
        CHECK(p2.direction == West,          "2.1 plain enum roundtrip");
        CHECK(p2.color     == Color::Blue,   "2.2 enum class roundtrip");
        CHECK(p2.status    == Status::Active,"2.3 enum:uint8_t roundtrip");
    }

    // --------------------------------------------------
    // 3. 枚举默认值（字段缺失时不崩溃）
    // --------------------------------------------------
    {
        Player p;
        p.direction = North;
        p.color     = Color::Red;
        p.status    = Status::Inactive;
        bool ok = p.transform_from_json(R"({"name":"ghost"})");
        // 缺失字段 from_json 返回 false，但不应崩溃
        CHECK(!ok,                "3.1 returns false when fields missing");
        CHECK(p.name == "ghost",  "3.2 present field still parsed");
    }

    // --------------------------------------------------
    // 4. vector<enum class>
    // --------------------------------------------------
    {
        Scene s;
        bool ok = s.transform_from_json(R"({"scene_name":"sunset","palette":[0,2,1]})");
        CHECK(ok,                              "4.1 vector<enum> from_json ok");
        CHECK(s.palette.size() == 3,           "4.2 vector size");
        CHECK(s.palette[0] == Color::Red,      "4.3 palette[0]");
        CHECK(s.palette[1] == Color::Blue,     "4.4 palette[1]");
        CHECK(s.palette[2] == Color::Green,    "4.5 palette[2]");

        std::string json = s.transform_to_json();
        printf("4.6 vector<enum> to_json: %s\n", json.c_str());

        Scene s2;
        s2.transform_from_json(json);
        CHECK(s2.palette == s.palette, "4.7 vector<enum> roundtrip");
    }

    // --------------------------------------------------
    // 5. 边界：枚举值 0
    // --------------------------------------------------
    {
        Player p;
        p.transform_from_json(R"({"name":"zero","direction":0,"color":0,"status":0})");
        CHECK(p.direction == North,           "5.1 enum value 0 (North)");
        CHECK(p.color     == Color::Red,      "5.2 enum class value 0");
        CHECK(p.status    == Status::Inactive,"5.3 enum:uint8_t value 0");
    }

    printf("\n==========================================\n");
    printf("Result: %d passed, %d failed\n", s_pass, s_fail);
    return s_fail == 0 ? 0 : 1;
}
