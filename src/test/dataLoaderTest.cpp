#ifdef __WIN32
#include <cstdlib>
#endif

#include "../dataLoader/operatorData.h"
#include "../dataLoader/skillData.h"

int main() {
#ifdef __WIN32
    system("chcp 65001");
#endif
    loadSkillData();
    loadOperatorData();
    return 0;
}