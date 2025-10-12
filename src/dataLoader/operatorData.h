#pragma once

#include "../operator.h"
#include "../skillList/skillSplitTable.h"
#include "skillData.h"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, int> operatorNameToUid;
extern std::vector<Operator> uidToOperator;

// 每个干员对应的技能列表，索引0,1,2分别对应精0时，精1（部分干员为精0 30级）时，精2时拥有的全部技能
// 例如：skillListByOperator[3][1][0]表示干员uid为3的干员精1时，拥有的首个技能的技能uid
extern std::vector<std::array<std::vector<int>, 3>> skillListByOperator;

void loadOperatorData(); // 加载干员数据
extern bool hasLoadedOperatorData;