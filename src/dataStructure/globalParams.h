#pragma once
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "arrangement.h"
#include "dataLoader.h"
#include "operator.h"
#include "facility.h"

// 基建全局参数，包括各个设施的直接加成值，以及其它影响干员基建技能的全局参数
struct GlobalParams {
    // 发电站（多个发电站视为1个）、办公室、会客室、控制中枢、训练室、加工站的数量固定
    // 宿舍的数量最多为4
    // 制造站、贸易站的数量可变，制造站、贸易站、发电站设施数量（发电站的设施数量为发电站等级）之和至多为9
    std::array<std::vector<std::shared_ptr<Facility>>, FACILITY_TYPE_COUNT + 1>
        facilities; // 按设施类型存储的设施列表，索引0未使用，1~12分别对应设施类型

    std::array<int, FACILITY_TYPE_COUNT + 1> facilityCount = {0}; // 各设施类型的数量

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
     * @param facilityCounts 设施数量列表，形式为{
     * <设施类型a, {设施a-1等级, 设施a-2等级, ...}>,
     * <设施类型b, {设施b-1等级, 设施b-2等级, ...}>,
     *  ... }
     *
     * @note 设施类型参见宏定义；
     * @note 制造站、贸易站、发电站设施数量之和至多为9个，且发电站等级必须为3；
     * @note 宿舍数量至多为4个；
     * @note 上限为1的设施类型数量至多为1个
     */
    GlobalParams(const FacilitiesInfo &facilityCounts) { reset(facilityCounts); }

    /*
     * @brief 重设基建建筑类型和全局参数
     * @param facilityCounts 设施数量列表，形式为{{},
     * {关键字为1的设施-1的等级, 关键字为1的设施-2的等级, ...},
     * {关键字为2的设施-1的等级, 关键字为2的设施-2的等级, ...},
     *  ... }
     *
     * @note 设施类型参见宏定义；
     * @note 制造站、贸易站、发电站设施数量之和至多为9个，且发电站等级必须为3；
     * @note 宿舍数量至多为4个；
     * @note 上限为1的设施类型数量至多为1个
     */
    void reset(const FacilitiesInfo &facilityCounts);

    /*
     * @brief 设置干员驻守安排
     * @param operatorArrangement 干员驻守安排，形式为{{},
     * {{关键字为1的设施-1的干员列表}, {关键字为1的设施-2的干员列表}, ...},
     * {{关键字为2的设施-1的干员列表}, {关键字为2的设施-2的干员列表}, ...},
     *  ... }
     *
     * @note 设施类型参见宏定义；
     * @note 数量限制不为1的设施，设施的顺序必须和调用reset时一致；
     */
    void arrangeOperators(const SingleSchedule &operatorArrangement);

    // 获取所有设施的列表
    std::vector<std::shared_ptr<Facility>> getAllFacilities();

    // 获取所有设施中驻守的干员列表
    std::vector<std::shared_ptr<Operator>> getAllOperators();

  private:
    std::vector<std::shared_ptr<Facility>> cache_allFacilities; // 设施列表缓存
    bool cache_allFacilities_valid = false;                     // 设施列表缓存是否有效

    std::vector<std::shared_ptr<Operator>> cache_allOperators; // 干员列表缓存
    bool cache_allOperators_valid = false;                     // 干员列表缓存是否有效
};
