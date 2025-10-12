#include "globalParams.h"
using std::invalid_argument;
using std::string;
using std::vector;

double productValueList[15][6];
double itemValueList[3];
bool hasLoadedValueList = false;

bool isMfg(int facilityType) {
    return facilityType == MFG_GOLD || facilityType == MFG_RECORDS || facilityType == MFG_ORIGINIUM;
}

bool isTrade(int facilityType) {
    return facilityType == TRADE_ORUNDUM || facilityType == TRADE_LMD;
}

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
    // 假设1个线索收益为65信用（获取线索得到20，之后直接使用为30，送人为两方带来收益分别为20、30+(0~15)）
    double productValue_perClue = 65.0;
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
    std::pair<int, int> benefit[4] = {{10, 0}, {30, 1}, {0, 5}, {0, 10}};
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
    std::pair<double, double> expectedBenefit = {0.0, 0.0};
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

string Facility::getName() const {
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
    case PROCESSING:
        return "加工站";
    case TRAINING:
        return "训练室";
    default:
        return "其它设施";
    }
}

double Trade_LMD::getValuePerOrder() {
    double expectedValue = 0.0;
    double totalRate = productRate[0] + productRate[1] + productRate[2];
    for (int i = 0; i < 3; i++) {
        if (productNumber[i] * 500 == productLMD[i]) {
            double productValue_OrderPerGold = productValue;
            expectedValue +=
                productValue_OrderPerGold * productNumber[i] * productRate[i] / totalRate;
        } else {
            // 1龙门币的价值
            double value_LMD = itemValueList[LMD];
            // 1制造站赤金的价值
            double productValue_perGold = productValueList[MFG_GOLD][3];
            // 相减得到该类型订单下，由工作时长得到的价值增益
            double benefit = value_LMD * productLMD[i] - productValue_perGold * productNumber[i];
            expectedValue += benefit * productRate[i] / totalRate;
        }
    }
    return expectedValue;
}

double Trade_LMD::getTimePerOrder() {
    double timePerOrder = 0.0;
    double totalRate = productRate[0] + productRate[1] + productRate[2];
    for (int i = 0; i < 3; i++) {
        timePerOrder += productTimeList[i] * productRate[i];
    }
    timePerOrder /= totalRate;
    return timePerOrder;
}

void Trade_LMD::changeProductRate(vector<double> newRate) {
    if (newRate.size() != 3) {
        throw invalid_argument("Trade_LMD::changeProductRate：newRate大小必须为3");
    }
    for (int i = 0; i < 3; i++) {
        productRate[i] = newRate[i];
    }
};

void Trade_LMD::changeProductNumber(vector<int> newNumber) {
    if (newNumber.size() != 3) {
        throw invalid_argument("Trade_LMD::changeProductNumber：newNumber大小必须为3");
    }
    for (int i = 0; i < 3; i++) {
        if (newNumber[i] <= 0) {
            throw invalid_argument("Trade_LMD::changeProductNumber：newNumber的元素必须大于0");
        }
        productNumber[i] = newNumber[i];
        productLMD[i] = productNumber[i] * 500;
    }
};

void Trade_LMD::changeProductLMD(int index, int new_LMD) {
    if (index < 0 || index >= 3) {
        throw invalid_argument("Trade_LMD::changeProductLMD：index参数错误");
    }
    if (new_LMD <= 0) {
        throw invalid_argument("Trade_LMD::changeProductLMD：new_LMD必须大于0");
    }
    productLMD[index] = new_LMD;
}

vector<Facility *> GlobalParams::getAllFacilities() {
    if (cache_allFacilities_valid) {
        return cache_allFacilities;
    }
    cache_allFacilities.clear();
    cache_allFacilities_valid = true;

    cache_allFacilities.push_back(&power);
    cache_allFacilities.push_back(&office);
    cache_allFacilities.push_back(&reception);
    cache_allFacilities.push_back(&control);
    cache_allFacilities.push_back(&training);
    cache_allFacilities.push_back(&processing);

    for (auto &dorm : dormitories) {
        cache_allFacilities.push_back(&dorm);
    }

    for (auto &fac : otherFacilities) {
        cache_allFacilities.push_back(fac.get());
    }

    return cache_allFacilities;
}

vector<Operator *> GlobalParams::getAllOperators() {
    if (cache_allOperators_valid) {
        return cache_allOperators;
    }
    cache_allOperators.clear();
    cache_allOperators_valid = true;

    auto facilities = getAllFacilities();
    for (const auto &fac : facilities) {
        for (const auto &op : fac->operators) {
            cache_allOperators.push_back(op);
        }
    }
    return cache_allOperators;
}