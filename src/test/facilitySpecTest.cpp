#ifdef __WIN32
#include <cstdlib>
#endif

#include "globalParams.h"
#include <iostream>
using namespace std;

void demonstrateSpecUsage() {
    // 创建一个制造站
    Mfg_Gold goldMfg(3);

    // 访问制造站特有属性 - 目前为空结构体
    if (auto *goldSpec = goldMfg.getSpec<Mfg_Gold::Mfg_GoldSpec>()) {
        cout << "成功访问到赤金制造站的专属Spec" << endl;
        // 后续可以在这里添加具体的属性访问
    }

    // 也可以访问父类的Spec
    if (auto *mfgSpec = goldMfg.getSpec<Mfg::MfgSpec>()) {
        cout << "成功访问到制造站的通用Spec" << endl;
    }

    // 创建贸易站
    Trade_LMD lmdTrade(3);

    // 访问贸易站特有属性
    if (auto *tradeSpec = lmdTrade.getSpec<Trade::TradeSpec>()) {
        cout << "成功访问到贸易站的Spec" << endl;
    }

    // 创建其他设施
    Meeting meeting(3);
    Power power(3);
    Office office(3);
    Control control(5);

    // 访问各自的Spec
    if (auto *meetingSpec = meeting.getSpec<Meeting::MeetingSpec>()) {
        cout << "成功访问到会客室的Spec" << endl;
    }

    if (auto *powerSpec = power.getSpec<Power::PowerSpec>()) {
        cout << "成功访问到发电站的Spec" << endl;
    }

    if (auto *officeSpec = office.getSpec<Office::OfficeSpec>()) {
        cout << "成功访问到办公室的Spec" << endl;
    }

    if (auto *controlSpec = control.getSpec<Control::ControlSpec>()) {
        cout << "成功访问到控制中枢的Spec" << endl;
    }

    // 多态访问示例
    vector<unique_ptr<Facility>> facilities;
    facilities.push_back(make_unique<Mfg_Gold>(3));
    facilities.push_back(make_unique<Trade_LMD>(2));
    facilities.push_back(make_unique<Meeting>(3));

    for (auto &facility : facilities) {
        cout << "设施: " << facility->getName() << endl;

        // 根据类型访问不同的spec
        if (auto *mfgSpec = facility->getSpec<Mfg::MfgSpec>()) {
            cout << "  这是制造站类型的设施" << endl;
        } else if (auto *tradeSpec = facility->getSpec<Trade::TradeSpec>()) {
            cout << "  这是贸易站类型的设施" << endl;
        } else if (auto *meetingSpec = facility->getSpec<Meeting::MeetingSpec>()) {
            cout << "  这是会客室" << endl;
        }
    }
}

int main() {
#ifdef __WIN32
    system("chcp 65001");
#endif
    demonstrateSpecUsage();
    return 0;
}