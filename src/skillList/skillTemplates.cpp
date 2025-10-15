#include "skillTemplates.h"
#include "globalParams.h"
using std::invalid_argument;
using std::string;
using std::vector;

void add_efficiency(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency函数：value不能为负");
    }
    op.efficiencyEnhance += value;
}

void reduce_efficiency(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("reduce_efficiency函数：value不能为负");
    }
    op.efficiencyReduce += value;
}

void reduce_mood_consumption_rate(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("reduce_mood_consumption_rate函数：value不能为负");
    }
    op.moodConsumptionReduce += value;
}

void add_mood_consumption_rate(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_mood_consumption_rate函数：value不能为负");
    }
    op.moodConsumptionEnhance += value;
}

void add_capacity(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_capacity函数：value不能为负");
    }
    op.capacityEnhance += value;
}

void reduce_capacity(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("reduce_capacity函数：value不能为负");
    }
    op.capacityReduce += value;
}

void add_efficiency_gold(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_gold函数：value不能为负");
    }
    if (facility.facilityType != MFG_GOLD) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_records(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_records函数：value不能为负");
    }
    if (facility.facilityType != MFG_RECORDS) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_originium(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_originium函数：value不能为负");
    }
    if (facility.facilityType != MFG_ORIGINIUM) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_by_facility(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_by_facility函数：value不能为负");
    }
    op.spec.efficiency_by_facility += value;
}

void add_efficiency_trade(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_trade函数：value不能为负");
    }
    if (facility.facilityType != TRADE_ORUNDUM && facility.facilityType != TRADE_LMD) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_LMD(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_LMD函数：value不能为负");
    }
    if (facility.facilityType != TRADE_LMD) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_orundum(Facility &facility, Operator &op, int value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_orundum函数：value不能为负");
    }
    if (facility.facilityType != TRADE_ORUNDUM) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_product_rate(Facility &facility, std::vector<double> rateValues) {
    if (facility.facilityType != TRADE_LMD) {
        throw invalid_argument("add_product_rate函数：facility不是贸易站-龙门币类型的设施");
    }
    if (rateValues.size() != 3) {
        throw invalid_argument("add_product_rate函数：rateValues大小必须为3");
    }
    Trade_LMD *tradeFacility = dynamic_cast<Trade_LMD *>(&facility);
    if (!tradeFacility) {
        throw std::runtime_error("add_product_rate函数：facility无法转换为Trade_LMD");
    }
    std::vector<double> newRate(3);
    for (int i = 0; i < 3; i++) {
        newRate[i] = tradeFacility->productRate[i] + rateValues[i];
    }
    tradeFacility->changeProductRate(newRate);
}