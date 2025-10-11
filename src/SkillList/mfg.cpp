#include "mfg.h"
#include "../globalParams.h"
#include "../operator.h"
#include "skillPriority.h"
#include "skillTemplates.h"
using std::invalid_argument;
using std::string;
using std::vector;

// 标准化：进驻制造站时，生产力+15%（alpha）或25%（beta）
auto skillFunc_biao_zhun_hua = [](GlobalParams &gp, Facility &facility, Operator &op,
                                  const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 15;
    } else if (level == "beta") {
        eff = 25;
    } else {
        throw invalid_argument("skillFunc_biao_zhun_hua函数：level参数错误");
    }

    facility.getSpec<Mfg::MfgSpec>()->skillCount_biao_zhun_hua++;
    add_efficiency(facility, op, eff);
};

// 公证所教习：进驻制造站时，作战记录类配方的生产力+20%（alpha）或30%（beta）
auto skillFunc_gong_zheng_suo_jiao_xi = [](GlobalParams &gp, Facility &facility, Operator &op,
                                           const string level) {
    if (facility.facilityType != MFG_RECORDS) {
        return;
    }

    int eff = 0;
    if (level == "alpha") {
        eff = 20;
    } else if (level == "beta") {
        eff = 30;
    } else {
        throw invalid_argument("skillFunc_gong_zheng_suo_jiao_xi函数：level参数错误");
    }

    add_efficiency(facility, op, eff);
};

// 机械精通：进驻制造站时，每有1台作业平台进驻发电站，贵金属类配方的生产力+5%（alpha）或10%（beta）
auto skillFunc_ji_xie_jing_tong = [](GlobalParams &gp, Facility &facility, Operator &op,
                                     const string level) {
    if (facility.facilityType != MFG_GOLD) {
        return;
    }
    auto ops = gp.power.operators;
    int platformCount = 0;
    for (auto &o : ops) {
        if (in_forces(*o, "作业平台")) {
            platformCount++;
        }
    }
    int rate = 0;
    if (level == "alpha") {
        rate = 5;
    } else if (level == "beta") {
        rate = 10;
    } else {
        throw invalid_argument("skillFunc_ji_xie_jing_tong函数：level参数错误");
    }
    int eff = rate * platformCount;
    add_efficiency_gold(facility, op, eff);
};

// 机械辅助：进驻制造站时，每16个（alpha）或8个（beta）工程机器人+5%生产力
auto skillFunc_ji_xie_fu_zhu = [](GlobalParams &gp, Facility &facility, Operator &op,
                                  const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int divisor = 100000;
    if (level == "alpha") {
        divisor = 16;
    } else if (level == "beta") {
        divisor = 8;
    } else {
        throw invalid_argument("skillFunc_ji_xie_fu_zhu函数：level参数错误");
    }

    int eff = 5 * (gp.spec.gong_cheng_ji_qi_ren / divisor);
    add_efficiency(facility, op, eff);
};

// 金属工艺：进驻制造站时，贵金属类配方的生产力+30%（alpha）或35%（beta）
auto skillFunc_jin_shu_gong_yi = [](GlobalParams &gp, Facility &facility, Operator &op,
                                    const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int eff = 0;
    if (level == "alpha") {
        eff = 30;
    } else if (level == "beta") {
        eff = 35;
    } else {
        throw invalid_argument("skillFunc_jin_shu_gong_yi函数：level参数错误");
    }
    facility.getSpec<Mfg::MfgSpec>()->skillCount_jin_shu_gong_yi++;
    add_efficiency_gold(facility, op, eff);
};

// 莱茵科技：进驻制造站时，生产力+15%（alpha）或25%（beta）或30%（gamma）
auto skillFunc_lai_yin_ke_ji = [](GlobalParams &gp, Facility &facility, Operator &op,
                                  const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int eff = 0;
    if (level == "alpha") {
        eff = 15;
    } else if (level == "beta") {
        eff = 25;
    } else if (level == "gamma") {
        eff = 30;
    } else {
        throw invalid_argument("skillFunc_lai_yin_ke_ji函数：level参数错误");
    }

    facility.getSpec<Mfg::MfgSpec>()->skillCount_lai_yin_ke_ji++;
    add_efficiency(facility, op, eff);
};

// 差遣使魔：进驻制造站时，生产力+20%（alpha）或30%（beta）
auto skillFunc_chai_qian_shi_mo = [](GlobalParams &gp, Facility &facility, Operator &op,
                                     const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int eff = 0;
    if (level == "alpha") {
        eff = 20;
    } else if (level == "beta") {
        eff = 30;
    } else {
        throw invalid_argument("skillFunc_chai_qian_shi_mo函数：level参数错误");
    }
    add_efficiency(facility, op, eff);
};

// 红松骑士团：进驻制造站时，生产力+15%（alpha）或25%（beta）
auto skillFunc_hong_song_qi_shi_tuan = [](GlobalParams &gp, Facility &facility, Operator &op,
                                          const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int eff = 0;
    if (level == "alpha") {
        eff = 15;
    } else if (level == "beta") {
        eff = 25;
    } else {
        throw invalid_argument("skillFunc_hong_song_qi_shi_tuan函数：level参数错误");
    }
    add_efficiency(facility, op, eff);
    facility.getSpec<Mfg::MfgSpec>()->skillCount_hong_song_qi_shi_tuan++;
};

// 自动化：进驻制造站时，当前制造站内其他干员提供的生产力全部归零（不包含根据设施数量提供加成的生产力），每个发电站为当前制造站+5%（alpha）或10%（beta）或15%（gamma，仿生海龙）的生产力
auto skillFunc_zi_dong_hua = [](GlobalParams &gp, Facility &facility, Operator &op,
                                const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }

    int rate = 0;
    if (level == "alpha") {
        rate = 5;
    } else if (level == "beta") {
        rate = 10;
    } else if (level == "gamma") {
        rate = 15;
    } else {
        throw invalid_argument("skillFunc_zi_dong_hua函数：level参数错误");
    }

    // 将当前制造站内其他干员提供的生产力归零
    for (auto &other_op : facility.operators) {
        if (other_op->name != op.name) {
            other_op->clearEfficiency();
        }
    }

    // 计算发电站数量
    int powerCount = gp.facilityCount[POWER] + gp.spec.power_count_by_influence;

    // 根据发电站数量增加生产力
    int eff = rate * powerCount;
    add_efficiency_by_facility(facility, op, eff);
};

// 仓库整备：进驻制造站时，仓库容量上限+6（alpha）或+10（beta），生产力+10%
auto skillFunc_cang_ku_zheng_bei = [](GlobalParams &gp, Facility &facility, Operator &op,
                                      const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int cap = 0;
    if (level == "alpha") {
        cap = 6;
    } else if (level == "beta") {
        cap = 10;
    } else {
        throw invalid_argument("skillFunc_cang_ku_zheng_bei函数：level参数错误");
    }
    add_capacity(facility, op, cap);
    add_efficiency(facility, op, 10);
};

// 工匠精神：进驻制造站时，生产力-5%，仓库容量上限+16（alpha）或+19（beta），心情每小时消耗-0.15（alpha）或-0.25（beta）
auto skillFunc_gong_jiang_jing_shen = [](GlobalParams &gp, Facility &facility, Operator &op,
                                         const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int cap = 0;
    int moodRate = 0;
    if (level == "alpha") {
        cap = 16;
        moodRate = 15;
    } else if (level == "beta") {
        cap = 19;
        moodRate = 25;
    } else {
        throw invalid_argument("skillFunc_gong_jiang_jing_shen函数：level参数错误");
    }
    reduce_mood_consumption_rate(facility, op, moodRate);
    add_capacity(facility, op, cap);
    reduce_efficiency(facility, op, 5);
};

// 剪辑：进驻制造站时，生产作战记录类配方时，仓库容量上限+12（alpha）或+15（beta）
auto skillFunc_jian_ji = [](GlobalParams &gp, Facility &facility, Operator &op,
                            const string level) {
    if (facility.facilityType != MFG_RECORDS) {
        return;
    }
    int cap = 0;
    if (level == "alpha") {
        cap = 12;
    } else if (level == "beta") {
        cap = 15;
    } else {
        throw invalid_argument("skillFunc_jian_ji函数：level参数错误");
    }
    add_capacity(facility, op, cap);
};

// 源石工艺：进驻制造站时，源石类配方的生产力+30%（alpha）或35%（beta）
// 与源石工艺·α等价的技能：地质学·α
// 与源石工艺·β等价的技能：地质学·β，火山学家，源石研究
auto skillFunc_yuan_shi_gong_yi = [](GlobalParams &gp, Facility &facility, Operator &op,
                                     const string level) {
    if (isMfg(facility.facilityType) == false) {
        return;
    }
    int eff = 0;
    if (level == "alpha") {
        eff = 30;
    } else if (level == "beta") {
        eff = 35;
    } else {
        throw invalid_argument("skillFunc_yuan_shi_gong_yi函数：level参数错误");
    }
    add_efficiency_originium(facility, op, eff);
};

void loadMfgSkillList(vector<Skill> &MfgSkillList) {
    // 标准化·α：进驻制造站时，生产力+15%
    MfgSkillList.push_back(
        Skill{NORMAL, "标准化·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_biao_zhun_hua(gp, facility, op, "alpha");
              }});

    // 标准化·β：进驻制造站时，生产力+25%
    MfgSkillList.push_back(
        Skill{NORMAL, "标准化·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_biao_zhun_hua(gp, facility, op, "beta");
              }});

    // 古老巫术：进驻制造站时，每5点人间烟火转化为1点巫术结晶
    MfgSkillList.push_back(
        Skill{WU_SHU_JIE_JING, "古老巫术", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  gp.spec.wu_shu_jie_jing += gp.spec.ren_jian_yan_huo / 5;
              }});

    // 戏中人：进驻制造站时，作战记录类配方的生产力+35%，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "戏中人", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency_records(facility, op, 35);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 镜中影：进驻制造站时，作战记录类配方的生产力+20%，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "镜中影", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency_records(facility, op, 20);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 实干的寡言者：进驻制造站时，生产力+20%，每有1瓶乌萨斯特饮，则仓库容量上限+2
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "实干的寡言者", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 20;
                  add_efficiency(facility, op, eff);
                  add_capacity(facility, op, 2 * gp.spec.wu_sa_si_te_yin);
              }});

    // 公证所教习·α：进驻制造站时，作战记录类配方的生产力+20%
    MfgSkillList.push_back(
        Skill{NORMAL, "公证所教习·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_gong_zheng_suo_jiao_xi(gp, facility, op, "alpha");
              }});

    // 公证所教习·β：进驻制造站时，作战记录类配方的生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "公证所教习·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_gong_zheng_suo_jiao_xi(gp, facility, op, "beta");
              }});

    // 逆境荣光：进驻制造站时，作战记录类配方的生产力+35%
    MfgSkillList.push_back(
        Skill{NORMAL, "逆境荣光", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  add_efficiency_records(facility, op, 35);
              }});

    // 拳术指导录像：进驻制造站时，作战记录类配方的生产力+35%
    MfgSkillList.push_back(
        Skill{NORMAL, "拳术指导录像", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  add_efficiency_records(facility, op, 35);
              }});

    // 作战指导录像：进驻制造站时，作战记录类配方的生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "作战指导录像", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  add_efficiency_records(facility, op, 30);
              }});

    // 原质塑金副产物：进驻制造站时，每个贸易站为当前制造站贵金属类配方的生产力+3%
    MfgSkillList.push_back(Skill{EFF_BY_FACILITY_COUNT, "原质塑金副产物",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {
                                     int tradeCount = gp.facilityCount[TRADE_ORUNDUM] +
                                                      gp.facilityCount[TRADE_LMD] +
                                                      gp.spec.trade_count_by_influence;
                                     int eff = 3 * tradeCount;
                                     if (facility.facilityType != MFG_GOLD) {
                                         return;
                                     }
                                     add_efficiency_by_facility(facility, op, eff);
                                 }});

    // 胜利之计：进驻制造站时，作战记录类配方的生产力+25%
    MfgSkillList.push_back(
        Skill{NORMAL, "胜利之计", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  add_efficiency_records(facility, op, 25);
              }});

    // 齐心沙盗：进驻制造站时，每间宿舍每级为贵金属类配方的生产力+1%
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "齐心沙盗", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_GOLD) {
                      return;
                  }
                  int dormCount = (int)(gp.dormitories.size());
                  int level = 0;
                  for (int i = 0; i < dormCount; i++) {
                      level += gp.dormitories[i].level;
                  }
                  int eff = level;
                  add_efficiency_gold(facility, op, eff);
              }});

    // 盛餐的回报：进驻制造站时，当与酒神在同一个制造站时，作战记录类配方的生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "盛餐的回报", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_RECORDS) {
                      return;
                  }
                  if (facility.hasOperator("酒神")) {
                      add_efficiency_records(facility, op, 30);
                  }
              }});

    // “搭把手！”：进驻制造站时，当与温米在同一个制造站时，贵金属类配方的生产力+15%
    MfgSkillList.push_back(
        Skill{NORMAL, "“搭把手！”", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_GOLD) {
                      return;
                  }
                  if (facility.hasOperator("温米")) {
                      add_efficiency_gold(facility, op, 15);
                  }
              }});

    // 机械精通·α：进驻制造站时，每有1台作业平台进驻发电站，贵金属类配方的生产力+5%
    MfgSkillList.push_back(
        Skill{NORMAL, "机械精通·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ji_xie_jing_tong(gp, facility, op, "alpha");
              }});

    // 机械精通·β：进驻制造站时，每有1台作业平台进驻发电站，贵金属类配方的生产力+10%
    MfgSkillList.push_back(
        Skill{NORMAL, "机械精通·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ji_xie_jing_tong(gp, facility, op, "beta");
              }});

    // 稻禾厚，顺秋收：进驻制造站时，每3点人间烟火+1%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "稻禾厚，顺秋收", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 1 * (gp.spec.ren_jian_yan_huo / 3);
                  add_efficiency(facility, op, eff);
              }});

    // 问枯荣：进驻制造站时，每1点巫术结晶+2%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "问枯荣", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 2 * gp.spec.wu_shu_jie_jing;
                  add_efficiency(facility, op, eff);
              }});

    // 逐水草：进驻制造站时，每1点巫术结晶+1%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "逐水草", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 1 * gp.spec.wu_shu_jie_jing;
                  add_efficiency(facility, op, eff);
              }});

    // 绘图设计：进驻制造站时，基建内除活动室外每间设施每级+1个工程机器人，上限64个
    MfgSkillList.push_back(Skill{GONG_CHENG_JI_QI_REN, "绘图设计",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {
                                     if (isMfg(facility.facilityType) == false) {
                                         return;
                                     }
                                     int level = 0;
                                     auto facilities = gp.getAllFacilities();
                                     for (auto &f : facilities) {
                                         level += f->level;
                                     }
                                     int robotCount = std::min(64, level);
                                     gp.spec.gong_cheng_ji_qi_ren += robotCount;
                                 }});

    // 机械辅助·α：进驻制造站时，每16个工程机器人+5%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "机械辅助·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ji_xie_fu_zhu(gp, facility, op, "alpha");
              }});

    // 机械辅助·β：进驻制造站时，每8个工程机器人+5%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "机械辅助·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_ji_xie_fu_zhu(gp, facility, op, "beta");
              }});

    // 打工心得：进驻制造站时，当前制造站内每个金属工艺类技能为自身+5%的生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "打工心得", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int skillCount = facility.getSpec<Mfg::MfgSpec>()->skillCount_jin_shu_gong_yi;
                  int eff = 5 * skillCount;
                  add_efficiency(facility, op, eff);
              }});

    // 金属工艺·α：进驻制造站时，贵金属类配方的生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "金属工艺·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_jin_shu_gong_yi(gp, facility, op, "alpha");
              }});

    // 金属工艺·β：进驻制造站时，贵金属类配方的生产力+35%
    MfgSkillList.push_back(
        Skill{NORMAL, "金属工艺·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_jin_shu_gong_yi(gp, facility, op, "beta");
              }});

    // 挑大梁：进驻制造站时，基建内（不包含副手及活动室使用者）每有1名黑钢国际干员（最多3名），贵金属类配方的生产力+2%，心情每小时消耗-0.15
    MfgSkillList.push_back(
        Skill{NORMAL, "挑大梁", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_GOLD) {
                      return;
                  }
                  int count = 0;
                  auto facilities = gp.getAllFacilities();
                  for (auto &f : facilities) {
                      for (auto &o : f->operators) {
                          if (in_forces(*o, "黑钢国际")) {
                              count++;
                              if (count >= 3) {
                                  break;
                              }
                          }
                      }
                  }
                  add_efficiency_gold(facility, op, 2 * std::min(3, count));
                  reduce_mood_consumption_rate(facility, op, 15);
              }});

    // 再生能源：进驻制造站时，每个贸易站为当前制造站贵金属类配方的生产力+20%
    MfgSkillList.push_back(Skill{
        EFF_BY_FACILITY_COUNT, "再生能源", [](GlobalParams &gp, Facility &facility, Operator &op) {
            if (facility.facilityType != MFG_GOLD) {
                return;
            }
            int tradeCount = gp.facilityCount[TRADE_ORUNDUM] + gp.facilityCount[TRADE_LMD] +
                             gp.spec.trade_count_by_influence;
            int eff = 20 * tradeCount;
            if (facility.facilityType != MFG_GOLD) {
                return;
            }
            add_efficiency_by_facility(facility, op, eff);
        }});

    // 例行清扫：进驻制造站后，生产力每小时+2%，最终达到+20%
    MfgSkillList.push_back(
        Skill{EFF_BY_DURATION, "例行清扫", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = std::min(2 * op.duration, 20);
                  add_efficiency(facility, op, eff);
              }});

    // 小奇思：进驻制造站时，贵金属类配方的生产力+25%，心情每小时消耗+0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "小奇思", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency_gold(facility, op, 25);
                  add_mood_consumption_rate(facility, op, 25);
              }});

    // 净味香氛：进驻制造站时，贵金属类配方的生产力+25%，心情每小时消耗+0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "净味香氛", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency_gold(facility, op, 25);
                  add_mood_consumption_rate(facility, op, 25);
              }});

    // 莱茵科技·α：进驻制造站时，生产力+15%
    MfgSkillList.push_back(
        Skill{NORMAL, "莱茵科技·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_lai_yin_ke_ji(gp, facility, op, "alpha");
              }});
    // 莱茵科技·β：进驻制造站时，生产力+25%
    MfgSkillList.push_back(
        Skill{NORMAL, "莱茵科技·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_lai_yin_ke_ji(gp, facility, op, "beta");
              }});

    // 莱茵科技·γ：进驻制造站时，生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "莱茵科技·γ", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_lai_yin_ke_ji(gp, facility, op, "gamma");
              }});

    // 行动派：进驻制造站时，生产力+20%，仓库容量上限-8
    MfgSkillList.push_back(
        Skill{NORMAL, "行动派", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 20);
                  reduce_capacity(facility, op, 8);
              }});

    // 得心应手：进驻制造站时，生产力+25%，仓库容量上限+6
    MfgSkillList.push_back(
        Skill{NORMAL, "得心应手", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 25);
                  add_capacity(facility, op, 6);
              }});

    // 掘进工程：进驻制造站时，仓库容量上限+10，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "掘进工程", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 10);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 量体裁衣：进驻制造站时，生产力+20%，心情每小时消耗+0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "量体裁衣", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 20);
                  add_mood_consumption_rate(facility, op, 25);
              }});

    // 超感：进驻制造站时，宿舍内每有1名干员则感知信息+1，同时每1点感知信息转化为1点思维链环
    MfgSkillList.push_back(
        Skill{GAN_ZHI_XIN_XI, "超感_1", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int dormCount = (int)(gp.dormitories.size());
                  int operatorCount = dormCount * 5; // 默认满人
                  gp.spec.gan_zhi_xin_xi += operatorCount;
              }});
    MfgSkillList.push_back(
        Skill{SI_WEI_LIAN_HUAN, "超感_2", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  gp.spec.si_wei_lian_huan += gp.spec.gan_zhi_xin_xi;
              }});

    // 念力：进驻制造站时，每2点思维链环+1%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "念力", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 1 * (gp.spec.si_wei_lian_huan / 2);
                  add_efficiency(facility, op, eff);
              }});

    // 意识实体：进驻制造站时，每1点思维链环+1%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "意识实体", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 1 * gp.spec.si_wei_lian_huan;
                  add_efficiency(facility, op, eff);
              }});

    // 配合意识：进驻制造站时，当前制造站内其他干员提供的每5%生产力（不包含根据设施数量提供加成的生产力），额外提供5%生产力，最多提供40%生产力
    MfgSkillList.push_back(
        Skill{PEI_HE_YI_SHI, "配合意识", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int bonus = 0;
                  auto ops = facility.operators;
                  for (auto &o : ops) {
                      if (o->name != op.name) {
                          bonus += o->getEfficiencyEnhance();
                          bonus -= o->getEfficiencyReduce();
                      }
                  }
                  bonus = (int)(bonus / 5) * 5; // 向下取整到5%的倍数
                  bonus = std::min(bonus, 40);
                  bonus = std::max(bonus, 0);
                  add_efficiency(facility, op, bonus);
              }});

    // 差遣使魔·α：进驻制造站时，生产力+20%
    MfgSkillList.push_back(
        Skill{NORMAL, "差遣使魔·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_chai_qian_shi_mo(gp, facility, op, "alpha");
              }});

    // 差遣使魔·β：进驻制造站时，生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "差遣使魔·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_chai_qian_shi_mo(gp, facility, op, "beta");
              }});

    // 意想不到的美味：进驻制造站时，每1点魔物料理+1%生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "意想不到的美味", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = 1 * gp.spec.mo_wu_liao_li;
                  add_efficiency(facility, op, eff);
              }});

    // 咪波·制造型：进驻制造站时，生产力+30%
    MfgSkillList.push_back(
        Skill{NORMAL, "咪波·制造型", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 30);
              }});

    // 红松骑士团·α：进驻制造站时，生产力+15%
    MfgSkillList.push_back(
        Skill{NORMAL, "红松骑士团·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_hong_song_qi_shi_tuan(gp, facility, op, "alpha");
              }});

    // 红松骑士团·β：进驻制造站时，生产力+25%
    MfgSkillList.push_back(
        Skill{NORMAL, "红松骑士团·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_hong_song_qi_shi_tuan(gp, facility, op, "beta");
              }});

    // 探险者：进驻制造站时，仓库容量上限+16，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "探险者", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 16);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 收纳达人：进驻制造站时，仓库容量上限+16，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "收纳达人", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 16);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 麻烦制造者：进驻制造站时，生产力+25%，仓库容量上限-12，心情每小时消耗+0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "麻烦制造者", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 25);
                  reduce_capacity(facility, op, 12);
                  add_mood_consumption_rate(facility, op, 25);
              }});

    // 急性子：进驻制造站后，生产力首小时+20%，此后每小时+1%，最终达到+25%
    MfgSkillList.push_back(
        Skill{EFF_BY_DURATION, "急性子", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = std::min(20 + 1 * op.duration, 25);
                  add_efficiency(facility, op, eff);
              }});

    // 延时摄影：进驻制造站后，生产力首小时+15%，此后每小时+2%，最终达到+25%
    MfgSkillList.push_back(
        Skill{EFF_BY_DURATION, "延时摄影", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = std::min(15 + 2 * op.duration, 25);
                  add_efficiency(facility, op, eff);
              }});

    // 慢性子：进驻制造站后，生产力首小时+15%，此后每小时+2%，最终达到+25%
    MfgSkillList.push_back(
        Skill{EFF_BY_DURATION, "慢性子", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = std::min(15 + 2 * op.duration, 25);
                  add_efficiency(facility, op, eff);
              }});

    // 磐蟹·豆豆：进驻制造站时，生产力+15%
    MfgSkillList.push_back(
        Skill{NORMAL, "磐蟹·豆豆", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 15);
              }});

    // 仿生海龙：进驻制造站时，当前制造站内其他干员提供的生产力全部归零（不包含根据设施数量提供加成的生产力），每个发电站为当前制造站+15%的生产力
    MfgSkillList.push_back(Skill{EFF_BY_FACILITY_COUNT, "仿生海龙",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {
                                     skillFunc_zi_dong_hua(gp, facility, op, "gamma");
                                 }});

    // 自动化·α：进驻制造站时，当前制造站内其他干员提供的生产力全部归零（不包含根据设施数量提供加成的生产力），每个发电站为当前制造站+5%的生产力
    MfgSkillList.push_back(Skill{EFF_BY_FACILITY_COUNT, "自动化·α",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {
                                     skillFunc_zi_dong_hua(gp, facility, op, "alpha");
                                 }});

    // 自动化·β：进驻制造站时，当前制造站内其他干员提供的生产力全部归零（不包含根据设施数量提供加成的生产力），每个发电站为当前制造站+10%的生产力
    MfgSkillList.push_back(Skill{EFF_BY_FACILITY_COUNT, "自动化·β",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {
                                     skillFunc_zi_dong_hua(gp, facility, op, "beta");
                                 }});

    // 源石技艺理论应用：进驻制造站时，当前制造站内每个莱茵科技类技能为自身+5%的生产力
    MfgSkillList.push_back(Skill{
        EFF_BY_GP, "源石技艺理论应用", [](GlobalParams &gp, Facility &facility, Operator &op) {
            if (isMfg(facility.facilityType) == false) {
                return;
            }
            int skillCount = facility.getSpec<Mfg::MfgSpec>()->skillCount_lai_yin_ke_ji;
            int eff = 5 * skillCount;
            add_efficiency(facility, op, eff);
        }});

    // 意识协议：进驻制造站时，当前制造站内每个标准化类技能为自身+5%的生产力
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "意识协议", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int skillCount = facility.getSpec<Mfg::MfgSpec>()->skillCount_biao_zhun_hua;
                  int eff = 5 * skillCount;
                  add_efficiency(facility, op, eff);
              }});

    // 可靠的随从们：进驻制造站时，仓库容量上限+8，生产力+5%，同时每有1个木天蓼，则生产力+1%
    MfgSkillList.push_back(
        Skill{EFF_BY_GP, "可靠的随从们", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 8);
                  int eff = 5 + 1 * gp.spec.mu_tian_liao;
                  add_efficiency(facility, op, eff);
              }});

    // 仓库整备·α：进驻制造站时，仓库容量上限+6，生产力+10%
    MfgSkillList.push_back(
        Skill{NORMAL, "仓库整备·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_cang_ku_zheng_bei(gp, facility, op, "alpha");
              }});

    // 仓库整备·β：进驻制造站时，仓库容量上限+10，生产力+10%
    MfgSkillList.push_back(
        Skill{NORMAL, "仓库整备·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_cang_ku_zheng_bei(gp, facility, op, "beta");
              }});

    // 重聚时光：进驻制造站时，当前制造站内每个A1小队干员为自身+10%的生产力
    MfgSkillList.push_back(
        Skill{NORMAL, "重聚时光", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  auto &ops = facility.operators;
                  int a1Count = opList_in_forces(ops, "A1小队");
                  int eff = 10 * a1Count;
                  add_efficiency(facility, op, eff);
              }});

    // 工匠精神·α：进驻制造站时，生产力-5%，仓库容量上限+16，心情每小时消耗-0.15
    MfgSkillList.push_back(
        Skill{NORMAL, "工匠精神·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_gong_jiang_jing_shen(gp, facility, op, "alpha");
              }});

    // 工匠精神·β：进驻制造站时，生产力-5%，仓库容量上限+19，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "工匠精神·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_gong_jiang_jing_shen(gp, facility, op, "beta");
              }});

    // 合理利用：进驻制造站时，生产作战记录类配方时，仓库容量上限+4
    MfgSkillList.push_back(
        Skill{NORMAL, "合理利用", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_RECORDS) {
                      return;
                  }
                  add_capacity(facility, op, 4);
              }});

    // “可靠”助手：进驻制造站时，生产力-20%，仓库容量上限+17，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "“可靠”助手", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  reduce_efficiency(facility, op, 20);
                  add_capacity(facility, op, 17);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 剪辑·α：进驻制造站时，生产作战记录类配方时，仓库容量上限+12
    MfgSkillList.push_back(
        Skill{NORMAL, "剪辑·α", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_jian_ji(gp, facility, op, "alpha");
              }});

    // 剪辑·β：进驻制造站时，生产作战记录类配方时，仓库容量上限+15
    MfgSkillList.push_back(
        Skill{NORMAL, "剪辑·β", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  skillFunc_jian_ji(gp, facility, op, "beta");
              }});

    // 模糊视线：进驻制造站时，生产力+30%，自身每有4点心情落差，生产力-5%
    // 替换为：生产力+25%
    MfgSkillList.push_back(
        Skill{XIN_QING_LUO_CHA, "模糊视线", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_efficiency(facility, op, 25);
              }});

    // 窗外雪啸：进驻制造站时，当自身心情落差大于12时，生产力+10%，仓库容量+6
    // 替换为：白板
    MfgSkillList.push_back(Skill{XIN_QING_LUO_CHA, "窗外雪啸",
                                 [](GlobalParams &gp, Facility &facility, Operator &op) {}});

    // 午休好去处：进驻制造站时，仓库容量上限+10，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "午休好去处", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 10);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 无畏豪情：进驻制造站时，仓库容量上限+10，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "无畏豪情", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 10);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 囤积者：进驻制造站时，仓库容量上限+10，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "囤积者", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 10);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 大就是好！：进驻制造站时，当前制造站内干员根据自身提升的仓库容量提供一定的生产力。提升16格以下的，每格仓库容量提供1%生产力；提升大于16格的，每格仓库容量提供3%生产力（无法与回收利用叠加，且优先生效）
    MfgSkillList.push_back(Skill{
        DA_JIU_SHI_HAO, "大就是好！", [](GlobalParams &gp, Facility &facility, Operator &op) {
            if (isMfg(facility.facilityType) == false) {
                return;
            }
            facility.getSpec<Mfg::MfgSpec>()->has_skill_da_jiu_shi_hao = true;
            facility.getSpec<Mfg::MfgSpec>()->has_skill_hui_shou_li_yong = false;
            int eff = 0;
            for (auto &o : facility.operators) {
                int addedCapacity = std::max(o->getCapacityEnhance() - o->getCapacityReduce(), 0);
                int eff = 0;
                if (addedCapacity <= 16) {
                    eff = 1 * addedCapacity;
                } else {
                    eff = 16 + 3 * (addedCapacity - 16);
                }
            }
            facility.getSpec<Mfg::MfgSpec>()->efficiency_by_hui_shou_li_yong = 0;
            facility.getSpec<Mfg::MfgSpec>()->efficiency_by_da_jiu_shi_hao = eff;
        }});

    // 智慧之境：进驻制造站时，仓库容量上限+8，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "智慧之境", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 8);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // “都想要”：进驻制造站时，仓库容量上限+8，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "“都想要”", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 8);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 磐蟹·阿盘：进驻制造站时，仓库容量上限+8，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "磐蟹·阿盘", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 8);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 拾荒者：进驻制造站时，仓库容量上限+8，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "拾荒者", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  add_capacity(facility, op, 8);
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // “等不及”：进驻制造站后，生产力首小时+20%，此后每小时+1%，最终达到+25%
    MfgSkillList.push_back(
        Skill{EFF_BY_DURATION, "等不及", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  int eff = std::min(20 + 1 * op.duration, 25);
                  add_efficiency(facility, op, eff);
              }});

    // 回收利用：进驻制造站时，当前制造站内干员提升的每格仓库容量，提供2%生产力
    MfgSkillList.push_back(Skill{
        HUI_SHOU_LI_YONG, "回收利用", [](GlobalParams &gp, Facility &facility, Operator &op) {
            if (isMfg(facility.facilityType) == false) {
                return;
            }
            facility.getSpec<Mfg::MfgSpec>()->has_skill_hui_shou_li_yong = 1;
            int eff = 0;
            for (auto &o : facility.operators) {
                int addedCapacity = std::max(o->getCapacityEnhance() - o->getCapacityReduce(), 0);
                int eff = 0;
                eff = 2 * addedCapacity;
            }
            facility.getSpec<Mfg::MfgSpec>()->efficiency_by_hui_shou_li_yong = eff;
        }});

    // Vlog：进驻制造站时，生产作战记录类配方时，心情每小时消耗-0.25
    MfgSkillList.push_back(
        Skill{NORMAL, "Vlog", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (facility.facilityType != MFG_RECORDS) {
                      return;
                  }
                  reduce_mood_consumption_rate(facility, op, 25);
              }});

    // 患难拍档：进驻制造站时，若古米在贸易站，则作战记录类配方的生产力+35%
    MfgSkillList.push_back(
        Skill{NORMAL, "患难拍档", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  auto facilities = gp.getAllFacilities();
                  for (auto &f : facilities) {
                      if (isTrade(f->facilityType)) {
                          if (f->hasOperator("古米")) {
                              add_efficiency_records(facility, op, 35);
                              return;
                          }
                      }
                  }
              }});

    // 源石工艺·α：进驻制造站时，源石类配方的生产力+30%
    // 与源石工艺·α等价的技能：地质学·α
    for (const string &s : {"源石工艺·α", "地质学·α"}) {
        MfgSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_yuan_shi_gong_yi(gp, facility, op, "alpha");
                  }});
    }

    // 源石工艺·β：进驻制造站时，源石类配方的生产力+35%
    // 与源石工艺·β等价的技能：地质学·β，火山学家，源石研究
    for (const string &s : {"源石工艺·β", "地质学·β", "火山学家", "源石研究"}) {
        MfgSkillList.push_back(
            Skill{NORMAL, s, [](GlobalParams &gp, Facility &facility, Operator &op) {
                      skillFunc_yuan_shi_gong_yi(gp, facility, op, "beta");
                  }});
    }

    // 春雷响，万物长：进驻制造站时，当前制造站内所有干员心情每小时消耗-0.1
    MfgSkillList.push_back(
        Skill{NORMAL, "春雷响，万物长", [](GlobalParams &gp, Facility &facility, Operator &op) {
                  if (isMfg(facility.facilityType) == false) {
                      return;
                  }
                  for (auto &o : facility.operators) {
                      reduce_mood_consumption_rate(facility, *o, 10);
                  }
              }});

    // 团队精神：进驻制造站时，消除当前制造站内所有干员自身心情消耗的影响
    MfgSkillList.push_back(Skill{
        TUAN_DUI_JING_SHEN, "团队精神", [](GlobalParams &gp, Facility &facility, Operator &op) {
            if (isMfg(facility.facilityType) == false) {
                return;
            }
            facility.getSpec<Mfg::MfgSpec>()->has_skill_tuan_dui_jing_shen = 1;
            for (auto &o : facility.operators) {
                o->clearMoodConsumption();
            }
        }});
}

#include <iostream>
int main() {
    vector<Skill> MfgSkillList;
    loadMfgSkillList(MfgSkillList);
    try {
        for (auto x : MfgSkillList) {
            GlobalParams gp;
            Mfg_Gold f1(3);
            Mfg_Originium f2(3);
            Mfg_Records f3(3);
            Operator op;
            x.apply(gp, f1, op);
            x.apply(gp, f2, op);
            x.apply(gp, f3, op);
        }
    } catch (const std::invalid_argument &e) {
        std::cout << "捕获到异常：" << e.what() << std::endl;
    }
    return 0;
}