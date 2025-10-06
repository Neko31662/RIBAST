#include <bits/stdc++.h>
using namespace std;

#include "globalParams.h"

bool isMfg(int facilityType) {
    return facilityType == MFG_GOLD || facilityType == MFG_RECORDS || facilityType == MFG_ORIGINIUM;
}

bool isTrade(int facilityType) {
    return facilityType == TRADE_ORUNDUM || facilityType == TRADE_LMD;
}

double productValueList[15][6];
double itemValueList[3];
bool hasLoadedValueList = 0;
void loadValueList() {
    if (hasLoadedValueList) {
        return;
    }
    hasLoadedValueList = 1;
    for (int i = 1; i <= 12; i++) {
        for (int j = 1; j <= 5; j++) {
            productValueList[i][j] = 0.0;
        }
    }

    productValueList[MFG_RECORDS][1] = 3.0 * 60 * (200.0 / 1000);
    productValueList[MFG_RECORDS][2] = 3.0 * 60 * (400.0 / 1000);
    productValueList[MFG_RECORDS][3] = 3.0 * 60;

    // 计算售卖单个赤金的价值
    int productTimeList[3] = {2 * 60 + 24, 3 * 60 + 30, 4 * 60 + 36}; // 2、3、4赤金订单生产时间
    // 3级贸易站每份订单平均生产时间
    double productTime_lv3 =
        (30.0 * productTimeList[0] + 50.0 * productTimeList[1] + 20.0 * productTimeList[2]) / 100;
    // 3级贸易站每份订单平均产出赤金数量
    double averageGoldCount_lv3 = (2 * 30.0 + 3 * 50.0 + 4 * 20.0) / 100;
    // 交付1赤金（获取500龙门币）的价值
    double productValue_OrderPerGold = productTime_lv3 / averageGoldCount_lv3;

    // 计算单个线索的价值
    // 假设1个线索收益为45信用（直接使用为30，送人为两方带来收益分别为20、30+(0~15)）
    double productValue_perClue = 45.0;
    // 1信用对应18龙门币（50% off）
    productValue_perClue *= 18.0;
    // 1龙门币对应 （1/500贸易站赤金 + 1/500制造站赤金）
    double value_LMD = (productValue_OrderPerGold + 1 * 60 + 12) / 500.0;
    productValue_perClue *= value_LMD;

    for (int j = 1; j <= 3; j++) {
        productValueList[MFG_GOLD][j] = 1 * 60 + 12;
        productValueList[MFG_ORIGINIUM][j] = 1 * 60;
        productValueList[TRADE_ORUNDUM][j] = 2 * 60;
        productValueList[TRADE_LMD][j] = productValue_OrderPerGold;
        productValueList[RECEPTION][j] = productValue_perClue;
        productValueList[POWER][j] = 3.0;
    }

    // 计算一次刷新的价值
    // 公招出3~6星的概率，数据来源：https://ark.yituliu.cn/survey/maarecruitdata（截止2025.10.01）
    double probability[4] = {0.8394, 0.1530, 0.0055, 0.0021};
    // 每个星级对应的绿票和黄票收益，3、4星为满潜，5、6星为未满潜
    pair<int, int> benefit[4] = {{10, 0}, {30, 1}, {0, 5}, {0, 10}};
    // 合成玉的价值:龙门币价值 = 0.75:0.0036
    double value_orundum = 0.75 / 0.0036 * value_LMD;

    // 计算一张绿票价值
    double greenTicketValue = 1.0;
    greenTicketValue *= 30.0 / 50.0; // 50绿票对应30合成玉
    greenTicketValue *= value_orundum;

    // 计算一张黄票价值
    double yellowTicketValue = 1.0;
    yellowTicketValue *= 38.0 / 258.0;        // 258黄票对应38抽
    yellowTicketValue *= 600 * value_orundum; // 1抽对应600合成玉

    // 计算：刷新后公招收益期望 - 刷新前（3星）公招收益
    pair<double, double> expectedBenefit = {0.0, 0.0};
    for (int i = 0; i < 4; i++) {
        expectedBenefit.first += probability[i] * benefit[i].first;
        expectedBenefit.second += probability[i] * benefit[i].second;
    }
    expectedBenefit.first -= benefit[0].first;
    expectedBenefit.second -= benefit[0].second;
    double productValue_refresh =
        expectedBenefit.first * greenTicketValue + expectedBenefit.second * yellowTicketValue;

    for (int j = 1; j <= 3; j++) {
        productValueList[OFFICE][j] = productValue_refresh;
    }

    itemValueList[LMD] = value_LMD;
    itemValueList[ORUNDUM] = value_orundum;
}

double getProductValue(int facilityType, int level) {
    if (hasLoadedValueList == 0) {
        loadValueList();
    }
    if (facilityType < 1 || facilityType > 12 || level < 1 || level > 5) {
        throw invalid_argument("getProductValue函数：facilityType或level参数错误");
    }
    return productValueList[facilityType][level];
}
