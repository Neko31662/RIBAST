#include "playerOperatorData.h"

using std::string;
using std::vector;

std::map<int, Operator> playerOperatorData; // 玩家拥有的干员数据，key为干员uid
vector<string> unrecordedOperators;         // 未记录的干员名称

void loadPlayerOperatorData(const string &filePath) {
    loadOperatorData();

    string safePath = filePath;
#ifdef _WIN32
    SafeExcelPath sep(filePath);
    safePath = sep.getPath();
#endif
    auto table = readFirstSheet(safePath);

    // 遍历每行，跳过表头
    for (int i = 1; i < table.size(); i++) {
        auto &row = table[i];
        if (row[1] == "FALSE") {
            continue; // 未拥有该干员
        } else if (row[1] != "TRUE") {
            throw std::runtime_error("loadPlayerOperatorData函数：干员练度表第" +
                                     std::to_string(i + 1) + "行，第2列格式错误");
        }
        string opName = row[0];
        if (opName == "阿米娅（近卫）" || opName == "阿米娅（医疗）") {
            continue;
        }
        auto it = operatorNameToUid.find(opName);
        if (it == operatorNameToUid.end()) {
            unrecordedOperators.push_back(opName);
            continue; // 未在干员数据中找到该干员
        } else {
            int uid_op = it->second;

            // 测试稀有度是否匹配
            // int rarity1 = uidToOperatorInfo[uid_op].rarity;
            // int rarity2 = std::stoi(row[2]);
            // if (rarity1 != rarity2) {
            //     throw std::runtime_error("loadPlayerOperatorData函数：干员练度表第" +
            //                              std::to_string(i + 1) +
            //                              "行，第3列稀有度与干员数据不符");
            // }

            int rarity = uidToOperatorInfo[uid_op].rarity;
            int level = std::stoi(row[3]);
            int phase = std::stoi(row[4]);

            if (phase == 0 && rarity <= 2 && level >= 30) {
                phase = 1; // 1、2星干员精0 30级视为精英1
            }

            playerOperatorData[uid_op] = Operator(uidToOperatorInfo[uid_op], phase);
        }
    }
}
