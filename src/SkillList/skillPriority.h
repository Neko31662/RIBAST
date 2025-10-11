#pragma once

// 技能优先级定义

// 改变全局参数（如：热情值）的值的技能
#define GAN_ZHI_XIN_XI 400       // 改变感知信息的值的技能
#define REN_JIAN_YAN_HUO 450     // 改变人间烟火的值的技能
#define WU_SHU_JIE_JING 500      // 改变巫术结晶的值的技能（巫术结晶受人间烟火影响）
#define GONG_CHENG_JI_QI_REN 550 // 改变工程机器人数量的技能
#define SI_WEI_LIAN_HUAN 600     // 改变思维链环的值的技能（思维链环受感知信息影响）
#define MO_WU_LIAO_LI 650        // 改变魔物料理的值的技能
#define WU_SHENG_GONG_MING 700   // 改变无声共鸣的值的技能（无声共鸣受感知信息影响）

// 普通技能
// 普通技能的定义为：该技能只影响设施效率、设施容量、干员自身心情消耗速率；
// 该技能（在技能说明中）除了受进驻干员身份的影响外，不受任何其它因素影响。
// 例如技能“标准化”会受到水月的技能影响，但由于在技能说明中并未提及，因此归为普通技能
#define NORMAL 1000

// 受进驻时长影响的技能
#define EFF_BY_DURATION 1001

// 受全局参数（如：热情值）影响的技能
#define EFF_BY_GP 1500

// 受基建容量的改变值、基建效率的改变值影响的技能
// 部分技能之间有特殊结算顺序，使用EFF_BY_FACILITY - 50 ~ EFF_BY_FACILITY + 50之间的值来区分
#define EFF_BY_FACILITY 2050
// 回收利用：进驻制造站时，当前制造站内干员提升的每格仓库容量，提供2%生产力
#define HUI_SHOU_LI_YONG (EFF_BY_FACILITY + 1)
// 大就是好！：进驻制造站时，当前制造站内干员根据自身提升的仓库容量提供一定的生产力。提升16格以下的，每格仓库容量提供1%生产力；提升大于16格的，每格仓库容量提供3%生产力（无法与回收利用叠加，且优先生效）
#define DA_JIU_SHI_HAO (EFF_BY_FACILITY + 2)
// 配合意识：进驻制造站时，当前制造站内其他干员提供的每5%生产力（不包含根据设施数量提供加成的生产力），额外提供5%生产力，最多提供40%生产力
#define PEI_HE_YI_SHI (EFF_BY_FACILITY + 3)
// 市井之道：进驻贸易站时，当前贸易站内其他干员提供的每10%订单获取效率使订单上限-1（订单最少为1），同时每有1笔订单就+4%订单获取效率（与部分技能有特殊叠加规则）
// 无法单独与天道酬勤·α、天道酬勤·β进行叠加，且优先生效
// 当天道酬勤·α、天道酬勤·β与其他技能进行叠加时，该技能会对此叠加效果进行叠加
#define SHI_JING_ZHI_DAO (EFF_BY_FACILITY - 1)
// 天道酬勤：进驻贸易站时，当前贸易站内干员提供的每5%订单获取效率，额外提供5%效率，最多提供25%（alpha）或35%（beta）效率
#define TIAN_DAO_CHOU_QIN (EFF_BY_FACILITY - 2)

// 受设施数量影响的技能
#define EFF_BY_FACILITY_COUNT 2100

// 修改贸易站赤金订单的：各类型订单概率、赤金交付数、龙门币获取数的技能
// 部分技能之间有特殊结算顺序，使用ORDER_CHANGE - 50 ~ ORDER_CHANGE + 50之间的值来区分
#define ORDER_CHANGE 2550
// 天真的谈判者：进驻贸易站时，订单获取效率+10%，当前贸易站赤金订单的赤金交付数必定为2（不视作违约订单）
#define TIAN_ZHEN_DE_TAN_PAN_ZHE (ORDER_CHANGE + 1)

// 结算类技能
#define END 3000
// 团队精神：进驻制造站时，消除当前制造站内所有干员自身心情消耗的影响
#define TUAN_DUI_JING_SHEN END
// 低语：进驻贸易站时，当前贸易站内其他干员提供的订单获取效率全部归零，且每人为自身+45%订单获取效率，同时全体心情每小时消耗+0.25
#define DI_YU END

// 特殊情况
// 带有“心情落差”字样的技能，由于完全实现过于复杂，目前先用其它逻辑替代
#define XIN_QING_LUO_CHA (NORMAL)
// 慧眼独到：进驻贸易站时，固定获取特别独占订单（不视作违约订单），且该类订单不受任何订单获取效率的影响
// 目前使用标识符表示，技能本体逻辑由facility实现
#define HUI_YAN_DU_DAO (NORMAL)
// 订单流可视化：进驻贸易站时，订单获取效率+5%，每有4条（alpha）或2条（beta）赤金生产线，则为当前贸易站额外提供2条赤金生产线
// 游戏内对其技能的实现存在bug，技能生效与干员入驻顺序有关，特此标注
#define DING_DAN_LIU_KE_SHI_HUA (NORMAL)
// 摊贩经济：进驻贸易站时，当前订单数与订单上限每差1笔订单，则订单获取效率+4%
// 实现过于复杂，用效率+（4% * 订单上限），入驻1小时后-4%，此后交替-3%和2%替代
// 如果同时拥有技能“市井之道”，则会清除该技能的效果，以正确的逻辑同时实现两个技能
#define TAN_FAN_JING_JI (EFF_BY_FACILITY - 10)

// 检查约束条件：变量
// 1. 人间烟火 < 巫术结晶
static_assert(REN_JIAN_YAN_HUO < WU_SHU_JIE_JING);
// 2. 感知信息 < 思维链环
static_assert(GAN_ZHI_XIN_XI < SI_WEI_LIAN_HUAN);
// 3. 感知信息 < 无声共鸣
static_assert(GAN_ZHI_XIN_XI < WU_SHENG_GONG_MING);

// 检查约束条件：技能
// 1. 大就是好！ < 配合意识
static_assert(DA_JIU_SHI_HAO < PEI_HE_YI_SHI);
// 2. 回收利用 < 大就是好！
static_assert(HUI_SHOU_LI_YONG < DA_JIU_SHI_HAO);
// 3. 天道酬勤 < 市井之道
static_assert(TIAN_DAO_CHOU_QIN < SHI_JING_ZHI_DAO);
// 4. 摊贩经济 < 市井之道
static_assert(TAN_FAN_JING_JI < SHI_JING_ZHI_DAO);