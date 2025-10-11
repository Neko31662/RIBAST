#pragma once
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "operator.h"

// 设施编号
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

// 物品编号
#define LMD 1
#define ORUNDUM 2

// 设施产品的价值表
// 赤金、中级作战记录、源石碎片、龙门币、合成玉产品的价值由三级设施生产一份产品所需的时间确定；
//
// 无人机的价值由无人机充能带来的基础耗时降低值确定（即3分钟）；
//
// 基础作战记录、初级作战记录的价值根据其与中级作战记录的经验比例确定；
//
// 线索的价值确定方式如下：由线索->信用->龙门币（50% off）确定每线索对应的龙门币价值；
//
// 公招刷新次数的价值确定方式如下：公招刷新带来的黄票与绿票的增益期望->
// (黄票->黄票对应的抽数（258黄票对38抽）->每抽对应的合成玉数量) +
// (绿票->绿票在资质凭证区阶段3对应的合成玉) ->每刷新次数对应的合成玉数量增益。
extern double productValueList[15][6];

// 单个物品的价值表
// 龙门币价值由 (制造站制造赤金的价值 + 贸易站龙门币订单的价值) 确定；
// 单个合成玉的价值:单个龙门币价值 = 0.75:0.0036。
extern double itemValueList[3];

extern bool hasLoadedValueList;
void loadValueList();                                // 加载价值表
double getProductValue(int facilityType, int level); // 获取产品价值

// 判断设施类型是否为制造站
bool isMfg(int facilityType);

// 判断设施类型是否为贸易站
bool isTrade(int facilityType);

// 单个设施的基类
struct Facility {
    // 只与设施种类有关的参数

    // （可能）与设施等级有关的参数
    int level = 0;            // 设施等级
    int efficiency = 0;       // 效率，使用整数表示，基础效率100代表1.0
    int capacity = 0;         // 容量
    int operatorLimit = 0;    // 干员数量上限
    int powerConsumption = 0; // 设施的电力消耗

    // （可能）与设施生产产品（和设施等级）有关的参数
    int facilityType = -1;       // 设施类型，在派生类构造时指定
    int productSpace = 1;        // 单个产品占用的容量
    double productTime = 1;      // 单个产品在效率1.0下的生产时间，单位分钟
    double productValue = 0.001; // 单个产品的基础价值

    std::vector<Operator *> operators; // 设施中驻守的干员列表

    // 设施特化属性的基类
    struct FacilitySpecBase {
        virtual ~FacilitySpecBase() = default;
        virtual std::unique_ptr<FacilitySpecBase> clone() const = 0;
    };
    std::unique_ptr<FacilitySpecBase> spec; // 设施特化属性

    Facility() {}
    Facility(int facilityType, int lv) {
        this->facilityType = facilityType;
        level = lv;
        productValue = getProductValue(facilityType, lv);
    }
    // 返回该设施是否在工作，工作的定义为，该设施所制造的产品是否在生产
    virtual bool isWorking() const { return !operators.empty(); }

    // 获取特定类型的 spec，类型安全的访问方式
    template <typename T> T *getSpec() { return dynamic_cast<T *>(spec.get()); }
    template <typename T> const T *getSpec() const { return dynamic_cast<const T *>(spec.get()); }

    // 返回设施名称
    std::string getName() const;

    // 返回是否含有指定名称的干员
    bool hasOperator(std::string opName) const {
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

        bool has_skill_hui_shou_li_yong = false;     // 是否含有“回收利用”技能
        double efficiency_by_hui_shou_li_yong = 0.0; // “回收利用”技能提供的效率值

        bool has_skill_tuan_dui_jing_shen = false; // 是否含有“团队精神”技能

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<MfgSpec>(*this);
        }
    };

    Mfg() {}
    Mfg(int facilityType, int lv) : Facility(facilityType, lv) {
        efficiency = 100;
        spec = std::make_unique<MfgSpec>();

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
            throw std::invalid_argument("Mfg构造函数：制造站等级只能为1,2,3");
        }
    }
};

// 制造站-赤金
struct Mfg_Gold : Mfg {
    // 赤金制造站的Spec
    struct Mfg_GoldSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<Mfg_GoldSpec>(*this);
        }
    };

    Mfg_Gold() {}
    Mfg_Gold(int lv) : Mfg(MFG_GOLD, lv) {
        productSpace = 2;
        productTime = 1 * 60 + 12;
        spec = std::make_unique<Mfg_GoldSpec>();
    }
};

// 制造站-作战记录
struct Mfg_Records : Mfg {
    // 作战记录制造站的Spec
    struct Mfg_RecordsSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<Mfg_RecordsSpec>(*this);
        }
    };

    Mfg_Records() {}
    Mfg_Records(int lv) : Mfg(MFG_RECORDS, lv) {
        if (lv == 1) {
            // 生产基础作战记录
            productSpace = 2;
            productTime = 45;
        } else if (lv == 2) {
            // 生产初级作战记录
            productSpace = 3;
            productTime = 1 * 60 + 20;
        } else if (lv == 3) {
            // 生产中级作战记录
            productSpace = 5;
            productTime = 3 * 60;
        }
        spec = std::make_unique<Mfg_RecordsSpec>();
    }
};

// 制造站-源石碎片
struct Mfg_Originium : Mfg {
    // 源石碎片制造站的Spec
    struct Mfg_OriginiumSpec : public MfgSpec {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<Mfg_OriginiumSpec>(*this);
        }
    };

    Mfg_Originium() {}
    Mfg_Originium(int lv) : Mfg(MFG_ORIGINIUM, lv) {
        if (lv != 3) {
            throw std::invalid_argument("Mfg_Originium构造函数：源石碎片制造站等级只能为3");
        }
        productSpace = 3;
        productTime = 1 * 60;
        spec = std::make_unique<Mfg_OriginiumSpec>();
    }
};

// 贸易站
struct Trade : Facility {
    // 贸易站的Spec
    struct TradeSpec : public FacilitySpecBase {

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<TradeSpec>(*this);
        }
    };

    Trade() {}
    Trade(int facilityType, int lv) : Facility(facilityType, lv) {
        efficiency = 100;
        spec = std::make_unique<TradeSpec>();

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
            throw std::invalid_argument("Trade构造函数：贸易站等级只能为1,2,3");
        }
    }
};

// 贸易站-合成玉
struct Trade_Orundum : Trade {
    // 合成玉贸易站的Spec
    struct Trade_OrundumSpec : public TradeSpec {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<Trade_OrundumSpec>(*this);
        }
    };

    Trade_Orundum() {}
    Trade_Orundum(int lv) : Trade(TRADE_ORUNDUM, lv) {
        if (lv != 3) {
            throw std::invalid_argument("Trade_Orundum构造函数：合成玉贸易站等级只能为3");
        }
        productSpace = 1;
        productTime = 2 * 60;
        spec = std::make_unique<Trade_OrundumSpec>();
    }
};

// 贸易站-龙门币
struct Trade_LMD : Trade {
    // 贸易站-龙门币的productValue指的是1赤金对应的订单价值（即无加成3级贸易站下售卖1赤金的所需期望时间）
    // 贸易站-龙门币的productTime指的是1赤金对应的订单生产时间（与productValue相等）

    // 龙门币贸易站的Spec
    struct Trade_LMDSpec : public TradeSpec {
        bool has_skill_hui_yan_du_dao = false; // 是否含有“慧眼独到”技能
        bool has_skill_he_tong_fa = false;     // 是否含有“合同法”技能

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<Trade_LMDSpec>(*this);
        }
    };

    int productTimeList[3];                 // 分别对应2,3,4赤金订单的生产时间
    int productRate[3];                     // 分别对应2,3,4赤金订单的生产权重，初始的总和为100，修改后总和不一定为100
    int productNumber[3] = {2, 3, 4};       // 分别对应2,3,4赤金订单的实际交付赤金个数
    int productLMD[3] = {1000, 1500, 2000}; // 分别对应2,3,4赤金订单的实际获取龙门币数

    Trade_LMD() {}
    Trade_LMD(int lv) : Trade(TRADE_LMD, lv) {
        productSpace = 1;

        productTimeList[0] = 2 * 60 + 24;
        productTimeList[1] = 3 * 60 + 30;
        productTimeList[2] = 4 * 60 + 36;
        // 2,3,4赤金订单生产概率数据来源：https://m.prts.wiki/w/%E7%BD%97%E5%BE%B7%E5%B2%9B%E5%9F%BA%E5%BB%BA/%E8%B4%B8%E6%98%93%E7%AB%99
        if (lv == 1) {
            productRate[0] = 100;
            productRate[1] = 0;
            productRate[2] = 0;
        } else if (lv == 2) {
            productRate[0] = 60;
            productRate[1] = 40;
            productRate[2] = 0;
        } else if (lv == 3) {
            productRate[0] = 30;
            productRate[1] = 50;
            productRate[2] = 20;
        }

        productTime = 0;
        int goldCount = 0;
        for (int i = 0; i < 3; i++) {
            productTime += productTimeList[i] * productRate[i];
            goldCount += productNumber[i] * productRate[i];
        }
        productTime /= (double)goldCount;

        spec = std::make_unique<Trade_LMDSpec>();
    }

    // 确定2,3,4赤金订单的生产时间、生产概率、实际交付赤金个数、实际获取龙门币数后，平均每份订单的价值
    double getValuePerOrder();

    // 确定2,3,4赤金订单的生产时间、生产概率、实际交付赤金个数、实际获取龙门币数后，返回平均每份订单的生产时间
    double getTimePerOrder();

    // 修改2,3,4赤金订单的生产概率
    void changeProductRate(std::vector<double> newRate);

    // 修改2,3,4赤金订单的实际交付赤金个数，并将获得的龙门币个数自动设为赤金数*500
    void changeProductNumber(std::vector<int> newNumber);

    // 不改变2,3,4赤金订单实际交付赤金个数，修改某一类订单的实际获取龙门币数
    void changeProductLMD(int index, int new_LMD);
};

// 会客室（获取线索）
struct Reception : Facility {
    // 会客室的Spec
    struct ReceptionSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<ReceptionSpec>(*this);
        }
    };

    Reception() {}
    Reception(int lv) : Facility(RECEPTION, lv) {
        spec = std::make_unique<ReceptionSpec>();

        // 效率计算参考：https://m.prts.wiki/w/%E7%BD%97%E5%BE%B7%E5%B2%9B%E5%9F%BA%E5%BB%BA/%E4%BC%9A%E5%AE%A2%E5%AE%A4
        efficiency = 100;
        // 会客室等级加成
        if (lv == 1) {
            efficiency += 0.07;
        } else if (lv == 2) {
            efficiency += 0.09;
        } else if (lv == 3) {
            efficiency += 0.11;
        } else {
            throw std::invalid_argument("Reception构造函数：会客室等级只能为1,2,3");
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

        productSpace = 1;
        productTime = 20 * 60;
    }
};

// 发电站（无人机自动回复视为发电站的产出，所有发电站默认为满级，且视为一个设施，该结构体的level值代表发电站数量）
struct Power : Facility {
    // 发电站的Spec
    struct PowerSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<PowerSpec>(*this);
        }
    };

    Power() {}
    Power(int number) : Facility(POWER, number) {
        if (number <= 0) {
            throw std::invalid_argument("Power构造函数：发电站数量不能为0或负数");
        }
        level = number;
        spec = std::make_unique<PowerSpec>();
        efficiency = 100;
        capacity = 235; // 后面或许会改成可变值
        operatorLimit = number;
        powerConsumption = -270 * number;
        productSpace = 1;
        productTime = 6;
    }

    // 无干员驻守时无人机也会充能，因此发电站在无人时也视为工作
    bool isWorking() const override { return true; }
};

// 办公室（获取公招刷新次数）
struct Office : Facility {
    // 办公室的Spec
    struct OfficeSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<OfficeSpec>(*this);
        }
    };

    int recruitSlots; // 招募位个数

    Office() {}
    Office(int lv) : Facility(OFFICE, lv) {
        spec = std::make_unique<OfficeSpec>();
        efficiency = 105; // 只要有干员进驻就获得5%的效率加成
        capacity = 3;
        operatorLimit = 1;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 30;
        } else if (lv == 3) {
            powerConsumption = 60;
        } else {
            throw std::invalid_argument("Office构造函数：办公室等级只能为1,2,3");
        }

        productSpace = 1;
        productTime = 12 * 60;
    }
};

// 控制中枢
struct Control : Facility {
    // 控制中枢的Spec
    struct ControlSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<ControlSpec>(*this);
        }
    };

    Control() {}
    Control(int lv) : Facility(CONTROL, lv) {
        if (lv <= 0 || lv > 5) {
            throw std::invalid_argument("Control构造函数：控制中枢等级只能为1~5");
        }
        spec = std::make_unique<ControlSpec>();
        operatorLimit = lv;
        powerConsumption = 0;
    }
};

// 宿舍
struct Dormitory : Facility {
    double basicRecoverySpeed; // 基础恢复速率
    int atmosphere = 5000;     // 氛围值

    double getRecoverySpeed() const {
        double speed = basicRecoverySpeed;
        speed += std::min(atmosphere, 1000 * level) * 0.0004;
        return speed;
    }

    Dormitory() {}
    Dormitory(int lv) : Facility(DORMITORY, lv) {
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
            throw std::invalid_argument("Dormitory构造函数：宿舍等级只能为1~5");
        }
    }

    bool isWorking() const override { return false; }
};

// 加工站
struct Processing : Facility {
    Processing() {}
    Processing(int lv) : Facility(PROCESSING, lv) {
        operatorLimit = 1;

        if (lv == 1 || lv == 2 || lv == 3) {
            powerConsumption = 10;
        } else {
            throw std::invalid_argument("Processing构造函数：加工站等级只能为1,2,3");
        }
    }
};

// 训练室
struct Training : Facility {
    Training() {}
    Training(int lv) : Facility(TRAINING, lv) {
        operatorLimit = 2;

        if (lv == 1) {
            powerConsumption = 10;
        } else if (lv == 2) {
            powerConsumption = 30;
        } else if (lv == 3) {
            powerConsumption = 60;
        } else {
            throw std::invalid_argument("Training构造函数：训练室等级只能为1,2,3");
        }
    }
};

// 设施比较器 - 按类型和等级排序
struct FacilityComparator {
    bool operator()(const std::unique_ptr<Facility> &a, const std::unique_ptr<Facility> &b) const {
        if (a->facilityType != b->facilityType) {
            return a->facilityType < b->facilityType; // 不同类型按类型升序
        }
        return a->level > b->level; // 同类型按等级降序
    }
};

// 设施比较器 - 按产出速率排序
struct FacilityValueComparator {
    bool operator()(const std::unique_ptr<Facility> &a, const std::unique_ptr<Facility> &b) const {
        double rateA = a->productValue / a->productTime;
        double rateB = b->productValue / b->productTime;
        return rateA > rateB; // 按产出速率降序排序
    }
};

// 设施比较器 - 按饱和时间排序
struct FacilitySaturationComparator {
    bool operator()(const std::unique_ptr<Facility> &a, const std::unique_ptr<Facility> &b) const {
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
    Power power;                                            // 发电站
    Office office;                                          // 办公室
    Reception reception;                                    // 会客室
    Control control;                                        // 控制中枢
    Training training;                                      // 训练室
    Processing processing;                                  // 加工站
    std::vector<Dormitory> dormitories;                     // 宿舍
    std::vector<std::unique_ptr<Facility>> otherFacilities; // 其他设施（制造站、贸易站）

    int facilityCount[20] = {0}; // 各设施类型的数量

    int clearDuration = 0; // 清理基建时间，以小时为单位，刚清理后为0小时

    struct {
        // 将所有参数初始化为0
        int ren_jian_yan_huo = 0;     // 人间烟火
        int wu_shu_jie_jing = 0;      // 巫术结晶
        int wu_sa_si_te_yin = 0;      // 乌萨斯特饮
        int gong_cheng_ji_qi_ren = 0; // 工程机器人
        int gan_zhi_xin_xi = 0;       // 感知信息
        int si_wei_lian_huan = 0;     // 思维链环
        int mo_wu_liao_li = 0;        // 魔物料理
        int mu_tian_liao = 0;         // 木天蓼
        int wu_sheng_gong_ming = 0;   // 无声共鸣
        // 赤金生产线（技能“订单流可视化”的参考值为赤金贸易站实际个数，其它技能的参考值都为赤金贸易站实际个数+该值）
        int extra_chi_jin_sheng_chan_xian = 0;
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
    GlobalParams(std::vector<std::pair<int, int>> facilityCounts) { reset(facilityCounts); }

    void reset(std::vector<std::pair<int, int>> facilityCounts) {
        spec = {};
        memset(facilityCount, 0, sizeof(facilityCount));
        otherFacilities.clear();
        dormitories.clear();
        cache_allFacilities_valid = false;
        cache_allOperators_valid = false;

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
                    throw std::invalid_argument("GlobalParams构造函数：宿舍数量不能超过4个");
                }
            } else if (tp == POWER) {
                if (lv != 3) {
                    throw std::invalid_argument("GlobalParams构造函数：发电站等级只能为3");
                }
            } else if (tp == MFG_GOLD) {
                otherFacilities.push_back(std::make_unique<Mfg_Gold>(lv));
            } else if (tp == MFG_RECORDS) {
                otherFacilities.push_back(std::make_unique<Mfg_Records>(lv));
            } else if (tp == MFG_ORIGINIUM) {
                otherFacilities.push_back(std::make_unique<Mfg_Originium>(lv));
            } else if (tp == TRADE_ORUNDUM) {
                otherFacilities.push_back(std::make_unique<Trade_Orundum>(lv));
            } else if (tp == TRADE_LMD) {
                otherFacilities.push_back(std::make_unique<Trade_LMD>(lv));
            } else {
                throw std::invalid_argument("GlobalParams构造函数：设施类型错误");
            }
        }
        power = Power(facilityCount[POWER]);

        int totalCount = otherFacilities.size() + facilityCount[POWER];
        if (totalCount > 9) {
            throw std::invalid_argument(
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
                throw std::invalid_argument(
                    "GlobalParams构造函数：上限为1的设施类型数量不能超过1个");
            }
        }

        auto allFacilities = getAllFacilities();
        int powerConsumptionSum = 0;
        for (const auto &fac : allFacilities) {
            powerConsumptionSum += fac->powerConsumption;
        }
        if (powerConsumptionSum > 0) {
            throw std::invalid_argument("GlobalParams构造函数：设施总耗电量不能大于总发电量");
        }
    }

    // 获取所有设施的列表
    std::vector<Facility *> getAllFacilities();

    // 获取所有设施中驻守的干员列表
    std::vector<Operator *> getAllOperators();

  private:
    std::vector<Facility *> cache_allFacilities; // 设施列表缓存
    bool cache_allFacilities_valid = false;      // 设施列表缓存是否有效

    std::vector<Operator *> cache_allOperators; // 干员列表缓存
    bool cache_allOperators_valid = false;      // 干员列表缓存是否有效
};
