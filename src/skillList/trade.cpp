#include "trade.h"
#include "../globalParams.h"
#include "../operator.h"
#include "skillPriority.h"
#include "skillTemplates.h"
#include <stdexcept>
#include <string>
#include <vector>
using std::invalid_argument;
using std::string;
using std::vector;

// 订单分发：进驻贸易站时，订单获取效率+20%（alpha）或30%（beta）或35%（gamma）
// 与订单分发·α等价的技能：企鹅物流·α，峯驰物流
// 与订单分发·β等价的技能：企鹅物流·β，外贸决议·α
// 与订单分发·γ（实际并无该技能）等价的技能：名流欢会，物流专家
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
    } else if (level == "gamma") {
        eff = 35;
    } else {
        throw invalid_argument("skillFunc_ding_dan_fen_fa函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
};

// 对陆接洽代表：进驻贸易站时，订单获取效率+25%（alpha）或30%（beta）；当乌尔比安在基建内时（不包含副手及活动室使用者），订单获取效率额外+5%（alpha）或10%（beta）
auto skillFunc_dui_lu_jie_qia_dai_biao = [](GlobalParams &gp, Facility &facility, Operator &op,
                                            const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff1 = 0, eff2 = 0;
    if (level == "alpha") {
        eff1 = 25;
        eff2 = 5;
    } else if (level == "beta") {
        eff1 = 30;
        eff2 = 10;
    } else {
        throw invalid_argument("skillFunc_dui_lu_jie_qia_dai_biao函数：level参数错误");
    }

    add_efficiency(facility, op, eff1);
    vector<Operator *> allOps = gp.getAllOperators();
    for (const auto &o : allOps) {
        if (o->name == "乌尔比安") {
            add_efficiency(facility, op, eff2);
            break;
        }
    }
};

// 白手起家·α：进驻贸易站时，订单获取效率+25%；当伊内丝入驻工作场所时（不包含副手），订单获取效率额外+5%
// 白手起家·β：进驻贸易站时，订单获取效率+30%；当伊内丝、W入驻工作场所时（不包含副手），订单获取效率分别额外+5%
auto skillFunc_bai_shou_qi_jia = [](GlobalParams &gp, Facility &facility, Operator &op,
                                    const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff1 = 0, eff2 = 0;
    if (level == "alpha") {
        eff1 = 25;
        eff2 = 5;
    } else if (level == "beta") {
        eff1 = 30;
        eff2 = 5;
    } else {
        throw invalid_argument("skillFunc_bai_shou_qi_jia函数：level参数错误");
    }

    add_efficiency(facility, op, eff1);
    auto ops = facility.operators;
    for (auto &o : ops) {
        if (o->name == "伊内丝") {
            add_efficiency(facility, op, eff2);
            if (level != "beta") {
                break;
            }
        }
        if (o->name == "W" && level == "beta") {
            add_efficiency(facility, op, eff2);
        }
    }
};

// 醉翁之意：当与德克萨斯在同一个贸易站时，心情每小时消耗-0.1，订单上限+2（alpha）或+4（beta）
auto skillFunc_zui_weng_zhi_yi = [](GlobalParams &gp, Facility &facility, Operator &op,
                                    const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int cap = 0;
    if (level == "alpha") {
        cap = 2;
    } else if (level == "beta") {
        cap = 4;
    } else {
        throw invalid_argument("skillFunc_zui_weng_zhi_yi函数：level参数错误");
    }

    if (facility.hasOperator("德克萨斯")) {
        reduce_mood_consumption_rate(facility, op, 10);
        add_capacity(facility, op, cap);
    }
};

// 天道酬勤：进驻贸易站时，当前贸易站内干员提供的每5%订单获取效率，额外提供5%效率，最多提供25%（alpha）或35%（beta）效率
auto skillFunc_tian_dao_chou_qin = [](GlobalParams &gp, Facility &facility, Operator &op,
                                      const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }
    int max_eff = 0;
    if (level == "alpha") {
        max_eff = 25;
    } else if (level == "beta") {
        max_eff = 35;
    } else {
        throw invalid_argument("skillFunc_tian_dao_chou_qin函数：level参数错误");
    }

    auto &ops = facility.operators;
    int total_efficiency = 0;
    for (const auto &o : ops) {
        total_efficiency += o->getEfficiencyEnhance();
        total_efficiency -= o->getEfficiencyReduce();
    }
    if (total_efficiency <= 0) {
        return;
    }
    int eff = (total_efficiency / 5) * 5;
    eff = std::min(eff, max_eff);
    add_efficiency(facility, op, eff);
};

// 勤俭经营：进驻贸易站时，贸易站内除自身以外每名处于工作状态的干员+10%（alpha）或20%（beta）订单获取效率
// 拥有相同技能模板的技能：代为说项：+15%（alpha+）
auto skillFunc_qin_jian_jing_ying = [](GlobalParams &gp, Facility &facility, Operator &op,
                                       const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 10;
    } else if (level == "beta") {
        eff = 20;
    } else if (level == "alpha+") {
        eff = 15;
    } else {
        throw invalid_argument("skillFunc_qin_jian_jing_ying函数：level参数错误");
    }

    auto &ops = facility.operators;
    for (const auto &o : ops) {
        if (o->name != op.name) {
            add_efficiency(facility, op, eff);
        }
    }
};

// 供应管理（alpha）：进驻贸易站时，订单获取效率+25%，且订单上限+1
// 半身人公会代表（beta）：进驻贸易站时，订单获取效率+30%，且订单上限+1
// 与半身人公会代表等价的技能：大巴扎管理学，使命必达，少当家
auto skillFunc_gong_ying_guan_li = [](GlobalParams &gp, Facility &facility, Operator &op,
                                      const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 25;
    } else if (level == "beta") {
        eff = 30;
    } else {
        throw invalid_argument("skillFunc_gong_ying_guan_li函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
    add_capacity(facility, op, 1);
};

// 喀兰贸易：进驻贸易站时，订单获取效率+15%，且订单上限+2（alpha）或+4（beta）
auto skillFunc_ka_lan_mao_yi = [](GlobalParams &gp, Facility &facility, Operator &op,
                                  const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 15;
    int cap = 0;
    if (level == "alpha") {
        cap = 2;
    } else if (level == "beta") {
        cap = 4;
    } else {
        throw invalid_argument("skillFunc_ka_lan_mao_yi函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
    add_capacity(facility, op, cap);
};

// 订单管理：进驻贸易站时，订单获取效率+10%，且订单上限+2（alpha）或+4（beta）
auto skillFunc_ding_dan_guan_li = [](GlobalParams &gp, Facility &facility, Operator &op,
                                     const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 10;
    int cap = 0;
    if (level == "alpha") {
        cap = 2;
    } else if (level == "beta") {
        cap = 4;
    } else {
        throw invalid_argument("skillFunc_ding_dan_guan_li函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
    add_capacity(facility, op, cap);
};

// 物流规划：进驻贸易站时，订单获取效率+5%，每有4条（alpha）或2条（beta）赤金生产线，则当前贸易站订单获取效率额外+15%
auto skillFunc_wu_liu_gui_hua = [](GlobalParams &gp, Facility &facility, Operator &op,
                                   const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff1 = 5;
    int eff2 = 0;
    int count = gp.spec.extra_chi_jin_sheng_chan_xian + gp.facilityCount[MFG_GOLD];

    if (level == "alpha") {
        eff2 = (count / 4) * 15;
    } else if (level == "beta") {
        eff2 = (count / 2) * 15;
    } else {
        throw invalid_argument("skillFunc_wu_liu_gui_hua函数：level参数错误");
    }

    add_efficiency(facility, op, eff1 + eff2);
};

// 订单流可视化：进驻贸易站时，订单获取效率+5%，每有4条（alpha）或2条（beta）赤金生产线，则为当前贸易站额外提供2条赤金生产线
auto skillFunc_ding_dan_liu_ke_shi_hua = [](GlobalParams &gp, Facility &facility, Operator &op,
                                            const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 5;
    int count = gp.facilityCount[MFG_GOLD];
    int extra_lines = 0;

    if (level == "alpha") {
        extra_lines = (count / 4) * 2;
    } else if (level == "beta") {
        extra_lines = (count / 2) * 2;
    } else {
        throw invalid_argument("skillFunc_ding_dan_liu_ke_shi_hua函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
    gp.spec.extra_chi_jin_sheng_chan_xian += extra_lines;
};

// 多面逢源：进驻贸易站时，当前贸易站每级+1个订单上限
// 拥有相同技能模板的技能：钱不我待
auto skillFunc_duo_mian_feng_yuan = [](GlobalParams &gp, Facility &facility, Operator &op) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int level = facility.level;
    add_capacity(facility, op, level);
};

// 虔诚筹款：进驻贸易站时，每间宿舍每级+1%（alpha）或+2%（beta）获取效率
auto skillFunc_qian_cheng_chou_kuan = [](GlobalParams &gp, Facility &facility, Operator &op,
                                         const string level) {
    if (isTrade(facility.facilityType) == false) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 1;
    } else if (level == "beta") {
        eff = 2;
    } else {
        throw invalid_argument("skillFunc_qian_cheng_chou_kuan函数：level参数错误");
    }

    int total_level = 0;
    for (const auto &f : gp.dormitories) {
        total_level += f.level;
    }
    add_efficiency(facility, op, eff * total_level);
};

// 裁缝：进驻贸易站时，提升当前贸易站高品质贵金属订单的出现概率（工作时长影响概率），心情每小时消耗-0.25
// 对裁缝类技能的实现如下：
// 对一级贸易站，裁缝技能不产生影响（真有人会这么用吗）；
// 对二级贸易站，裁缝·α在3小时后对订单权重的影响值为{+0,+25,+25} 参考[3,4]；
// 对二级贸易站，裁缝·β在5小时后对订单权重的影响值为{+0,+50,+50} （猜的）；
// 对三级贸易站，裁缝·α在3小时后对订单权重的影响值为{+0,+10,+90} 参考[1,2]；
// 对三级贸易站，裁缝·β在5小时后对订单权重的影响值为{+0,+10,+490} 参考[1,2]。
// 存在多个裁缝技能时，对订单权重的影响值直接相加 参考[1~4]。
// 工作时长未达到峰值时，对订单权重的影响值为（原影响值 * max(工作时长,1) /
// 达到峰值所要求的工作时长） 参考[2]。 数据来源：
// [1]https://m.prts.wiki/w/%E7%BD%97%E5%BE%B7%E5%B2%9B%E5%9F%BA%E5%BB%BA/%E8%B4%B8%E6%98%93%E7%AB%99
// [2]https://www.bilibili.com/video/BV1UW4y1a78E
// [3]https://www.bilibili.com/video/BV1L34y1T7gw
// [4]https://www.bilibili.com/video/BV11nRqYxEBG
auto skillFunc_cai_feng = [](GlobalParams &gp, Facility &facility, Operator &op,
                             const string level) {
    if (level != "alpha" && level != "beta") {
        throw invalid_argument("skillFunc_cai_feng函数：level参数错误");
    }
    if (!isTrade(facility.facilityType)) {
        return;
    }
    reduce_mood_consumption_rate(facility, op, 25);
    if (facility.facilityType != TRADE_LMD) {
        return;
    }

    int facility_level = facility.level;
    if (facility_level <= 1) {
        return;
    }

    vector<double> influence = {0.0, 0.0, 0.0}; // 对订单权重的影响值
    if (facility_level == 2) {
        if (level == "alpha") {
            influence = {0.0, 25.0, 25.0};
        } else if (level == "beta") {
            influence = {0.0, 50.0, 50.0};
        }
    } else if (facility_level == 3) {
        if (level == "alpha") {
            influence = {0.0, 10.0, 90.0};
        } else if (level == "beta") {
            influence = {0.0, 10.0, 490.0};
        }
    }

    if (level == "alpha") {
        if (op.duration < 3) {
            int d = std::max(1, op.duration);
            for (int i = 0; i < 3; ++i) {
                influence[i] = influence[i] * d / 3.0;
            }
        }
    } else {
        if (op.duration < 5) {
            int d = std::max(1, op.duration);
            for (int i = 0; i < 3; ++i) {
                influence[i] = influence[i] * d / 5.0;
            }
        }
    }

    add_product_rate(facility, influence);
};

// 违约索赔：进驻贸易站时，如果下笔赤金订单是违约订单，则赤金交付数额外+1（alpha）或+2（beta）
auto skillFunc_wei_yue_suo_pei = [](GlobalParams &gp, Facility &facility, Operator &op,
                                    const string level) {
    if (facility.facilityType != TRADE_LMD) {
        return;
    }

    int extra = 0;
    if (level == "alpha") {
        extra = 1;
    } else if (level == "beta") {
        extra = 2;
    } else {
        throw invalid_argument("skillFunc_wei_yue_suo_pei函数：level参数错误");
    }

    Trade_LMD *tradeFacility = dynamic_cast<Trade_LMD *>(&facility);
    if (!tradeFacility) {
        throw std::runtime_error("skillFunc_wei_yue_suo_pei函数：facility无法转换为Trade_LMD");
    }

    vector<int> curProductNumber(3);
    for (int i = 0; i < 3; ++i) {
        curProductNumber[i] = tradeFacility->productNumber[i];
        if (curProductNumber[i] < 4) {
            curProductNumber[i] += extra;
        }
    }

    tradeFacility->changeProductNumber(curProductNumber);
};

// 投资：进驻贸易站后，如果下笔赤金订单交付数大于3（违约订单不视作赤金订单），则其龙门币收益+250（alpha）或+500（beta），心情每小时消耗-0.25
auto skillFunc_tou_zi = [](GlobalParams &gp, Facility &facility, Operator &op, const string level) {
    if (!isTrade(facility.facilityType)) {
        return;
    }
    reduce_mood_consumption_rate(facility, op, 25);
    if (facility.facilityType != TRADE_LMD) {
        return;
    }

    int extra = 0;
    if (level == "alpha") {
        extra = 250;
    } else if (level == "beta") {
        extra = 500;
    } else {
        throw invalid_argument("skillFunc_tou_zi函数：level参数错误");
    }

    Trade_LMD *tradeFacility = dynamic_cast<Trade_LMD *>(&facility);
    if (!tradeFacility) {
        throw std::runtime_error("skillFunc_tou_zi函数：facility无法转换为Trade_LMD");
    }

    if (tradeFacility->productNumber[2] > 3) {
        tradeFacility->changeProductLMD(2, extra + tradeFacility->productLMD[2]);
    }
};

void loadTradeSkillList(vector<Skill> &TradeSkillList) {
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

    // 订单分发·γ：进驻贸易站时，订单获取效率+35%
    // 与订单分发·γ（实际并无该技能）等价的技能：名流欢会，物流专家
    for (const string &s : {"名流欢会", "物流专家"}) {
        TradeSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_ding_dan_fen_fa(gp, facility, op, "gamma");
                  }});
    }

    // 外贸决议·β：进驻贸易站时，订单获取效率+30%；当前贸易站内存在格拉斯哥帮干员时，订单获取效率额外+10%
    TradeSkillList.push_back(
        Skill{NORMAL, "外贸决议·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 30);
                  auto ops = facility.operators;
                  for (auto &o : ops) {
                      if (in_forces(*o, "格拉斯哥帮")) {
                          add_efficiency(facility, op, 10);
                          break;
                      }
                  }
              }});

    // 对陆接洽代表·α：进驻贸易站时，订单获取效率+25%；当乌尔比安在基建内时（不包含副手及活动室使用者），订单获取效率额外+5%
    TradeSkillList.push_back(
        Skill{NORMAL, "对陆接洽代表·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_dui_lu_jie_qia_dai_biao(gp, facility, op, "alpha");
              }});

    // 对陆接洽代表·β：进驻贸易站时，订单获取效率+30%；当乌尔比安在基建内时（不包含副手及活动室使用者），订单获取效率额外+10%
    TradeSkillList.push_back(
        Skill{NORMAL, "对陆接洽代表·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_dui_lu_jie_qia_dai_biao(gp, facility, op, "beta");
              }});

    // 白手起家·α：进驻贸易站时，订单获取效率+25%；当伊内丝入驻工作场所时（不包含副手），订单获取效率额外+5%
    TradeSkillList.push_back(
        Skill{NORMAL, "白手起家·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_bai_shou_qi_jia(gp, facility, op, "alpha");
              }});

    // 白手起家·β：进驻贸易站时，订单获取效率+30%；当伊内丝、W入驻工作场所时（不包含副手），订单获取效率分别额外+5%
    TradeSkillList.push_back(
        Skill{NORMAL, "白手起家·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_bai_shou_qi_jia(gp, facility, op, "beta");
              }});

    // 招商引资：进驻贸易站时，当前贸易站内干员提升的每个订单上限，提供4%订单获取效率
    TradeSkillList.push_back(
        Skill{EFF_BY_FACILITY, "招商引资", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  auto &ops = facility.operators;
                  int total_increase = 0;
                  for (const auto &o : ops) {
                      total_increase += o->getCapacityEnhance();
                      total_increase -= o->getCapacityReduce();
                  }
                  if (total_increase <= 0) {
                      return;
                  }
                  int eff = total_increase * 4;
                  add_efficiency(facility, op, eff);
              }});

    // 醉翁之意·α：当与德克萨斯在同一个贸易站时，心情每小时消耗-0.1，订单上限+2
    TradeSkillList.push_back(
        Skill{NORMAL, "醉翁之意·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_zui_weng_zhi_yi(gp, facility, op, "alpha");
              }});

    // 醉翁之意·β：当与德克萨斯在同一个贸易站时，心情每小时消耗-0.1，订单上限+4
    TradeSkillList.push_back(
        Skill{NORMAL, "醉翁之意·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_zui_weng_zhi_yi(gp, facility, op, "beta");
              }});

    // 恩怨：当与拉普兰德在同一个贸易站时，心情每小时消耗+0.3，订单获取效率+65%
    TradeSkillList.push_back(
        Skill{NORMAL, "恩怨", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  if (facility.hasOperator("拉普兰德")) {
                      add_efficiency(facility, op, 65);
                      add_mood_consumption_rate(facility, op, 30);
                  }
              }});

    // 默契：当与能天使在同一个贸易站时，心情每小时消耗-0.3
    TradeSkillList.push_back(
        Skill{NORMAL, "默契", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  if (facility.hasOperator("能天使")) {
                      reduce_mood_consumption_rate(facility, op, 30);
                  }
              }});

    // 低语：进驻贸易站时，当前贸易站内其他干员提供的订单获取效率全部归零，且每人为自身+45%订单获取效率，同时全体心情每小时消耗+0.25
    TradeSkillList.push_back(
        Skill{DI_YU, "低语", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  auto &ops = facility.operators;
                  for (auto &o : ops) {
                      if (o->name != op.name) {
                          o->clearEfficiency();
                          add_mood_consumption_rate(facility, *o, 25);
                          add_efficiency(facility, op, 45);
                      }
                  }
                  add_mood_consumption_rate(facility, op, 25);
              }});

    // 天道酬勤·α：进驻贸易站时，当前贸易站内干员提供的每5%订单获取效率，额外提供5%效率，最多提供25%效率
    TradeSkillList.push_back(Skill{TIAN_DAO_CHOU_QIN, "天道酬勤·α",
                                   [](GlobalParams &gp, Facility &facility, Operator &op) {
                                       skillFunc_tian_dao_chou_qin(gp, facility, op, "alpha");
                                   }});

    // 天道酬勤·β：进驻贸易站时，当前贸易站内干员提供的每5%订单获取效率，额外提供5%效率，最多提供35%效率
    TradeSkillList.push_back(Skill{TIAN_DAO_CHOU_QIN, "天道酬勤·β",
                                   [](GlobalParams &gp, Facility &facility, Operator &op) {
                                       skillFunc_tian_dao_chou_qin(gp, facility, op, "beta");
                                   }});

    // 慧眼独到：进驻贸易站时，固定获取特别独占订单（不视作违约订单），且该类订单不受任何订单获取效率的影响
    TradeSkillList.push_back(
        Skill{HUI_YAN_DU_DAO, "慧眼独到", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  facility.getSpec<Trade_LMD::Trade_LMDSpec>()->has_skill_hui_yan_du_dao = true;
              }});

    // 相伴：进驻贸易站时，订单获取效率+20%；当与能天使在同一个贸易站时，订单获取效率额外+25%
    TradeSkillList.push_back(
        Skill{NORMAL, "相伴", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 20);
                  for (auto &o : facility.operators) {
                      if (in_forces(*o, "能天使")) {
                          add_efficiency(facility, op, 25);
                          break;
                      }
                  }
              }});

    // 同城加急单：进驻贸易站时，同个贸易站中每有1名拉特兰干员，当前贸易站订单获取效率+15%
    TradeSkillList.push_back(
        Skill{NORMAL, "同城加急单", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int count = 0;
                  for (auto &o : facility.operators) {
                      if (in_forces(*o, "拉特兰")) {
                          count++;
                      }
                  }
                  if (count > 0) {
                      add_efficiency(facility, op, count * 15);
                  }
              }});

    // 交际：进驻贸易站时，订单获取效率+30%，心情每小时消耗-0.25
    TradeSkillList.push_back(
        Skill{NORMAL, "交际", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 30);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 帮派指南针：进驻贸易站时，同个贸易站中每有1名格拉斯哥帮干员，当前贸易站订单获取效率+20%；当与推进之王在同一个贸易站时，订单获取效率额外+35%
    TradeSkillList.push_back(
        Skill{NORMAL, "帮派指南针", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  for (auto &o : facility.operators) {
                      if (in_forces(*o, "格拉斯哥帮")) {
                          add_efficiency(facility, op, 20);
                      }
                      if (o->name == "推进之王") {
                          add_efficiency(facility, op, 35);
                      }
                  }
              }});

    // 勤俭经营·α：进驻贸易站时，贸易站内除自身以外每名处于工作状态的干员+10%订单获取效率
    TradeSkillList.push_back(
        Skill{NORMAL, "勤俭经营·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_qin_jian_jing_ying(gp, facility, op, "alpha");
              }});

    // 勤俭经营·β：进驻贸易站时，贸易站内除自身以外每名处于工作状态的干员+20%订单获取效率
    TradeSkillList.push_back(
        Skill{NORMAL, "勤俭经营·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_qin_jian_jing_ying(gp, facility, op, "beta");
              }});

    // 代为说项：进驻贸易站时，贸易站内除自身以外每名处于工作状态的干员+15%订单获取效率
    TradeSkillList.push_back(
        Skill{NORMAL, "代为说项", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_qin_jian_jing_ying(gp, facility, op, "alpha+");
              }});

    // 万里传书：进驻贸易站时，贸易站内全体干员每小时心情消耗-0.1，每有10点人间烟火，则额外-0.02
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "万里传书", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  reduce_mood_consumption_rate(facility, op, 10);
                  int extra = gp.spec.ren_jian_yan_huo;
                  reduce_mood_consumption_rate(facility, op, extra * 2);
              }});

    // 跋山涉水：进驻贸易站时，贸易站内全体干员每小时心情消耗-0.1，每有10点人间烟火，则额外-0.01
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "跋山涉水", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  reduce_mood_consumption_rate(facility, op, 10);
                  int extra = gp.spec.ren_jian_yan_huo;
                  reduce_mood_consumption_rate(facility, op, extra);
              }});

    // 供应管理：进驻贸易站时，订单获取效率+25%，且订单上限+1
    TradeSkillList.push_back(
        Skill{NORMAL, "供应管理", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_gong_ying_guan_li(gp, facility, op, "alpha");
              }});

    // 半身人公会代表：进驻贸易站时，订单获取效率+30%，且订单上限+1
    // 与半身人公会代表等价的技能：大巴扎管理学，使命必达，少当家
    for (const string &s : {"半身人公会代表", "大巴扎管理学", "使命必达", "少当家"}) {
        TradeSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_gong_ying_guan_li(gp, facility, op, "beta");
                  }});
    }

    // 熟悉的味道：进驻贸易站时，每1点魔物料理+1%订单效率
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "熟悉的味道", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int eff = gp.spec.mo_wu_liao_li;
                  if (eff > 0) {
                      add_efficiency(facility, op, eff);
                  }
              }});

    // 喀兰之主：进驻贸易站时，订单获取效率+20%，且订单上限+4
    TradeSkillList.push_back(
        Skill{NORMAL, "喀兰之主", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 20);
                  add_capacity(facility, op, 4);
              }});

    // 冠军风采：进驻贸易站时，当前贸易站内干员提升的每5个订单上限，提供25%订单获取效率，最多提供100%效率
    TradeSkillList.push_back(
        Skill{EFF_BY_FACILITY, "冠军风采", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  auto &ops = facility.operators;
                  int total_increase = 0;
                  for (const auto &o : ops) {
                      total_increase += o->getCapacityEnhance();
                      total_increase -= o->getCapacityReduce();
                  }
                  if (total_increase <= 0) {
                      return;
                  }
                  int eff = (total_increase / 5) * 25;
                  eff = std::min(eff, 100);
                  add_efficiency(facility, op, eff);
              }});

    // 威压：进驻贸易站时，订单获取效率+20%，且订单上限-2（订单最少为1）
    TradeSkillList.push_back(
        Skill{NORMAL, "威压", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 20);
                  reduce_capacity(facility, op, 2);
              }});

    // 不怒自威：进驻贸易站时，订单获取效率+25%，且订单上限-6（订单最少为1）
    TradeSkillList.push_back(
        Skill{NORMAL, "不怒自威", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 25);
                  reduce_capacity(facility, op, 6);
              }});

    // 喀兰贸易·α：进驻贸易站时，订单获取效率+15%，且订单上限+2
    TradeSkillList.push_back(
        Skill{NORMAL, "喀兰贸易·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ka_lan_mao_yi(gp, facility, op, "alpha");
              }});

    // 喀兰贸易·β：进驻贸易站时，订单获取效率+15%，且订单上限+4
    TradeSkillList.push_back(
        Skill{NORMAL, "喀兰贸易·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ka_lan_mao_yi(gp, facility, op, "beta");
              }});

    // 天真的谈判者：进驻贸易站时，订单获取效率+10%，当前贸易站赤金订单的赤金交付数必定为2（不视作违约订单）
    TradeSkillList.push_back(
        Skill{TIAN_ZHEN_DE_TAN_PAN_ZHE, "天真的谈判者",
              [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 10);

                  Trade_LMD *tradeFacility = dynamic_cast<Trade_LMD *>(&facility);
                  if (!tradeFacility) {
                      throw std::runtime_error("天真的谈判者技能：facility无法转换为Trade_LMD");
                  }
                  tradeFacility->changeProductRate(vector<double>{100, 0, 0});
                  tradeFacility->changeProductNumber(vector<int>{2, 0, 0});
              }});

    // 暖场：进驻贸易站时，贸易站内干员心情每小时消耗-0.1
    TradeSkillList.push_back(
        Skill{NORMAL, "暖场", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  auto &ops = facility.operators;
                  for (auto &o : ops) {
                      reduce_mood_consumption_rate(facility, *o, 10);
                  }
              }});

    // 订单管理·α：进驻贸易站时，订单获取效率+10%，且订单上限+2
    TradeSkillList.push_back(
        Skill{NORMAL, "订单管理·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ding_dan_guan_li(gp, facility, op, "alpha");
              }});

    // 订单管理·β：进驻贸易站时，订单获取效率+10%，且订单上限+4
    TradeSkillList.push_back(
        Skill{NORMAL, "订单管理·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ding_dan_guan_li(gp, facility, op, "beta");
              }});

    // 可爱的艾露猫：进驻贸易站时，订单获取效率+5%，且订单上限+2，同时每有1个木天蓼，则订单获取效率+3%
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "可爱的艾露猫", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 5);
                  int eff = gp.spec.mu_tian_liao;
                  if (eff > 0) {
                      add_efficiency(facility, op, eff * 3);
                  }
                  add_capacity(facility, op, 2);
              }});

    // 精准排期：进驻贸易站时，订单获取效率+30%，制造站每有1类配方进行加工，则当前贸易站订单获取效率额外+2%
    TradeSkillList.push_back(
        Skill{NORMAL, "精准排期", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 30);
                  for (auto i : {MFG_GOLD, MFG_ORIGINIUM, MFG_RECORDS}) {
                      if (gp.facilityCount[i] > 0) {
                          add_efficiency(facility, op, 2);
                      }
                  }
              }});

    // 销路宣发：进驻贸易站时，每有1条赤金生产线，则当前贸易站订单获取效率+5%
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "销路宣发", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int count = gp.facilityCount[MFG_GOLD] + gp.spec.extra_chi_jin_sheng_chan_xian;
                  if (count > 0) {
                      add_efficiency(facility, op, count * 5);
                  }
              }});

    // 物流规划·α：进驻贸易站时，订单获取效率+5%，每有4条赤金生产线，则当前贸易站订单获取效率额外+15%
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "物流规划·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_wu_liu_gui_hua(gp, facility, op, "alpha");
              }});

    // 物流规划·β：进驻贸易站时，订单获取效率+5%，每有2条赤金生产线，则当前贸易站订单获取效率额外+15%
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "物流规划·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_wu_liu_gui_hua(gp, facility, op, "beta");
              }});

    // 新城贸易：进驻贸易站时，订单获取效率+25%，会客室每级额外提供5%获取效率，最多提供40%效率
    TradeSkillList.push_back(
        Skill{NORMAL, "新城贸易", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int lv = gp.reception.level;
                  int eff = std::min(lv * 5 + 25, 40);
                  add_efficiency(facility, op, eff);
              }});

    // 天生的顾问：进驻贸易站时，订单获取效率+15%，会客室每级额外提供5%获取效率，最多提供30%效率
    TradeSkillList.push_back(
        Skill{NORMAL, "天生的顾问", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int lv = gp.reception.level;
                  int eff = std::min(lv * 5 + 15, 30);
                  add_efficiency(facility, op, eff);
              }});

    // 际崖居民：进驻贸易站时，基建内（不包含副手及活动室使用者）每1名杜林族干员（最多4名）为当前贸易站提供1条赤金生产线
    TradeSkillList.push_back(
        Skill{NORMAL, "际崖居民", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int count = 0;
                  auto ops = gp.getAllOperators();
                  for (const auto &o : ops) {
                      if (in_forces(*o, "杜林族")) {
                          count++;
                          if (count >= 4) {
                              break;
                          }
                      }
                  }
                  gp.spec.extra_chi_jin_sheng_chan_xian += count;
              }});

    // 订单流可视化·α：进驻贸易站时，订单获取效率+5%，每有4条赤金生产线，则为当前贸易站额外提供2条赤金生产线
    TradeSkillList.push_back(Skill{DING_DAN_LIU_KE_SHI_HUA, "订单流可视化·α",
                                   [](GlobalParams &gp, Facility &facility, Operator &op) {
                                       skillFunc_ding_dan_liu_ke_shi_hua(gp, facility, op, "alpha");
                                   }});

    // 订单流可视化·β：进驻贸易站时，订单获取效率+5%，每有2条赤金生产线，则为当前贸易站额外提供2条赤金生产线
    TradeSkillList.push_back(Skill{DING_DAN_LIU_KE_SHI_HUA, "订单流可视化·β",
                                   [](GlobalParams &gp, Facility &facility, Operator &op) {
                                       skillFunc_ding_dan_liu_ke_shi_hua(gp, facility, op, "beta");
                                   }});

    // 多面逢源：进驻贸易站时，当前贸易站每级+1个订单上限
    // 拥有相同技能模板的技能：钱不我待
    for (const string &s : {"多面逢源", "钱不我待"}) {
        TradeSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_duo_mian_feng_yuan(gp, facility, op);
                  }});
    }

    // 摊贩经济：进驻贸易站时，当前订单数与订单上限每差1笔订单，则订单获取效率+4%
    // 实现过于复杂，用效率+（4% * 订单上限），清理基建1小时后-4%，此后交替-3%和2%替代
    // 如果同时拥有技能“市井之道”，则会清除该技能的效果，以正确的逻辑同时实现两个技能
    TradeSkillList.push_back(
        Skill{TAN_FAN_JING_JI, "摊贩经济", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int cap = facility.capacity;
                  for (const auto &o : facility.operators) {
                      cap += o->getCapacityEnhance();
                      cap -= o->getCapacityReduce();
                  }
                  cap = std::max(cap, 1);

                  int eff = cap * 4;
                  int hour = gp.clearDuration;
                  if (hour >= 1) {
                      eff -= 4;
                      hour--;
                  }
                  eff -= (hour / 2) * 5;
                  if (hour % 2 == 1) {
                      eff -= 3;
                  }
                  eff = std::max(eff, 0);
                  add_efficiency(facility, op, eff);
              }});

    // 市井之道：进驻贸易站时，当前贸易站内其他干员提供的每10%订单获取效率使订单上限-1（订单最少为1），同时每有1笔订单就+4%订单获取效率（与部分技能有特殊叠加规则）
    // 无法单独与天道酬勤·α、天道酬勤·β进行叠加，且优先生效
    // 当天道酬勤·α、天道酬勤·β与其他技能进行叠加时，该技能会对此叠加效果进行叠加
    TradeSkillList.push_back(
        Skill{SHI_JING_ZHI_DAO, "市井之道", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  op.clearEfficiency();
                  int eff = 0;
                  for (const auto &o : facility.operators) {
                      if (o->name != op.name) {
                          eff += o->getEfficiencyEnhance();
                          eff -= o->getEfficiencyReduce();
                      }
                  }
                  if (eff > 0) {
                      int reduce = eff / 10;
                      reduce_capacity(facility, op, reduce);
                  }
                  int cap = facility.capacity;
                  for (const auto &o : facility.operators) {
                      cap += o->getCapacityEnhance();
                      cap -= o->getCapacityReduce();
                  }
                  cap = std::max(cap, 1);
                  add_efficiency(facility, op, cap * 4);
              }});

    // 虔诚筹款·α：进驻贸易站时，每间宿舍每级+1%获取效率
    TradeSkillList.push_back(
        Skill{NORMAL, "虔诚筹款·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_qian_cheng_chou_kuan(gp, facility, op, "alpha");
              }});

    // 虔诚筹款·β：进驻贸易站时，每间宿舍每级+2%获取效率
    TradeSkillList.push_back(
        Skill{NORMAL, "虔诚筹款·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_qian_cheng_chou_kuan(gp, facility, op, "beta");
              }});

    // 乐感：进驻贸易站时，宿舍内每有1名干员则感知信息+1，同时每1点感知信息转化为1点无声共鸣
    TradeSkillList.push_back(
        Skill{GAN_ZHI_XIN_XI, "乐感_1", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int dormCount = (int)(gp.dormitories.size());
                  int operatorCount = dormCount * 5; // 默认满人
                  gp.spec.gan_zhi_xin_xi += operatorCount;
              }});
    TradeSkillList.push_back(
        Skill{WU_SHENG_GONG_MING, "乐感_2", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  gp.spec.wu_sheng_gong_ming += gp.spec.gan_zhi_xin_xi;
              }});

    // 怅惘和声：进驻贸易站时，每2点无声共鸣+1%订单效率
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "怅惘和声", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int eff = gp.spec.wu_sheng_gong_ming / 2;
                  if (eff > 0) {
                      add_efficiency(facility, op, eff);
                  }
              }});

    // 徘徊旋律：进驻贸易站时，每4点无声共鸣+1%订单效率
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "徘徊旋律", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int eff = gp.spec.wu_sheng_gong_ming / 4;
                  if (eff > 0) {
                      add_efficiency(facility, op, eff);
                  }
              }});

    // “愿者上钩”：进驻贸易站时，宿舍内每有1名干员则人间烟火+1，同时每有1点人间烟火，则订单获取效率+1%
    TradeSkillList.push_back(Skill{REN_JIAN_YAN_HUO, "“愿者上钩”_1",
                                   [](GlobalParams &gp, Facility &facility, Operator &op) {
                                       if (isTrade(facility.facilityType) == false) {
                                           return;
                                       }
                                       int dormCount = (int)(gp.dormitories.size());
                                       int operatorCount = dormCount * 5; // 默认满人
                                       gp.spec.ren_jian_yan_huo += operatorCount;
                                   }});
    TradeSkillList.push_back(
        Skill{EFF_BY_GP, "“愿者上钩”_2", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int eff = gp.spec.ren_jian_yan_huo;
                  if (eff > 0) {
                      add_efficiency(facility, op, eff);
                  }
              }});

    // 裁缝·α：进驻贸易站时，小幅提升当前贸易站高品质贵金属订单的出现概率（工作时长影响概率），心情每小时消耗-0.25
    // 与裁缝·α等价的技能：手工艺品·α，鉴定师的眼光，懂行
    for (const string &s : {"裁缝·α", "手工艺品·α", "鉴定师的眼光", "懂行"}) {
        TradeSkillList.push_back(
            Skill{ORDER_CHANGE, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_cai_feng(gp, facility, op, "alpha");
                  }});
    }

    // 裁缝·β：进驻贸易站时，提升当前贸易站高品质贵金属订单的出现概率（工作时长影响概率），心情每小时消耗-0.25
    // 与裁缝·β等价的技能：手工艺品·β，鉴定师的手段
    for (const string &s : {"裁缝·β", "手工艺品·β", "鉴定师的手段"}) {
        TradeSkillList.push_back(
            Skill{ORDER_CHANGE, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_cai_feng(gp, facility, op, "beta");
                  }});
    }

    // 合同法：进驻贸易站时，如果下笔赤金订单交付数小于4，则视为违约订单
    TradeSkillList.push_back(
        Skill{NORMAL, "合同法", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != TRADE_LMD) {
                      return;
                  }
                  facility.getSpec<Trade_LMD::Trade_LMDSpec>()->has_skill_he_tong_fa = true;
              }});

    // 违约索赔·α：进驻贸易站时，如果下笔赤金订单是违约订单，则赤金交付数额外+1
    TradeSkillList.push_back(
        Skill{ORDER_CHANGE, "违约索赔·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_wei_yue_suo_pei(gp, facility, op, "alpha");
              }});

    // 违约索赔·β：进驻贸易站时，如果下笔赤金订单是违约订单，则赤金交付数额外+2
    TradeSkillList.push_back(
        Skill{ORDER_CHANGE, "违约索赔·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_wei_yue_suo_pei(gp, facility, op, "beta");
              }});

    // 投资·α：进驻贸易站后，如果下笔赤金订单交付数大于3（违约订单不视作赤金订单），则其龙门币收益+250，心情每小时消耗-0.25
    TradeSkillList.push_back(
        Skill{ORDER_CHANGE, "投资·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_tou_zi(gp, facility, op, "alpha");
              }});

    // 投资·β：进驻贸易站后，如果下笔赤金订单交付数大于3（违约订单不视作赤金订单），则其龙门币收益+500，心情每小时消耗-0.25
    TradeSkillList.push_back(
        Skill{ORDER_CHANGE, "投资·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_tou_zi(gp, facility, op, "beta");
              }});

    // 谈判：进驻贸易站时，订单上限+5，心情每小时消耗-0.25
    TradeSkillList.push_back(
        Skill{NORMAL, "谈判", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 5);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 精英小队：进驻贸易站时，订单获取效率+25%，基建内（不包含副手及活动室）每有一间进驻精英干员的设施，订单获取效率额外+2%（最多10间）
    TradeSkillList.push_back(
        Skill{NORMAL, "精英小队", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isTrade(facility.facilityType) == false) {
                      return;
                  }
                  int eliteCount = 0;
                  for (auto &f : gp.getAllFacilities()) {
                      for (auto &ops : f->operators) {
                          if (in_forces(*ops, "精英干员")) {
                              eliteCount++;
                              break;
                          }
                      }
                  }
                  eliteCount = std::min(10, eliteCount);
                  int eff = eliteCount * 2 + 25;
                  add_efficiency(facility, op, eff);
              }});
}