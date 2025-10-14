#ifdef __WIN32
#include "../tools/characterConverter.h"
#include <cstdlib>
#endif
#include "../dataLoader/excelLoader.h"
#include <OpenXLSX.hpp>
#include <string>

using namespace OpenXLSX;

int main() {

#ifdef __WIN32
    system("chcp 65001");
#endif

    std::string filePath = "data/干员练度表.xlsx";

#ifdef __WIN32
    SafeExcelPath sep(filePath);
    filePath = sep.getPath();
#endif

    exampleReadExcelRelative(filePath);

    return 0;
}