#pragma once
#ifdef _WIN32
#include "characterConverter.h"
#endif

#include "operator.h"
#include "excelLoader.h"
#include "operatorData.h"

extern std::map<int, Operator> playerOperatorData;        // 玩家拥有的干员数据，key为干员uid
extern std::vector<std::string> unrecordedOperators;      // 未记录的干员名称
void loadPlayerOperatorData(const std::string &filePath); // 加载玩家干员数据
