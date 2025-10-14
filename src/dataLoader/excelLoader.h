// excelLoader.h
// 提供使用 OpenXLSX 读取 .xlsx 文件的简易接口
#pragma once

#include <OpenXLSX.hpp>
#include <string>
#include <vector>

// 读取指定文件的第一个工作表，返回二维字符串表格。
// maxRows > 0 时限制最大读取行数。
std::vector<std::vector<std::string>> readFirstSheet(const std::string &filePath,
                                                     std::size_t maxRows = 0);

// 读取指定文件中给定名称的工作表，返回二维字符串表格。
// maxRows > 0 时限制最大读取行数。
std::vector<std::vector<std::string>>
readSheet(const std::string &filePath, const std::string &sheetName, std::size_t maxRows = 0);

// 示例：读取相对路径 ../../data/干员练度表.xlsx 并打印前几行。
// 注意：运行目录不同可能导致相对路径失效，本函数会优先尝试
// 使用项目根路径(PROJECT_SOURCE_DIR)/data/干员练度表.xlsx。
void exampleReadExcelRelative(std::string filePath);
