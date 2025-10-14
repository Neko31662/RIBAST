#include "skillData.h"
#include <stdexcept>

using std::string;
using std::vector;

std::unordered_map<string, int> skillNameToUid;
vector<Skill> uidToSkill;
bool hasLoadedSkillData = false;

void loadSkillData() {
    if (hasLoadedSkillData) {
        return;
    }
    hasLoadedSkillData = true;

    uidToSkill.clear();
    skillNameToUid.clear();

    loadMfgSkillList(uidToSkill);
    loadTradeSkillList(uidToSkill);
    loadOthersSkillList(uidToSkill);

    for (int uid = 0; uid < (int)uidToSkill.size(); uid++) {
        const string &name = uidToSkill[uid].name;
        if (skillNameToUid.count(name)) {
            throw std::invalid_argument("loadSkillData函数：技能名称重复：" + name);
        }
        skillNameToUid[name] = uid;
    }
}
