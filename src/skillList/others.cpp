#include "others.h"

#include "globalParams.h"
#include "operator.h"
#include "skillPriority.h"
#include "skillTemplates.h"
#include "json/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
using json = nlohmann::json;
using std::invalid_argument;
using std::string;
using std::vector;

vector<string> otherFacilityTypes = {
    "会客室", "办公室", "加工站", "发电站", "宿舍", "控制中枢", "训练室",
};

auto skillFunc_trival = [](GlobalParams &gp, Facility &facility, Operator &op) {
    // 无任何作用的技能
};

void loadOthersSkillList(std::vector<Skill> &OthersSkillList) {

    string data_file_path = string(PROJECT_SOURCE_DIR) + "/data/facility-skills.json";
    std::ifstream fin(data_file_path);
    if (!fin) {
        throw std::runtime_error("loadOthersSkillList函数：无法打开 facility-skills.json 文件");
    }

    json j_operators;
    try {
        fin >> j_operators;
    } catch (json::parse_error &e) {
        fin.close();
        throw std::runtime_error("loadOthersSkillList函数：JSON 解析错误: " + string(e.what()));
    }
    fin.close();

    for (auto const &[facilityName, skills] : j_operators.items()) {
        if (std::find(otherFacilityTypes.begin(), otherFacilityTypes.end(), facilityName) ==
            otherFacilityTypes.end()) {
            continue;
        }
        for (auto const &skillName : skills) {
            OthersSkillList.push_back(Skill(TRIVAL, skillName.get<string>(), skillFunc_trival));
        }
    }
}