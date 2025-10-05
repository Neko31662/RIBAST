#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "skill.h"

// 干员类
struct Operator {
    string name; // 干员代号（游戏中的名称）

    double moodConsumptionRate = 1.0; // 干员心情消耗速率 + 设施提供的心情消耗速率

  private:
    // 由基建技能提供的心情消耗速率（消耗增加为正值，减少为负值）
    double efficiencyEnhance = 0.0;      // 由干员基建技能增加的效率
    double efficiencyReduce = 0.0;       // 由干员基建技能减少的效率
    int capacityEnhance = 0;             // 由干员基建技能增加的容量上限
    int capacityReduce = 0;              // 由干员基建技能减少的容量上限
    double moodConsumptionEnhance = 0.0; // 由干员基建技能提供的心情消耗速率增加值
    double moodConsumptionReduce = 0.0;  // 由干员基建技能提供的心情消耗速率减少值
    friend void add_efficiency(Facility &facility, Operator &op, double value);
    friend void reduce_efficiency(Facility &facility, Operator &op, double value);
    friend void add_capacity(Facility &facility, Operator &op, int value);
    friend void reduce_capacity(Facility &facility, Operator &op, int value);
    friend void add_mood_consumption_rate(Facility &facility, Operator &op, double value);
    friend void reduce_mood_consumption_rate(Facility &facility, Operator &op, double value);

    struct {
        double efficiency_by_facility = 0.0; // 由设施数量提供的效率加成
    } spec;
    friend void add_efficiency_by_facility(Facility &facility, Operator &op, double value);

  public:
    double getEfficiencyEnhance() const { return efficiencyEnhance; }
    double getEfficiencyReduce() const { return efficiencyReduce; }
    int getCapacityEnhance() const { return capacityEnhance; }
    int getCapacityReduce() const { return capacityReduce; }
    double getMoodConsumptionEnhance() const { return moodConsumptionEnhance; }
    double getMoodConsumptionReduce() const { return moodConsumptionReduce; }

    void clearEfficiency() {
        efficiencyEnhance = 0.0;
        efficiencyReduce = 0.0;
    }

    void clearCapacity() {
        capacityEnhance = 0;
        capacityReduce = 0;
    }

    void clearMoodConsumption() {
        moodConsumptionEnhance = 0.0;
        moodConsumptionReduce = 0.0;
    }
};

// 势力-干员列表
static map<string, vector<string>> forces_opList = {
    {"作业平台", {"Lancet-2", "Castle-3", "THRM-EX", "正义骑士号", "Friston-3", "PhonoR-0"}},
    {"黑钢国际", {"雷蛇", "芙兰卡", "杰西卡", "香草", "杏仁", "寻澜"}},
    {"A1小队", {"芙蓉", "炎熔", "米格鲁", "芬", "克洛丝"}}};

// 判断干员是否属于某个势力
bool in_forces(Operator &op, string forcesName);

// 统计干员列表中属于某个势力的干员的数量
int opList_in_forces(const vector<Operator *> &opList, string forcesName);