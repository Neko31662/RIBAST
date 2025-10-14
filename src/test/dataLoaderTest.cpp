#ifdef __WIN32
#include <cstdlib>
#endif

#include "../dataLoader/operatorData.h"
#include "../dataLoader/playerOperatorData.h"
#include "../dataLoader/skillData.h"

int main() {
#ifdef __WIN32
    system("chcp 65001");
#endif
    loadSkillData();
    loadOperatorData();
    loadPlayerOperatorData("data/干员练度表.xlsx");
    for (auto x : playerOperatorData) {
        auto op = x.second;
        std::cout << op.name << " " << op.rarity << "星 精" << op.phase << std::endl;
    }
    for (auto &name : unrecordedOperators) {
        std::cout << "未记录干员：" << name << std::endl;
    }
    return 0;
}