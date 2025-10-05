#include <bits/stdc++.h>
using namespace std;

#include "operator.h"

bool inNameList(Operator &op, const vector<string> &nameList) {
    for (const auto &n : nameList) {
        if (op.name == n) {
            return true;
        }
    }
    return false;
}

bool in_forces(Operator &op, string forcesName) {
    if (forces_opList.find(forcesName) == forces_opList.end()) {
        throw invalid_argument("in_forces函数：不存在该势力名称");
    }
    return inNameList(op, forces_opList[forcesName]);
}

int opList_in_forces(const vector<Operator*> &opList, string forcesName){
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