#pragma once
#include <string>
#include <functional>

struct Facility;
struct GlobalParams;
struct Operator;

// 基建技能类
struct Skill {
    // 技能优先级，数值越小优先级越高，即执行越靠前
    int priority;
    // 技能名称
    std::string name;
    // 描述技能效果的函数，前三个参数分别为 GlobalParams 、该技能生效的 Facility
    // 、该技能所属的干员的引用
    std::function<void(GlobalParams &, Facility &, Operator &)> func;
    // func 的参数存储在 Skill.args 内

    Skill() {};
    Skill(int p, std::string n, std::function<void(GlobalParams &, Facility &, Operator &)> f)
        : priority(p), name(n), func(std::move(f)) {}

    void apply(GlobalParams &GP, Facility &facility, Operator &op) const {
        apply_impl(GP, facility, op);
    }

  private:
    template <size_t... I>
    void apply_impl(GlobalParams &GP, Facility &facility, Operator &op) const {
        func(GP, facility, op);
    }
};