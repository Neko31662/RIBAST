#include <string>
#include <utility>
#include <vector>

#include "facility.h"

using FacilitiesInfo = std::array<std::vector<int>, FACILITY_TYPE_COUNT + 1>;
using SingleSchedule = std::array<std::vector<std::vector<int>>, FACILITY_TYPE_COUNT + 1>;

struct Arrangement {
    FacilitiesInfo facilities;
    std::vector<int> scheduleDuration;        // 每个排班计划的工作时长
    std::vector<SingleSchedule> schedulePlan; // 每个排班计划的干员配置
    std::vector<int> Fiammeta;                // 菲亚梅塔的使用对象

    std::vector<std::string> illegalArrangementsInfo; // 存储不合法的排班安排信息

    bool checkLegal(); // 检查排班安排是否合法
};