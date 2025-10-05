#include <bits/stdc++.h>
using namespace std;

#include "../globalParams.h"

int main() {
    vector<unique_ptr<Facility>> Facilities_test;

    for (int lv = 1; lv <= 3; lv++) {
        Facilities_test.push_back(make_unique<Mfg_Gold>(lv));
        Facilities_test.push_back(make_unique<Mfg_Records>(lv));
        if (lv == 3)
            Facilities_test.push_back(make_unique<Mfg_Originium>(lv));
        if (lv == 3)
            Facilities_test.push_back(make_unique<Trade_Orundum>(lv));
        Facilities_test.push_back(make_unique<Trade_LMD>(lv));
    }
    Facilities_test.push_back(make_unique<Reception>(3));
    Facilities_test.push_back(make_unique<Power>(3));
    Facilities_test.push_back(make_unique<Office>(3));
    sort(Facilities_test.begin(), Facilities_test.end(), FacilityValueComparator{});
    for (const auto &facility : Facilities_test) {
        cout << facility->getName() << " (Level " << facility->level << "): \n";
        cout << "单个产品生产时间：" << facility->productTime << "分, \n";
        cout << "单个产品产出价值：" << facility->productValue << ", 相对效率为"
             << facility->productValue / facility->productTime << " \n";
        cout << "\n";
    }
}