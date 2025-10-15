#include "arrangement.h"

bool fitOperatorLimit(int facilityType, int level, int operatorCount) {
    if (isMfg(facilityType) || isTrade(facilityType) || facilityType == CONTROL) {
        return operatorCount <= std::max(level, 1);
    } else if (facilityType == DORMITORY) {
        return operatorCount <= 5;
    } else if (facilityType == TRAINING || facilityType == MEETING) {
        return operatorCount <= 2;
    } else if (facilityType == PROCESSING || facilityType == POWER || facilityType == OFFICE) {
        return operatorCount <= 1;
    }
    return false;
}

bool Arrangement::checkLegal() {
    illegalArrangementsInfo.clear();

    std::array<int, FACILITY_TYPE_COUNT + 1> facilityCount = {0};
    std::array<int, FACILITY_TYPE_COUNT + 1> facilityMaxLevel = {0};

    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        auto &levels = facilities[tp];
        facilityCount[tp] = (int)levels.size();
        for (auto level : levels) {
            if (level < 1) {
                illegalArrangementsInfo.push_back("存在设施[" + getFacilityTypeName(tp) +
                                                  "]等级为[" + std::to_string(level) + "]小于1");
            }
            facilityMaxLevel[tp] = std::max(facilityMaxLevel[tp], level);
        }
        for (auto level : levels) {
            if (tp == POWER && level != 3) {
                illegalArrangementsInfo.push_back("发电站等级只能为3，检测到等级为[" +
                                                  std::to_string(level) + "]");
                break;
            }
        }
    }

    int totalCount = 0;
    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        if (isUniqueFacility(tp)) {
            if (facilityCount[tp] > 1) {
                illegalArrangementsInfo.push_back("上限为1的设施[" + getFacilityTypeName(tp) +
                                                  "]数量不能超过1个，检测到数量为[" +
                                                  std::to_string(facilityCount[tp]) + "]");
            }
        } else if (tp == DORMITORY) {
            if (facilityCount[tp] > 4) {
                illegalArrangementsInfo.push_back("宿舍数量不能超过4个，检测到数量为[" +
                                                  std::to_string(facilityCount[tp]) + "]");
            }
        }
        if (isMfg(tp) || isTrade(tp) || tp == POWER) {
            totalCount += facilityCount[tp];
        }
    }
    if (totalCount > 9) {
        illegalArrangementsInfo.push_back(
            "制造站、贸易站、发电站设施数量之和不能超过9个，检测到数量为[" +
            std::to_string(totalCount) + "]");
    }

    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        if (tp == POWER)
            continue;
        if (isMfg(tp) || isTrade(tp) || tp == MEETING || tp == OFFICE || tp == PROCESSING ||
            tp == TRAINING) {
            if (facilityMaxLevel[tp] > 3) {
                illegalArrangementsInfo.push_back(
                    "检测到存在设施[" + getFacilityTypeName(tp) + "]等级为[" +
                    std::to_string(facilityMaxLevel[tp]) + "]，超过上限3级");
            }
        }
        if (tp == DORMITORY || tp == CONTROL) {
            if (facilityMaxLevel[tp] > 5) {
                illegalArrangementsInfo.push_back(
                    "检测到存在设施[" + getFacilityTypeName(tp) + "]等级为[" +
                    std::to_string(facilityMaxLevel[tp]) + "]，超过上限5级");
            }
        }
    }

    if (scheduleDuration.size() != schedulePlan.size()) {
        illegalArrangementsInfo.push_back("排班计划的班次数量与排班表数量不匹配");
    }

    for (auto duration : scheduleDuration) {
        if (duration < 1) {
            illegalArrangementsInfo.push_back("排班计划的每个班次持续时间必须大于0，检测到[" +
                                              std::to_string(duration) + "]");
            break;
        }
    }

    for (int idx = 0; idx < (int)schedulePlan.size(); idx++) {
        for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
            auto &levels = facilities[tp];
            if ((int)levels.size() != schedulePlan[idx][tp].size()) {
                illegalArrangementsInfo.push_back("排班计划第[" + std::to_string(idx + 1) +
                                                  "]个班次排班表的设施[" + getFacilityTypeName(tp) +
                                                  "]数量与基建的该设施数量不匹配");
                break;
            }
            for (int room = 0; room < (int)levels.size(); room++) {
                int operatorLimit =
                    fitOperatorLimit(tp, levels[room], (int)schedulePlan[idx][tp][room].size());
                if (operatorLimit == false) {
                    illegalArrangementsInfo.push_back("排班计划第[" + std::to_string(idx + 1) +
                                                      "]个班次中，第[" + std::to_string(room + 1) +
                                                      "]个设施[" + getFacilityTypeName(tp) +
                                                      "]的入驻干员数量超过上限");
                }
            }
        }
    }
    return illegalArrangementsInfo.empty();
}