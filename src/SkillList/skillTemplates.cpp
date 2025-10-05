#include <bits/stdc++.h>
using namespace std;

#include "../globalParams.h"
#include "skillTemplates.h"

void add_efficiency(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency函数：value不能为负");
    }
    op.efficiencyEnhance += value;
}

void reduce_efficiency(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("reduce_efficiency函数：value不能为负");
    }
    op.efficiencyReduce += value;
}

void reduce_mood_consumption_rate(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("reduce_mood_consumption_rate函数：value不能为负");
    }
    op.moodConsumptionReduce += value;
}

void add_mood_consumption_rate(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_mood_consumption_rate函数：value不能为负");
    }
    op.moodConsumptionEnhance += value;
}

void add_capacity(Facility &facility, Operator &op,int value) {
    if (value < 0) {
        throw invalid_argument("add_capacity函数：value不能为负");
    }
    op.capacityEnhance += value;
}

void reduce_capacity(Facility &facility, Operator &op,int value) {
    if (value < 0) {
        throw invalid_argument("reduce_capacity函数：value不能为负");
    }
    op.capacityReduce += value;
}

void add_efficiency_gold(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_gold函数：value不能为负");
    }
    if (facility.facilityType != MFG_GOLD) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_records(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_records函数：value不能为负");
    }
    if (facility.facilityType != MFG_RECORDS) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_originium(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_originium函数：value不能为负");
    }
    if (facility.facilityType != MFG_ORIGINIUM) {
        return;
    }
    add_efficiency(facility, op, value);
}

void add_efficiency_by_facility(Facility &facility, Operator &op, double value) {
    if (value < 0) {
        throw invalid_argument("add_efficiency_by_facility函数：value不能为负");
    }
    op.spec.efficiency_by_facility += value;
}