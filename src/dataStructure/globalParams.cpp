#include "globalParams.h"
using std::invalid_argument;
using std::pair;
using std::string;
using std::vector;

void GlobalParams::reset(const FacilitiesInfo &facilityCounts) {
    spec = {};
    facilityCount.fill(0);
    cache_allFacilities_valid = false;
    cache_allOperators_valid = false;

    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        for (auto lv : facilityCounts[tp]) {
            facilityCount[tp]++;
            if (tp == CONTROL) {
                facilities[tp].push_back(std::make_shared<Control>(lv));
            } else if (tp == MEETING) {
                facilities[tp].push_back(std::make_shared<Meeting>(lv));
            } else if (tp == OFFICE) {
                facilities[tp].push_back(std::make_shared<Office>(lv));
            } else if (tp == TRAINING) {
                facilities[tp].push_back(std::make_shared<Training>(lv));
            } else if (tp == PROCESSING) {
                facilities[tp].push_back(std::make_shared<Processing>(lv));
            } else if (tp == DORMITORY) {
                facilities[tp].push_back(std::make_shared<Dormitory>(lv));
                if ((int)facilities[tp].size() > 4) {
                    throw invalid_argument("GlobalParams构造函数：宿舍数量不能超过4个");
                }
            } else if (tp == POWER) {
                if (lv != 3) {
                    throw invalid_argument("GlobalParams构造函数：发电站等级只能为3");
                }
            } else if (tp == MFG_GOLD) {
                facilities[tp].push_back(std::make_shared<Mfg_Gold>(lv));
            } else if (tp == MFG_RECORDS) {
                facilities[tp].push_back(std::make_shared<Mfg_Records>(lv));
            } else if (tp == MFG_ORIGINIUM) {
                facilities[tp].push_back(std::make_shared<Mfg_Originium>(lv));
            } else if (tp == TRADE_ORUNDUM) {
                facilities[tp].push_back(std::make_shared<Trade_Orundum>(lv));
            } else if (tp == TRADE_LMD) {
                facilities[tp].push_back(std::make_shared<Trade_LMD>(lv));
            } else {
                throw invalid_argument("GlobalParams构造函数：设施类型错误");
            }
        }
    }
    facilities[POWER].push_back(std::make_shared<Power>(facilityCount[POWER]));

    int totalCount = 0;
    for (int tp : {MFG_GOLD, MFG_RECORDS, MFG_ORIGINIUM, TRADE_ORUNDUM, TRADE_LMD, POWER}) {
        {
            totalCount += facilityCount[tp];
        }
        if (totalCount > 9) {
            throw invalid_argument("GlobalParams构造函数：制造站、贸易站、发电站"
                                   "设施数量之和不能超过9个");
        }
        for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
            if (isMfg(tp))
                continue;
            if (isTrade(tp))
                continue;
            if (tp == POWER)
                continue;
            if (tp == DORMITORY)
                continue;
            if (facilityCount[tp] > 1) {
                throw invalid_argument("GlobalParams构造函数：上限为1的设施类型数量不能超过1个");
            }
        }

        auto allFacilities = getAllFacilities();
        int powerConsumptionSum = 0;
        for (const auto &fac : allFacilities) {
            powerConsumptionSum += fac->powerConsumption;
        }
        if (powerConsumptionSum > 0) {
            throw invalid_argument("GlobalParams构造函数：设施总耗电量不能大于总发电量");
        }
    }
}

// 修改一个工位的干员，oldop为nullptr表示该工位修改前无人驻守，newUid为0表示该工位修改后无人驻守
void changeOneOperator(std::shared_ptr<Operator> &oldop, int newUid) {
    if (oldop == nullptr) {
        if (newUid == 0) {
            // do nothing
            return;
        } else {
            if (newUid >= 1 && newUid < (int)uidToOperatorInfo.size()) {
                // 玩家未拥有该干员，但在数据中存在该干员信息
                throw invalid_argument("changeOneOperator函数：未拥有该干员：" +
                                       uidToOperatorInfo[newUid].name);
            } else {
                throw invalid_argument("changeOneOperator函数：干员uid错误");
            }

            oldop = std::make_shared<Operator>(playerOperatorData[newUid]);
            oldop->duration = 0;
        }

    } else if (oldop != nullptr) {
        if (oldop->name == uidToOperatorInfo[newUid].name) {
            // do nothing
            return;
        } else if (newUid == 0) {
            oldop->duration = 0;
            oldop = nullptr;
        } else {
            if (newUid >= 1 && newUid < (int)uidToOperatorInfo.size()) {
                // 玩家未拥有该干员，但在数据中存在该干员信息
                throw invalid_argument("changeOneOperator函数：未拥有该干员：" +
                                       uidToOperatorInfo[newUid].name);
            } else {
                throw invalid_argument("changeOneOperator函数：干员uid错误");
            }

            oldop->duration = 0;
            oldop = std::make_shared<Operator>(playerOperatorData[newUid]);
            oldop->duration = 0;
        }
    }
}

void GlobalParams::arrangeOperators(const SingleSchedule &operatorArrangement) {
    cache_allOperators_valid = false;
    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        auto &fac = operatorArrangement[tp];
        if ((int)fac.size() != facilityCount[tp]) {
            throw invalid_argument("arrangeOperators函数：设施数量不匹配");
        }
        for (int idx = 0; idx < (int)fac.size(); idx++) {
            auto uidList = fac[idx];
            int operatorLimit = facilities[tp][idx]->operatorLimit;
            if ((int)uidList.size() > operatorLimit) {
                throw invalid_argument("arrangeOperators函数：设施入驻干员数量过多");
            }
            if ((int)facilities[tp][idx]->operators.size() != operatorLimit) {
                facilities[tp][idx]->operators.resize(operatorLimit);
            }
            for (int idx2 = 0; idx2 < operatorLimit; idx2++) {
                if (idx2 >= (int)uidList.size()) {
                    // 如果干员列表不够长，后面的工位视为无人驻守
                    changeOneOperator(facilities[tp][idx]->operators[idx2], 0);
                } else {
                    int uid = uidList[idx2];
                    changeOneOperator(facilities[tp][idx]->operators[idx2], uid);
                }
            }
        }
    }
}

vector<std::shared_ptr<Facility>> GlobalParams::getAllFacilities() {
    if (cache_allFacilities_valid) {
        return cache_allFacilities;
    }
    cache_allFacilities.clear();
    cache_allFacilities_valid = true;

    for (int tp = 1; tp <= FACILITY_TYPE_COUNT; tp++) {
        for (auto ptr : facilities[tp]) {
            cache_allFacilities.push_back(ptr);
        }
    }

    return cache_allFacilities;
}

vector<std::shared_ptr<Operator>> GlobalParams::getAllOperators() {
    if (cache_allOperators_valid) {
        return cache_allOperators;
    }
    cache_allOperators.clear();
    cache_allOperators_valid = true;

    auto facilities = getAllFacilities();
    for (const auto &fac : facilities) {
        for (const auto &op : fac->operators) {
            cache_allOperators.push_back(op);
        }
    }
    return cache_allOperators;
}