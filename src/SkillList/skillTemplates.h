#pragma once
#include "../Skill.h"
#include "../globalParams.h"
#include "../operator.h"

// 技能模板函数

// 将该干员的 efficiencyEnhance 增加 value
void add_efficiency(Facility &facility, Operator &op, int value);
// 将该干员的 efficiencyReduce 增加 value
void reduce_efficiency(Facility &facility, Operator &op, int value);

// 将该干员的 moodConsumptionReduce 增加 value
void reduce_mood_consumption_rate(Facility &facility, Operator &op, int value);
// 将该干员的 moodConsumptionEnhance 增加 value
void add_mood_consumption_rate(Facility &facility, Operator &op, int value);

// 增加设施容量
void add_capacity(Facility &facility, Operator &op, int value);

// 减少设施容量
void reduce_capacity(Facility &facility, Operator &op, int value);

// 将制造站-赤金的 efficiencyEnhance 增加 value
void add_efficiency_gold(Facility &facility, Operator &op, int value);

// 将制造站-作战记录的 efficiencyEnhance 增加 value
void add_efficiency_records(Facility &facility, Operator &op, int value);

// 将制造站-源石碎片的 efficiencyEnhance 增加 value
void add_efficiency_originium(Facility &facility, Operator &op, int value);

// 将干员的 spec.efficiency_by_facility 增加 value
void add_efficiency_by_facility(Facility &facility, Operator &op, int value);

// 将贸易站的 efficiencyEnhance 增加 value
void add_efficiency_trade(Facility &facility, Operator &op, int value);

// 将贸易站-龙门币的 efficiencyEnhance 增加 value
void add_efficiency_LMD(Facility &facility, Operator &op, int value);

// 将贸易站-合成玉的 efficiencyEnhance 增加 value
void add_efficiency_orundum(Facility &facility, Operator &op, int value);

// 为贸易站-龙门设施的订单增加权重
void add_product_rate(Facility &facility,std::vector<double> rateValues);