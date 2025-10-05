#pragma once
#include <bits/stdc++.h>
using namespace std;

#include "operator.h"

#define MFG_GOLD 1      // 制造站-赤金
#define MFG_RECORDS 2   // 制造站-作战记录
#define MFG_ORIGINIUM 3 // 制造站-源石碎片
#define TRADE_ORUNDUM 4 // 贸易站-合成玉
#define TRADE_LMD 5     // 贸易站-龙门币
#define POWER 6         // 发电站
#define CONTROL 7       // 控制中枢
#define RECEPTION 8     // 会客室
#define OFFICE 9        // 办公室
#define DORMITORY 10    // 宿舍
#define PROCESSING 11   // 加工站
#define TRAINING 12     // 训练室

// 判断设施类型是否为制造站
inline bool isMfg(int facilityType) {
    return facilityType == MFG_GOLD || facilityType == MFG_RECORDS || facilityType == MFG_ORIGINIUM;
}

// 判断设施类型是否为贸易站
inline bool isTrade(int facilityType) {
    return facilityType == TRADE_ORUNDUM || facilityType == TRADE_LMD;
}

// 单个设施的基类
struct Facility {
    // 只与设施种类有关的参数

    // （可能）与设施等级有关的参数
    int level = 0;            // 设施等级
    double efficiency = 0.0;  // 效率，使用小数表示，不包括由干员基建技能提供的效率
    int capacity = 0;         // 容量
    int operatorLimit = 0;    // 干员数量上限
    int powerConsumption = 0; // 设施的电力消耗

    // （可能）与设施生产产品（和设施等级）有关的参数
    int facilityType = -1;       // 设施类型，在派生类构造时指定
    int productSpace = 1;        // 单个产品占用的容量
    int productTime = 1;         // 单个产品在效率1.0下的生产时间，单位分钟
    double productValue = 0.001; // 单个产品的基础价值
    // 赤金、中级作战记录、源石碎片、龙门币、合成玉产品的价值由三级设施生产一份产品所需的时间确定
    // 无人机的价值由无人机充能带来的基础耗时降低值确定（即3分钟）；
    // 基础作战记录、初级作战记录的价值根据其与中级作战记录的经验比例确定；
    // 线索的价值确定方式如下：由线索->信用->龙门币（50% off）确定每线索对应的龙门币价值；
    // 公招刷新次数的价值确定方式如下：公招刷新带来的黄票与绿票的增益期望->
    // (黄票->黄票对应的抽数（258黄票对38抽）->每抽对应的合成玉数量) +
    // (绿票->绿票在资质凭证区阶段3对应的合成玉) 确定每刷新次数对应的合成玉数量增益；
    // 单个合成玉的价值为生产对应的源石碎片+订单所需时间之和；

    vector<Operator *> operators; // 设施中驻守的干员列表
    int operatorDuration = 0;     // 干员进驻时长，以小时为单位，刚进驻为0小时

    // 设施特化属性的基类
    struct FacilitySpecBase {
        virtual ~FacilitySpecBase() = default;
        virtual unique_ptr<FacilitySpecBase> clone() const = 0;
    };
    unique_ptr<FacilitySpecBase> spec; // 设施特化属性

    // 返回该设施是否在工作，工作的定义为，该设施所制造的产品是否在生产
    virtual bool isWorking() const { return !operators.empty(); }

    // 获取特定类型的 spec，类型安全的访问方式
    template <typename T> T *getSpec() { return dynamic_cast<T *>(spec.get()); }
    template <typename T> const T *getSpec() const { return dynamic_cast<const T *>(spec.get()); }

    // 返回设施名称
    string getName() const {
        switch (facilityType) {
        case CONTROL:
            return "控制中枢";
        case MFG_GOLD:
            return "制造站-赤金";
        case MFG_RECORDS:
            return "制造站-作战记录";
        case MFG_ORIGINIUM:
            return "制造站-源石碎片";
        case TRADE_ORUNDUM:
            return "贸易站-合成玉";
        case TRADE_LMD:
            return "贸易站-龙门币";
        case RECEPTION:
            return "会客室";
        case POWER:
            return "发电站";
        case OFFICE:
            return "办公室";
        case DORMITORY:
            return "宿舍";
        default:
            return "其它设施";
        }
    }

    // 返回是否含有指定名称的干员
    bool hasOperator(string opName) const {
        for (const auto &op : operators) {
            if (op->name == opName) {
                return true;
            }
        }
        return false;
    }
};

// 制造站
struct Mfg : Facility {
    // 制造站的Spec
    struct MfgSpec : public FacilitySpecBase {
        int skillCount_jin_shu_gong_yi = 0;       // 金属工艺类技能个数
        int skillCount_lai_yin_ke_ji = 0;         // 莱茵科技类技能个数
        int skillCount_hong_song_qi_shi_tuan = 0; // 红松骑士团类技能个数
        int skillCount_biao_zhun_hua = 0;         // 标准化类技能个数

        bool has_skill_da_jiu_shi_hao = false;     // 是否含有“大就是好！”技能
        double efficiency_by_da_jiu_shi_hao = 0.0; // “大就是好！”技能提供的效率值

        bool has_skill_hui_shou_li_yong = false;   // 是否含有“回收利用”技能
        double efficiency_by_hui_shou_li_yong = 0.0; // “回收利用”技能提供的效率值

        bool has_skill_tuan_dui_jing_shen = false;   // 是否含有“团队精神”技能

        unique_ptr<FacilitySpecBase> clone() const override { return make_unique<MfgSpec>(*this); }
    };

    Mfg() {}
    Mfg(int lv) {
        efficiency = 1.0;
        level = lv;
        spec = make_unique<MfgSpec>();

        if (lv == 1) {
            capacity = 24;
            operatorLimit = 1;
            powerConsumption = 10;
        } else if (lv == 2) {
            capacity = 36;
            operatorLimit = 2;
            powerConsumption = 30;
        } else if (lv == 3) {
            capacity = 54;
            operatorLimit = 3;
            powerConsumption = 60;
        } else {
            throw invalid_argument("Mfg构造函数：制造站等级只能为1,2,3");
        }
    }
};

// 制造站-赤金
struct Mfg_Gold : Mfg {
    // 赤金制造站的Spec
    struct Mfg_GoldSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<Mfg_GoldSpec>(*this);
        }
    };

    Mfg_Gold() {}
    Mfg_Gold(int lv) : Mfg(lv) {
        facilityType = MFG_GOLD;
        productSpace = 2;
        productTime = 1 * 60 + 12;
        productValue = productTime;
        spec = make_unique<Mfg_GoldSpec>();
    }
};

// 制造站-作战记录
struct Mfg_Records : Mfg {
    // 作战记录制造站的Spec
    struct Mfg_RecordsSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<Mfg_RecordsSpec>(*this);
        }
    };

    Mfg_Records() {}
    Mfg_Records(int lv) : Mfg(lv) {
        facilityType = MFG_RECORDS;
        if (lv == 1) {
            // 生产基础作战记录
            productSpace = 2;
            productTime = 45;
            productValue = 3.0 * 60 * (200.0 / 1000);
        } else if (lv == 2) {
            // 生产初级作战记录
            productSpace = 3;
            productTime = 1 * 60 + 20;
            productValue = 3.0 * 60 * (400.0 / 1000);
        } else if (lv == 3) {
            // 生产中级作战记录
            productSpace = 5;
            productTime = 3 * 60;
            productValue = productTime;
        }
        spec = make_unique<Mfg_RecordsSpec>();
    }
};

// 制造站-源石碎片
struct Mfg_Originium : Mfg {
    // 源石碎片制造站的Spec
    struct Mfg_OriginiumSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<Mfg_OriginiumSpec>(*this);
        }
    };

    Mfg_Originium() {}
    Mfg_Originium(int lv) : Mfg(lv) {
        if (lv != 3) {
            throw invalid_argument("Mfg_Originium构造函数：源石碎片制造站等级只能为3");
        }
        facilityType = MFG_ORIGINIUM;
        productSpace = 3;
        productTime = 1 * 60;
        productValue = productTime;
        spec = make_unique<Mfg_OriginiumSpec>();
    }
};

// 贸易站
struct Trade : Facility {
    // 贸易站的Spec
    struct TradeSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<TradeSpec>(*this);
        }
    };

    Trade() {}
    Trade(int lv) {
        efficiency = 1.0;
        level = lv;
        spec = make_unique<TradeSpec>();

        if (lv == 1) {
            capacity = 6;
            operatorLimit = 1;
            powerConsumption = 10;
        } else if (lv == 2) {
            capacity = 8;
            operatorLimit = 2;
            powerConsumption = 30;
        } else if (lv == 3) {
            capacity = 10;
            operatorLimit = 3;
            powerConsumption = 60;
        } else {
            throw invalid_argument("Trade构造函数：贸易站等级只能为1,2,3");
        }
    }
};

// 贸易站-合成玉
struct Trade_Orundum : Trade {
    // 合成玉贸易站的Spec
    struct Trade_OrundumSpec : public TradeSpec {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<Trade_OrundumSpec>(*this);
        }
    };

    Trade_Orundum() {}
    Trade_Orundum(int lv) : Trade(lv) {
        if (lv != 3) {
            throw invalid_argument("Trade_Orundum构造函数：合成玉贸易站等级只能为3");
        }
        facilityType = TRADE_ORUNDUM;
        productSpace = 1;
        productTime = 2 * 60;
        productValue = productTime;
        spec = make_unique<Trade_OrundumSpec>();
    }
};

// 贸易站-龙门币
struct Trade_LMD : Trade {
    // 龙门币贸易站的Spec
    struct Trade_LMDSpec : public TradeSpec {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<Trade_LMDSpec>(*this);
        }
    };

    int productTimeList[3]; // 分别对应2,3,4赤金订单的生产时间
    int productTimeRate[3]; // 分别对应2,3,4赤金订单的生产概率
    double
        productValue_perGold; // 一份赤金对应的订单价值（即无加成3级贸易站下售卖一份赤金的期望时间）

    Trade_LMD() {}
    Trade_LMD(int lv) : Trade(lv) {
        facilityType = TRADE_LMD;
        productSpace = 1;

        productTimeList[0] = 2 * 60 + 24;
        productTimeList[1] = 3 * 60 + 30;
        productTimeList[2] = 4 * 60 + 36;
        // 2,3,4赤金订单生产概率数据来源：https://m.prts.wiki/w/%E7%BD%97%E5%BE%B7%E5%B2%9B%E5%9F%BA%E5%BB%BA/%E8%B4%B8%E6%98%93%E7%AB%99
        if (lv == 1) {
            productTimeRate[0] = 100;
            productTimeRate[1] = 0;
            productTimeRate[2] = 0;
        } else if (lv == 2) {
            productTimeRate[0] = 60;
            productTimeRate[1] = 40;
            productTimeRate[2] = 0;
        } else if (lv == 3) {
            productTimeRate[0] = 30;
            productTimeRate[1] = 50;
            productTimeRate[2] = 20;
        }

        productTime = 0;
        for (int i = 0; i < 3; i++) {
            productTime += productTimeList[i] * productTimeRate[i];
        }
        productTime /= 100;

        // 3级贸易站每份订单平均生产时间
        double productTime_lv3 =
            (30.0 * productTimeList[0] + 50.0 * productTimeList[1] + 20.0 * productTimeList[2]) /
            100;
        // 3级贸易站每份订单平均产出赤金数量
        double averageGoldCount_lv3 = (2 * 30.0 + 3 * 50.0 + 4 * 20.0) / 100;
        productValue_perGold = productTime_lv3 / averageGoldCount_lv3;
        double averageGoldCount =
            (productTimeRate[0] * 2 + productTimeRate[1] * 3 + productTimeRate[2] * 4) / 100.0;
        productValue = productValue_perGold * averageGoldCount;
        spec = make_unique<Trade_LMDSpec>();
    }
};

// 会客室（获取线索）
struct Reception : Facility {
    // 会客室的Spec
    struct ReceptionSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<ReceptionSpec>(*this);
        }
    };

    Reception() {}
    Reception(int lv) {
        level = lv;
        spec = make_unique<ReceptionSpec>();

        // 效率计算参考：https://m.prts.wiki/w/%E7%BD%97%E5%BE%B7%E5%B2%9B%E5%9F%BA%E5%BB%BA/%E4%BC%9A%E5%AE%A2%E5%AE%A4
        efficiency = 1.0 + 0.15 + 0.05; // 基础效率 + 宿舍氛围值加成 + 进驻干员非涣散加成
        // 会客室等级加成
        if (lv == 1) {
            efficiency += 0.07;
        } else if (lv == 2) {
            efficiency += 0.09;
        } else if (lv == 3) {
            efficiency += 0.11;
        } else {
            throw invalid_argument("Reception构造函数：会客室等级只能为1,2,3");
        }

        // 考虑到大多数情况下线索都是缺一张或两张，其他线索还可能重复，这里把线索容量设为3
        capacity = 3;

        operatorLimit = 2;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 30;
        } else if (lv == 3) {
            powerConsumption = 60;
        }

        facilityType = RECEPTION;
        productSpace = 1;
        productTime = 20 * 60;

        // 假设1个线索收益为45信用（直接使用为30，送人为两方带来收益分别为20、30+(0~15)）
        productValue = 45.0;
        // 1信用对应18龙门币（50% off）
        productValue *= 18.0;
        // 500龙门币对应1赤金
        productValue /= 500.0;
        // 生成一个3级龙门币贸易站，用于确定单个赤金的订单价值（好浪费啊草）
        auto tmp = Trade_LMD(3);
        double goldValue = tmp.productValue_perGold;
        productValue *= goldValue;
    }
};

// 发电站（无人机自动回复视为发电站的产出，所有发电站默认为满级，且视为一个设施，该结构体的level值代表发电站数量）
struct Power : Facility {
    // 发电站的Spec
    struct PowerSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<PowerSpec>(*this);
        }
    };

    Power() {}
    Power(int number) {
        if (number <= 0) {
            throw invalid_argument("Power构造函数：发电站数量不能为0或负数");
        }
        level = number;
        spec = make_unique<PowerSpec>();
        efficiency = 1.0;
        capacity = 235; // 后面或许会改成可变值
        operatorLimit = number;
        powerConsumption = -270 * number;
        facilityType = POWER;
        productSpace = 1;
        productTime = 6;
        productValue = 3.0;
    }

    // 无干员驻守时无人机也会充能，因此发电站在无人时也视为工作
    bool isWorking() const override { return true; }
};

// 办公室（获取公招刷新次数）
struct Office : Facility {
    // 办公室的Spec
    struct OfficeSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<OfficeSpec>(*this);
        }
    };

    int recruitSlots; // 招募位个数

    Office() {}
    Office(int lv) {
        level = lv;
        spec = make_unique<OfficeSpec>();
        efficiency = 1.05; // 只要有干员进驻就获得5%的效率加成
        capacity = 3;
        operatorLimit = 1;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 30;
        } else if (lv == 3) {
            powerConsumption = 60;
        } else {
            throw invalid_argument("Office构造函数：办公室等级只能为1,2,3");
        }

        facilityType = OFFICE;
        productSpace = 1;
        productTime = 12 * 60;

        // 计算一次刷新的价值
        // 公招出3~6星的概率，数据来源：https://ark.yituliu.cn/survey/maarecruitdata（截止2025.10.01）
        double probability[4] = {0.8394, 0.1530, 0.0055, 0.0021};
        // 每个星级对应的绿票和黄票收益，3、4星为满潜，5、6星为未满潜
        pair<int, int> benefit[4] = {{10, 0}, {30, 1}, {0, 5}, {0, 10}};
        // 生成一个3级合成玉贸易站，和一个3级源石碎片制造站，用于确定合成玉的价值
        auto tmp1 = Trade_Orundum(3);
        auto tmp2 = Mfg_Originium(3);
        double orundumValue = tmp1.productValue / 20 + tmp2.productValue / 10;

        // 计算一张绿票价值
        double greenTicketValue = 1.0;
        greenTicketValue *= 30.0 / 50.0; // 50绿票对应30合成玉
        greenTicketValue *= orundumValue;

        // 计算一张黄票价值
        double yellowTicketValue = 1.0;
        yellowTicketValue *= 38.0 / 258.0;       // 258黄票对应38抽
        yellowTicketValue *= 600 * orundumValue; // 1抽对应600合成玉

        // 计算：刷新后公招收益期望 - 刷新前（3星）公招收益
        pair<double, double> expectedBenefit = {0.0, 0.0};
        for (int i = 0; i < 4; i++) {
            expectedBenefit.first += probability[i] * benefit[i].first;
            expectedBenefit.second += probability[i] * benefit[i].second;
        }
        expectedBenefit.first -= benefit[0].first;
        expectedBenefit.second -= benefit[0].second;

        productValue =
            expectedBenefit.first * greenTicketValue + expectedBenefit.second * yellowTicketValue;
    }
};

// 控制中枢
struct Control : Facility {
    // 控制中枢的Spec
    struct ControlSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        unique_ptr<FacilitySpecBase> clone() const override {
            return make_unique<ControlSpec>(*this);
        }
    };

    Control() {}
    Control(int lv) {
        if (lv <= 0 || lv > 5) {
            throw invalid_argument("Control构造函数：控制中枢等级只能为1~5");
        }
        level = lv;
        spec = make_unique<ControlSpec>();
        operatorLimit = lv;
        powerConsumption = 0;
        facilityType = CONTROL;
    }
};

// 宿舍
struct Dormitory : Facility {
    double basicRecoverySpeed; // 基础恢复速率
    int atmosphere = 5000;     // 氛围值

    double getRecoverySpeed() const {
        double speed = basicRecoverySpeed;
        speed += min(atmosphere, 1000 * level) * 0.0004;
        return speed;
    }

    Dormitory() {}
    Dormitory(int lv) {
        level = lv;
        efficiency = 0.0;
        capacity = 0; // 宿舍不生产产品，因此容量设为0
        operatorLimit = lv;
        basicRecoverySpeed = 1.5 + 0.1 * lv;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 20;
        } else if (lv == 3) {
            powerConsumption = 30;
        } else if (lv == 4) {
            powerConsumption = 45;
        } else if (lv == 5) {
            powerConsumption = 65;
        } else {
            throw invalid_argument("Dormitory构造函数：宿舍等级只能为1~5");
        }

        facilityType = DORMITORY;
    }

    bool isWorking() const override { return false; }
};

// 加工站
struct Processing : Facility {
    Processing() {}
    Processing(int lv) {
        level = lv;
        operatorLimit = 1;

        if (lv == 1 || lv == 2 || lv == 3) {
            powerConsumption = 10;
        } else {
            throw invalid_argument("Processing构造函数：加工站等级只能为1,2,3");
        }

        facilityType = PROCESSING;
    }
};

// 训练室
struct Training : Facility {
    Training() {}
    Training(int lv) {
        level = lv;
        operatorLimit = 2;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 30;
        } else if (lv == 3) {
            powerConsumption = 60;
        } else {
            throw invalid_argument("Training构造函数：训练室等级只能为1,2,3");
        }

        facilityType = TRAINING;
    }
};

// 设施比较器 - 按类型和等级排序
struct FacilityComparator {
    bool operator()(const unique_ptr<Facility> &a, const unique_ptr<Facility> &b) const {
        if (a->facilityType != b->facilityType) {
            return a->facilityType < b->facilityType; // 不同类型按类型升序
        }
        return a->level > b->level; // 同类型按等级降序
    }
};

// 设施比较器 - 按产出速率排序
struct FacilityValueComparator {
    bool operator()(const unique_ptr<Facility> &a, const unique_ptr<Facility> &b) const {
        double rateA = a->productValue / a->productTime;
        double rateB = b->productValue / b->productTime;
        return rateA > rateB; // 按产出速率降序排序
    }
};

// 设施比较器 - 按饱和时间排序
struct FacilitySaturationComparator {
    bool operator()(const unique_ptr<Facility> &a, const unique_ptr<Facility> &b) const {
        int saturationA = a->capacity / a->productSpace * a->productTime;
        int saturationB = b->capacity / b->productSpace * b->productTime;
        return saturationA > saturationB; // 按饱和时间降序排序
    }
};

// 基建全局参数，包括各个设施的直接加成值，以及其它影响干员基建技能的全局参数
struct GlobalParams {
    // 发电站（多个发电站视为1个）、办公室、会客室、控制中枢、训练室、加工站的数量固定
    // 宿舍的数量最多为4
    // 制造站、贸易站的数量可变，制造站、贸易站、发电站设施数量（发电站的设施数量为发电站等级）之和至多为9
    Power power;                                  // 发电站
    Office office;                                // 办公室
    Reception reception;                          // 会客室
    Control control;                              // 控制中枢
    Training training;                            // 训练室
    Processing processing;                        // 加工站
    vector<Dormitory> dormitories;                // 宿舍
    vector<unique_ptr<Facility>> otherFacilities; // 其他设施（制造站、贸易站）

    int facilityCount[20] = {0};     // 各设施类型的数量
    vector<Operator *> allOperators; // 所有驻守的干员

    struct {
        // 将所有参数初始化为0
        int ren_jian_yan_huo = 0;         // 人间烟火
        int wu_shu_jie_jing = 0;          // 巫术结晶
        int wu_sa_si_te_yin = 0;          // 乌萨斯特饮
        int gong_cheng_ji_qi_ren = 0;     // 工程机器人
        int gan_zhi_xin_xi = 0;           // 感知信息
        int si_wei_lian_huan = 0;         // 思维链环
        int mo_wu_liao_li = 0;            // 魔物料理
        int mu_tian_liao = 0;             // 木天蓼
        int trade_count_by_influence = 0; // 受技能影响导致的贸易站名义上数量变化值
        int power_count_by_influence = 0; // 受技能影响导致的发电站名义上数量变化值
    } spec;

    GlobalParams() {}
    /*
     * @brief GlobalParams构造函数，初始化基建全局参数
     * @param facilityCounts 设施数量列表，形式为{<设施类型, 等级>, ... }
     *
     * @note 设施类型参见宏定义
     * @note 制造站、贸易站、发电站设施数量之和至多为9个，且发电站等级必须为3
     * @note 宿舍数量至多为4个
     * @note 上限为1的设施类型数量至多为1个
     */
    GlobalParams(vector<pair<int, int>> facilityCounts) { reset(facilityCounts); }

    void reset(vector<pair<int, int>> facilityCounts) {
        spec = {};
        memset(facilityCount, 0, sizeof(facilityCount));
        allOperators.clear();
        otherFacilities.clear();
        dormitories.clear();

        for (auto p : facilityCounts) {
            int tp = p.first;
            int lv = p.second;
            facilityCount[tp]++;
            if (tp == CONTROL) {
                control = Control(lv);
            } else if (tp == RECEPTION) {
                reception = Reception(lv);
            } else if (tp == OFFICE) {
                office = Office(lv);
            } else if (tp == TRAINING) {
                training = Training(lv);
            } else if (tp == PROCESSING) {
                processing = Processing(lv);
            } else if (tp == DORMITORY) {
                dormitories.emplace_back(lv);
                if ((int)dormitories.size() > 4) {
                    throw invalid_argument("GlobalParams构造函数：宿舍数量不能超过4个");
                }
            } else if (tp == POWER) {
                if (lv != 3) {
                    throw invalid_argument("GlobalParams构造函数：发电站等级只能为3");
                }
            } else if (tp == MFG_GOLD) {
                otherFacilities.push_back(make_unique<Mfg_Gold>(lv));
            } else if (tp == MFG_RECORDS) {
                otherFacilities.push_back(make_unique<Mfg_Records>(lv));
            } else if (tp == MFG_ORIGINIUM) {
                otherFacilities.push_back(make_unique<Mfg_Originium>(lv));
            } else if (tp == TRADE_ORUNDUM) {
                otherFacilities.push_back(make_unique<Trade_Orundum>(lv));
            } else if (tp == TRADE_LMD) {
                otherFacilities.push_back(make_unique<Trade_LMD>(lv));
            } else {
                throw invalid_argument("GlobalParams构造函数：设施类型错误");
            }
        }
        power = Power(facilityCount[POWER]);

        int totalCount = otherFacilities.size() + facilityCount[POWER];
        if (totalCount > 9) {
            throw invalid_argument(
                "GlobalParams构造函数：制造站、贸易站、发电站设施数量之和不能超过9个");
        }
        for (int tp = 1; tp <= 12; tp++) {
            if (isMfg(tp))
                continue;
            if (isTrade(tp))
                continue;
            if (tp == POWER)
                continue;
            if (tp == DORMITORY)
                continue;
            if (facilityCount[tp] > 1) {
                throw invalid_argument("GlobalParams构造函数：上限为1的设施类型数量不能超过1个");
            }
        }

        auto allFacilities = getAllFacilities();
        int powerConsumptionSum = 0;
        for (const auto &fac : allFacilities) {
            powerConsumptionSum += fac->powerConsumption;
        }
        if (powerConsumptionSum > 0) {
            throw invalid_argument("GlobalParams构造函数：设施总耗电量不能大于总发电量");
        }
    }

    vector<Facility *> getAllFacilities() {
        vector<Facility *> facilities;

        facilities.push_back(&power);
        facilities.push_back(&office);
        facilities.push_back(&reception);
        facilities.push_back(&control);
        facilities.push_back(&training);
        facilities.push_back(&processing);

        for (auto &dorm : dormitories) {
            facilities.push_back(&dorm);
        }

        for (auto &fac : otherFacilities) {
            facilities.push_back(fac.get());
        }

        return facilities;
    }

    // 获取所有设施中驻守的干员列表
    vector<Operator *> getAllOperators() {
        if (!allOperators.empty())
            return allOperators;
        vector<Operator *> ops;
        auto facilities = getAllFacilities();
        for (const auto &fac : facilities) {
            for (const auto &op : fac->operators) {
                ops.push_back(op);
            }
        }
        allOperators = ops;
        return ops;
    }
};
