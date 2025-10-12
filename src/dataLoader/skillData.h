#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../skill.h"
#include "../skillList/mfg.h"
#include "../skillList/others.h"
#include "../skillList/trade.h"

extern std::unordered_map<std::string, int> skillNameToUid;
extern std::vector<Skill> uidToSkill;

void loadSkillData(); // 加载技能数据
extern bool hasLoadedSkillData;