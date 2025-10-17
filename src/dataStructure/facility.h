#pragma once
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
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
#define MEETING 8       // 会客室
#define OFFICE 9        // 办公室
#define DORMITORY 10    // 宿舍
#define PROCESSING 11   // 加工站
#define TRAINING 12     // 训练室
#define FACILITY_TYPE_COUNT 12

// 物品编号
#define LMD 1
#define ORUNDUM 2

struct GlobalParams;

// 设施产品的价值表，productValueList[i][j]表示类型为i的设施，等级为j时，生产产品的价值
// 赤金、中级作战记录、源石碎片、龙门币、合成玉产品的价值由三级设施生产一份产品所需的时间确定；
//
// 无人机的价值由无人机充能带来的基础耗时降低值确定（即3分钟）；
//
// 基础作战记录、初级作战记录的价值根据其与中级作战记录的经验比例确定；
//
// 线索的价值确定方式如下：由线索->信用->龙门币（50%
// off）确定每线索对应的龙门币价值；
//
// 公招刷新次数的价值确定方式如下：公招刷新带来的黄票与绿票的增益期望->
// (黄票->黄票对应的抽数（258黄票对38抽）->每抽对应的合成玉数量) +
// (绿票->绿票在资质凭证区阶段3对应的合成玉) ->每刷新次数对应的合成玉数量增益。
extern double productValueList[FACILITY_TYPE_COUNT + 1][6];

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

// 判断设施类型是否为个数上限为1的设施（发电站不算上限为1）
bool isUniqueFacility(int facilityType);

// 获取设施类型名称
std::string getFacilityTypeName(int facilityType);

// 单个设施的基类
struct Facility {
    // 只与设施种类有关的参数

    // （可能）与设施等级有关的参数
    int level = 0;            // 设施等级
    double efficiency = 0;    // 效率，使用整数表示，基础效率100代表1.0
    int capacity = 0;         // 容量
    int operatorLimit = 0;    // 干员数量上限
    int powerConsumption = 0; // 设施的电力消耗

    // （可能）与设施生产产品（和设施等级）有关的参数
    int facilityType = -1;       // 设施类型，在派生类构造时指定
    int productSpace = 1;        // 单个产品占用的容量
    double productTime = 1;      // 单个产品在效率1.0下的生产时间，单位分钟
    double productValue = 0.001; // 单个产品的基础价值

    std::vector<std::shared_ptr<Operator>> operators; // 设施中驻守的干员列表

    Facility() {}
    Facility(int facilityType, int lv) {
        this->facilityType = facilityType;
        level = lv;
        productValue = getProductValue(facilityType, lv);
    }

    // 设施特化属性的基类
    struct FacilitySpecBase {
        virtual ~FacilitySpecBase() = default;
        virtual std::unique_ptr<FacilitySpecBase> clone() const = 0;
    };
    std::unique_ptr<FacilitySpecBase> spec; // 设施特化属性
    // 获取特定类型的 spec，类型安全的访问方式
    template <typename T> T *getSpec() { return dynamic_cast<T *>(spec.get()); }
    template <typename T> const T *getSpec() const { return dynamic_cast<const T *>(spec.get()); }

    // 返回该设施是否在工作，工作的定义为，如果容量未满，该设施所制造的产品是否会继续生产
    virtual bool isWorking() const { return !operators.empty(); }

    // 返回当前驻守的干员数量，不要使用 operators.size()，因为有可能有空位
    int countOperators() const {
        int count = 0;
        for (auto &op : operators) {
            if (op != nullptr && op->name != "") {
                count++;
            }
        }
        return count;
    }

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

    // 计算实际效率，即综合了所有建筑自身属性以及干员技能后，得到的效率
    double calculateActualEfficiency(GlobalParams &gp) {
        if (!isWorking()) {
            return 0.0;
        }

        double actualEfficiency = efficiency;
        for (const auto &op : operators) {
            actualEfficiency += op->getEfficiencyEffect();
        }
        actualEfficiency += calculateEfficiencyByFacility(gp);

        return actualEfficiency;
    }

    // 计算由建筑本身属性和干员入驻情况决定的、与干员技能无关的效率加成
    virtual double calculateEfficiencyByFacility(GlobalParams &gp) { return 0.0; }
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

    double calculateEfficiencyByFacility(GlobalParams &gp) override {
        return 1.0 * countOperators();
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

    double calculateEfficiencyByFacility(GlobalParams &gp) override {
        return 1.0 * countOperators();
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

    int productTimeList[3]; // 分别对应2,3,4赤金订单的生产时间
    int productRate[3]; // 分别对应2,3,4赤金订单的生产权重，初始的总和为100，修改后总和不一定为100
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
struct Meeting : Facility {
    // 会客室的Spec
    struct MeetingSpec : public FacilitySpecBase {
        // 目前为空，后续可根据需要扩展

        std::unique_ptr<FacilitySpecBase> clone() const override {
            return std::make_unique<MeetingSpec>(*this);
        }
    };

    Meeting() {}
    Meeting(int lv) : Facility(MEETING, lv) {
        spec = std::make_unique<MeetingSpec>();

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
            throw std::invalid_argument("Meeting构造函数：会客室等级只能为1,2,3");
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

    double calculateEfficiencyByFacility(GlobalParams &gp) override;
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

    double calculateEfficiencyByFacility(GlobalParams &gp) override {
        return 5.0 * countOperators();
    }
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
        efficiency = 100;
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

    double calculateEfficiencyByFacility(GlobalParams &gp) override {
        return 5.0 * countOperators();
    };
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

        atmosphere = std::min(5000, 1000 * lv);
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