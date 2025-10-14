#include "operatorData.h"
#include "../operator.h"
#include "../tools/json/json.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using nlohmann::json;
using std::runtime_error;
using std::string;
using std::vector;

std::unordered_map<string, int> operatorNameToUid;
vector<OperatorInfo> uidToOperatorInfo;
vector<std::array<vector<int>, 3>> skillListByOperator;
bool hasLoadedOperatorData = false;

void loadOperatorData() {
    if (hasLoadedOperatorData) {
        return;
    }
    hasLoadedOperatorData = true;
    loadSkillData();

    operatorNameToUid.clear();
    uidToOperatorInfo.clear();
    skillListByOperator.clear();

    // 构建文件路径 - 使用字符串拼接替代filesystem
    string data_file_path = string(PROJECT_SOURCE_DIR) + "/data/operatorInfo.json";
    std::ifstream fin(data_file_path);
    if (!fin) {
        throw std::runtime_error("loadOperatorData函数：无法打开 operatorInfo.json 文件");
    }

    // 解析JSON文件内容
    json j_operators;
    try {
        fin >> j_operators;
    } catch (json::parse_error &e) {
        fin.close();
        throw std::runtime_error("loadOperatorData函数：JSON 解析错误: " + string(e.what()));
    }
    fin.close();

    // 遍历解析后的JSON对象
    // 遍历所有干员名称
    for (auto const &[opName, info] : j_operators.items()) {
        uidToOperatorInfo.emplace_back(opName, info["rarity"], info["profession"]);
        int uid_op = (int)uidToOperatorInfo.size() - 1;
        operatorNameToUid[opName] = uid_op;

        if (skillListByOperator.size() <= uid_op) {
            skillListByOperator.resize(uid_op + 100);
        }

        // 遍历该干员的所有技能
        for (auto const &[skillId, skillLevels] : info["skill"].items()) {

            vector<std::pair<int, string>> skillInfo;
            // 遍历同一技能ID下，该技能受精英化程度影响的所有等级/名称组合
            for (auto const &skillDetail : skillLevels) {
                int level = skillDetail["skillLevel"];
                string name = skillDetail["skillName"];

                skillInfo.push_back({level, name});
            }
            sort(skillInfo.begin(), skillInfo.end());

            // tmp[i]表示该技能在该干员精英化i时（i=0,1,2），技能的名称，未解锁则名称为空，精0
            // 30级视为精1
            string tmp[3] = {"", "", ""};
            for (int i = 0; i < (int)skillInfo.size(); i++) {
                int level = skillInfo[i].first;
                for (int j = level; j <= 2; j++) {
                    tmp[j] = skillInfo[i].second;
                }
            }

            // 按干员uid和精英化等级，插入技能uid
            for (int lv = 0; lv < 3; lv++) {
                if (tmp[lv] == "")
                    continue;
                auto puid_sk = skillNameToUid.find(tmp[lv]);
                if (puid_sk != skillNameToUid.end()) {
                    skillListByOperator[uid_op][lv].push_back(puid_sk->second);
                } else {
                    // 处理技能拆分的情况
                    auto it = skillSplitTable.find(tmp[lv]);
                    if (it != skillSplitTable.end()) {
                        for (const auto &subSkillName : it->second) {
                            auto puid_subsk = skillNameToUid.find(subSkillName);
                            if (puid_subsk != skillNameToUid.end()) {
                                skillListByOperator[uid_op][lv].push_back(puid_subsk->second);
                            } else {
                                throw runtime_error("skillSplitTable中含有未定义的技能：" +
                                                    subSkillName);
                            }
                        }
                    } else {
                        throw runtime_error("operatorInfo.json中含有未定义的技能：" + tmp[lv]);
                    }
                }
            }
        }
    }
}