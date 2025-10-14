#pragma once
#include "skill.h"
#include <map>
#include <string>
#include <vector>

// 干员类
struct OperatorInfo {
    std::string name;       // 干员代号（游戏中的名称）
    int rarity;             // 稀有度，1-6星
    std::string profession; // 职业

    OperatorInfo() : name(""), rarity(1), profession("") {}

    OperatorInfo(std::string name_, int rarity_, std::string profession_)
        : name(name_), rarity(rarity_), profession(profession_) {}
};

struct Operator : public OperatorInfo {
    int phase = 0;                 // 精英化等级，1、2星的精0 30级视为精英1
    int moodConsumptionRate = 100; // 干员基础心情消耗速率
    int duration = 0;              // 进驻时长，单位小时，工位变化后，该值归0
    double mood = 24.0;            // 当前心情值

    Operator() : OperatorInfo() {}

    Operator(OperatorInfo &info, int phase_) : OperatorInfo(info), phase(phase_) {}

  private:
    // 由基建技能提供的心情消耗速率（消耗增加为正值，减少为负值）
    int efficiencyEnhance = 0;      // 由干员基建技能增加的效率
    int efficiencyReduce = 0;       // 由干员基建技能减少的效率
    int capacityEnhance = 0;        // 由干员基建技能增加的容量上限
    int capacityReduce = 0;         // 由干员基建技能减少的容量上限
    int moodConsumptionEnhance = 0; // 由干员基建技能提供的心情消耗速率增加值
    int moodConsumptionReduce = 0;  // 由干员基建技能提供的心情消耗速率减少值
    friend void add_efficiency(Facility &facility, Operator &op, int value);
    friend void reduce_efficiency(Facility &facility, Operator &op, int value);
    friend void add_capacity(Facility &facility, Operator &op, int value);
    friend void reduce_capacity(Facility &facility, Operator &op, int value);
    friend void add_mood_consumption_rate(Facility &facility, Operator &op, int value);
    friend void reduce_mood_consumption_rate(Facility &facility, Operator &op, int value);

    struct {
        int efficiency_by_facility = 0; // 由设施数量提供的效率加成
    } spec;
    friend void add_efficiency_by_facility(Facility &facility, Operator &op, int value);

  public:
    double getEfficiencyEnhance() const { return efficiencyEnhance; }
    double getEfficiencyReduce() const { return efficiencyReduce; }
    int getCapacityEnhance() const { return capacityEnhance; }
    int getCapacityReduce() const { return capacityReduce; }
    double getMoodConsumptionEnhance() const { return moodConsumptionEnhance; }
    double getMoodConsumptionReduce() const { return moodConsumptionReduce; }

    void clearEfficiency() {
        efficiencyEnhance = 0;
        efficiencyReduce = 0;
    }

    void clearCapacity() {
        capacityEnhance = 0;
        capacityReduce = 0;
    }

    void clearMoodConsumption() {
        moodConsumptionEnhance = 0;
        moodConsumptionReduce = 0;
    }
};

// 判断干员是否属于某个势力
bool in_forces(OperatorInfo &op, std::string forcesName);

// 统计干员列表中属于某个势力的干员的数量
int opList_in_forces(const std::vector<Operator *> &opList, std::string forcesName);