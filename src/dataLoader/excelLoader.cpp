#include "excelLoader.h"

#include <OpenXLSX.hpp>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

using std::string;
using std::vector;
namespace fs = std::filesystem;

static vector<vector<string>> readWorksheet(OpenXLSX::XLWorksheet ws, std::size_t maxRows) {
    vector<vector<string>> table;
    table.reserve(128);

    // 获取工作表的实际数据范围
    auto range = ws.range();
    if (range.numRows() == 0)
        return table;

    std::size_t endRow = range.numRows();
    if (maxRows > 0 && maxRows < endRow) {
        endRow = maxRows;
    }

    std::size_t numCols = range.numColumns();

    // 逐行读取
    for (std::size_t rowIdx = 1; rowIdx <= endRow; ++rowIdx) {
        vector<string> line;
        line.reserve(numCols);

        // 逐列读取
        for (std::size_t colIdx = 1; colIdx <= numCols; ++colIdx) {
            auto cell = ws.cell(rowIdx, colIdx);
            if (cell.value().type() == OpenXLSX::XLValueType::Empty) {
                line.emplace_back("");
            } else {
                try {
                    line.emplace_back(cell.value().get<string>());
                } catch (...) {
                    // 对于无法转为字符串的类型，尝试其他方式
                    try {
                        if (cell.value().type() == OpenXLSX::XLValueType::Integer) {
                            line.emplace_back(std::to_string(cell.value().get<int64_t>()));
                        } else if (cell.value().type() == OpenXLSX::XLValueType::Float) {
                            line.emplace_back(std::to_string(cell.value().get<double>()));
                        } else if (cell.value().type() == OpenXLSX::XLValueType::Boolean) {
                            line.emplace_back(cell.value().get<bool>() ? "TRUE" : "FALSE");
                        } else {
                            line.emplace_back("");
                        }
                    } catch (...) {
                        line.emplace_back("");
                    }
                }
            }
        }
        table.emplace_back(std::move(line));
    }
    return table;
}

vector<vector<string>> readFirstSheet(const string &filePath, std::size_t maxRows) {

    OpenXLSX::XLDocument doc;
    doc.open(filePath);
    auto wb = doc.workbook();
    if (wb.worksheetCount() == 0) {
        doc.close();
        throw std::runtime_error("Excel 文件不包含工作表: " + filePath);
    }
    auto ws = wb.worksheet(1); // 1-based index
    auto result = readWorksheet(ws, maxRows);
    doc.close();
    return result;
}

vector<vector<string>> readSheet(const string &filePath, const string &sheetName,
                                 std::size_t maxRows) {

    OpenXLSX::XLDocument doc;
    doc.open(filePath);
    auto wb = doc.workbook();
    if (!wb.worksheetExists(sheetName)) {
        doc.close();
        throw std::runtime_error("Excel 文件不包含工作表: " + sheetName);
    }
    auto ws = wb.worksheet(sheetName);
    auto result = readWorksheet(ws, maxRows);
    doc.close();
    return result;
}

void exampleReadExcelRelative(string filePath) {
    vector<fs::path> candidates;
    candidates.emplace_back(fs::path(filePath));

    fs::path chosen;
    for (const auto &p : candidates) {
        if (fs::exists(p)) {
            chosen = p;
            break;
        }
    }
    if (chosen.empty()) {
        std::ostringstream oss;
        oss << "找不到示例 Excel 文件。尝试路径: \n";
        for (const auto &p : candidates)
            oss << " - " << p.u8string() << "\n";
        throw std::runtime_error(oss.str());
    }

    std::cout << "读取示例文件: " << chosen.u8string() << std::endl;
    auto table = readFirstSheet(chosen.u8string(), /*maxRows=*/0);
    // 打印前几行
    for (const auto &row : table) {
        bool first = true;
        for (const auto &cell : row) {
            if (!first)
                std::cout << "\t";
            std::cout << cell;
            first = false;
        }
        std::cout << "\n";
    }
}
