#pragma once

// 技能优先级定义

// 普通技能
// 普通技能的定义为：该技能只影响设施效率、设施容量、干员自身心情消耗速率；
// 该技能（在技能说明中）除了受进驻干员身份的影响外，不受任何其它因素影响。
// 例如技能“标准化”会受到水月的技能影响，但由于在技能说明中并未提及，因此归为普通技能
#define NORMAL 1000

// 受进驻时长影响的技能
#define EFF_BY_DURATION 1001

// 受全局参数（如：热情值）影响的技能
#define EFF_BY_GP 2000

// 受设施数量影响的技能
#define EFF_BY_FACILITY_COUNT 2100

// 改变全局参数的值的技能
#define GAN_ZHI_XIN_XI 400       // 改变感知信息的值的技能
#define REN_JIAN_YAN_HUO 450     // 改变人间烟火的值的技能
#define WU_SHU_JIE_JING 500      // 改变巫术结晶的值的技能（巫术结晶受人间烟火影响）
#define GONG_CHENG_JI_QI_REN 550 // 改变工程机器人数量的技能
#define SI_WEI_LIAN_HUAN 600     // 改变思维链环的值的技能（思维链环受感知信息影响）
#define MO_WU_LIAO_LI 650        // 改变魔物料理的值的技能

// 特殊技能
// 配合意识：进驻制造站时，当前制造站内其他干员提供的每5%生产力（不包含根据设施数量提供加成的生产力），额外提供5%生产力，最多提供40%生产力
#define PEI_HE_YI_SHI 2003
// 带有“心情落差”字样的技能，由于完全实现过于复杂，目前先用其它逻辑替代
#define XIN_QING_LUO_CHA 1000
// 大就是好！：进驻制造站时，当前制造站内干员根据自身提升的仓库容量提供一定的生产力。提升16格以下的，每格仓库容量提供1%生产力；提升大于16格的，每格仓库容量提供3%生产力（无法与回收利用叠加，且优先生效）
#define DA_JIU_SHI_HAO 2002
// 回收利用：进驻制造站时，当前制造站内干员提升的每格仓库容量，提供2%生产力
#define HUI_SHOU_LI_YONG 2001
// 团队精神：进驻制造站时，消除当前制造站内所有干员自身心情消耗的影响
#define TUAN_DUI_JING_SHEN 2004

// 检查约束条件：
// 1. 人间烟火 < 巫术结晶
static_assert(REN_JIAN_YAN_HUO < WU_SHU_JIE_JING);
// 2. 感知信息 < 思维链环
static_assert(GAN_ZHI_XIN_XI < SI_WEI_LIAN_HUAN);
// 3. 受全局参数影响的技能 < 配合意识 < 受设施数量影响的技能
static_assert(EFF_BY_GP < PEI_HE_YI_SHI);
static_assert(PEI_HE_YI_SHI < EFF_BY_FACILITY_COUNT);
// 4. 大就是好！ < 配合意识
static_assert(DA_JIU_SHI_HAO < PEI_HE_YI_SHI);
// 5. 回收利用 < 大就是好！
static_assert(HUI_SHOU_LI_YONG < DA_JIU_SHI_HAO);