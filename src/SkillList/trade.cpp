#include <bits/stdc++.h>
using namespace std;

#include "../globalParams.h"
#include "../operator.h"
#include "mfg.h"
#include "skillPriority.h"
#include "skillTemplates.h"

vector<Skill> TradeSkillList;

// 订单分发：进驻贸易站时，订单获取效率+20%（alpha）或30%（beta）
// 与订单分发·α等价的技能：企鹅物流·α，峯驰物流
// 与订单分发·β等价的技能：企鹅物流·β，外贸决议·α
auto skillFunc_ding_dan_fen_fa = [](GlobalParams &gp, Facility &facility, Operator &op,
                                    const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 20;
    } else if (level == "beta") {
        eff = 30;
    } else {
        throw invalid_argument("skillFunc_ding_dan_fen_fa函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
};

void loadTradeSkillList() {
    // 订单分发·α：进驻贸易站时，订单获取效率+20%
    // 与订单分发·α等价的技能：企鹅物流·α，峯驰物流
    for (const string &s : {"订单分发·α", "企鹅物流·α", "峯驰物流"}) {
        TradeSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_ding_dan_fen_fa(gp, facility, op, "alpha");
                  }});
    }

    // 订单分发·β：进驻贸易站时，订单获取效率+30%
    // 与订单分发·β等价的技能：企鹅物流·β，外贸决议·α
    for (const string &s : {"订单分发·β", "企鹅物流·β", "外贸决议·α"}) {
        TradeSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_ding_dan_fen_fa(gp, facility, op, "beta");
                  }});
    }
}