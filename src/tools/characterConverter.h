#pragma once
#ifdef _WIN32
#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <windows.h>

// 获取系统临时目录的短路径（纯英文）
inline std::string getSystemTempShortPath() {
    wchar_t tempPath[MAX_PATH];
    DWORD len = GetTempPathW(MAX_PATH, tempPath);
    if (len == 0) {
        throw std::runtime_error("无法获取系统临时目录");
    }

    // 获取临时目录的短路径
    wchar_t shortPath[MAX_PATH];
    DWORD shortLen = GetShortPathNameW(tempPath, shortPath, MAX_PATH);
    if (shortLen == 0) {
        // 如果获取短路径失败，尝试使用原路径
        std::wcerr << L"警告：无法获取临时目录的短路径，使用原路径: " << tempPath << std::endl;
        wcscpy_s(shortPath, MAX_PATH, tempPath);
    }

    // 转换为窄字符
    int mbLen = WideCharToMultiByte(CP_ACP, 0, shortPath, -1, nullptr, 0, nullptr, nullptr);
    std::string result(mbLen - 1, 0);
    WideCharToMultiByte(CP_ACP, 0, shortPath, -1, &result[0], mbLen, nullptr, nullptr);

    return result;
}

// 安全的 Excel 路径处理类
class SafeExcelPath {
  private:
    std::string tempPath;
    bool needsCleanup = false;

    // 检查路径是否包含非ASCII字符
    static bool hasNonASCII(const std::string &path) {
        for (unsigned char c : path) {
            if (c > 127)
                return true;
        }
        return false;
    }

    // 尝试获取短路径
    static std::string tryGetShortPath(const std::string &longPath) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, longPath.c_str(), -1, nullptr, 0);
        if (wlen == 0)
            return "";

        std::wstring wLongPath(wlen - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, longPath.c_str(), -1, &wLongPath[0], wlen);

        // 检查文件是否存在
        if (GetFileAttributesW(wLongPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return "";
        }

        // 尝试获取短路径
        DWORD shortLen = GetShortPathNameW(wLongPath.c_str(), nullptr, 0);
        if (shortLen == 0)
            return "";

        std::wstring wShortPath(shortLen - 1, 0);
        if (GetShortPathNameW(wLongPath.c_str(), &wShortPath[0], shortLen) == 0) {
            return "";
        }

        // 转换回窄字符
        int len =
            WideCharToMultiByte(CP_ACP, 0, wShortPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len == 0)
            return "";

        std::string shortPath(len - 1, 0);
        WideCharToMultiByte(CP_ACP, 0, wShortPath.c_str(), -1, &shortPath[0], len, nullptr,
                            nullptr);

        return shortPath;
    }

  public:
    SafeExcelPath(const std::string &originalPath) {
        namespace fs = std::filesystem;

        // 1. 构建完整的原始路径
        std::string fullPath;
        if (fs::path(originalPath).is_absolute()) {
            fullPath = originalPath;
        } else {
            fullPath = std::string(PROJECT_SOURCE_DIR) + "/" + originalPath;
        }

        // 2. 检查文件是否存在
        if (!fs::exists(fullPath)) {
            throw std::runtime_error("源文件不存在: " + fullPath);
        }

        // 3. 尝试获取短路径（如果成功，直接使用）
        std::string shortPath = tryGetShortPath(fullPath);
        if (!shortPath.empty() && !hasNonASCII(shortPath)) {
            tempPath = shortPath;
            needsCleanup = false;
            // std::cout << "使用短路径: " << tempPath << std::endl;
            return;
        }

        // 4. 如果原路径没有非ASCII字符，直接使用
        if (!hasNonASCII(fullPath)) {
            tempPath = fullPath;
            needsCleanup = false;
            // std::cout << "使用原路径: " << tempPath << std::endl;
            return;
        }

        // 5. 短路径失败或包含中文，使用系统临时目录
        // std::cout << "原路径包含中文，创建临时副本..." << std::endl;

        try {
            // 获取系统临时目录的短路径
            std::string systemTempDir = getSystemTempShortPath();

            // 创建子目录（使用纯英文）
            std::string tempSubDir = systemTempDir + "RIBAST_Excel_Temp\\";
            fs::create_directories(tempSubDir);

            // 生成唯一的临时文件名
            auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
            tempPath = tempSubDir + "tmp_" + std::to_string(timestamp) + ".xlsx";

            // 确保临时路径是纯ASCII
            if (hasNonASCII(tempPath)) {
                throw std::runtime_error("临时路径仍包含非ASCII字符，请检查系统配置");
            }

            // 复制文件
            fs::copy_file(fullPath, tempPath, fs::copy_options::overwrite_existing);
            needsCleanup = true;

            // std::cout << "临时文件创建成功: " << tempPath << std::endl;

        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("创建临时文件失败: ") + e.what());
        }
    }

    ~SafeExcelPath() {
        if (needsCleanup) {
            try {
                if (std::filesystem::exists(tempPath)) {
                    std::filesystem::remove(tempPath);
                    // std::cout << "清理临时文件: " << tempPath << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "清理临时文件失败: " << e.what() << std::endl;
            }
        }
    }

    const std::string &getPath() const { return tempPath; }

    // 禁止拷贝
    SafeExcelPath(const SafeExcelPath &) = delete;
    SafeExcelPath &operator=(const SafeExcelPath &) = delete;
};

#endif