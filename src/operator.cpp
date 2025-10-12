#include "operator.h"
using std::invalid_argument;
using std::string;
using std::vector;

bool inNameList(Operator &op, const vector<string> &nameList) {
    for (const auto &n : nameList) {
        if (op.name == n) {
            return true;
        }
    }
    return false;
}

// 势力-干员列表
std::map<string, vector<string>> forces_opList = {
    {"作业平台", {"Lancet-2", "Castle-3", "THRM-EX", "正义骑士号", "Friston-3", "PhonoR-0"}},
    {"黑钢国际", {"雷蛇", "芙兰卡", "杰西卡", "香草", "杏仁", "寻澜"}},
    {"A1小队", {"芙蓉", "炎熔", "米格鲁", "芬", "克洛丝"}},
    {"格拉斯哥帮", {"推进之王", "摩根", "达格达", "因陀罗"}},
    {"能天使", {"能天使", "新约能天使"}},
    {"拉特兰",
     {"翎羽", "安德切尔", "送葬人", "莫斯提马", "安比尔", "芳汀", "空弦", "菲亚梅塔", "见行者",
      "空构", "隐现", "塑心", "蕾缪安", "信仰搅拌机", "CONFESS-47"}},
    {"精英干员", {"迷迭香", "煌", "逻各斯", "烛煌", "电弧", "真言"}}};

bool in_forces(Operator &op, string forcesName) {
    if (forces_opList.find(forcesName) == forces_opList.end()) {
        throw invalid_argument("in_forces函数：不存在该势力名称");
    }
    return inNameList(op, forces_opList[forcesName]);
}

int opList_in_forces(const vector<Operator *> &opList, string forcesName) {
    if (forces_opList.find(forcesName) == forces_opList.end()) {
        throw invalid_argument("opList_in_forces函数：不存在该势力名称");
    }
    int count = 0;
    for (const auto &op : opList) {
        if (inNameList(*op, forces_opList[forcesName])) {
            count++;
        }
    }
    return count;
}