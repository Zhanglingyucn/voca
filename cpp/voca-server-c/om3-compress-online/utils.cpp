#include "utils.h"
#include "alp.hpp"
#include <iostream>

u32 BLOCK_SIZE = alp::config::VECTOR_SIZE;
u32 MAX_THREADS;

u32 ceil(const u32& a, const u32& b)
{
    return a ? (a - 1) / b + 1 : 0;
}

namespace Timing {
    // 创建一个全局（在命名空间内）的统计数据实例
    Statistics stats;

    // 清除所有统计数据
    void timeclear() {
        stats.functionTimes.clear();
        stats.startTimes.clear();
        stats.callCounts.clear();
    }

    // 开始计时函数
    // 使用 const std::string& 避免不必要的字符串复制
    void timestart(const std::string& functionName) {
        stats.startTimes[functionName] = std::chrono::high_resolution_clock::now();
    }

    // 结束计时函数
    // 返回本次调用的耗时（毫秒）
    double timeend(const std::string& functionName) {
        // 检查是否存在对应的开始时间记录
        auto it = stats.startTimes.find(functionName);
        if (it == stats.startTimes.end()) {
            // 使用 std::cerr 输出错误信息到标准错误流
            std::cerr << "Error: No start time recorded for " << functionName << std::endl;
            return 0.0; // 或者可以抛出异常，这里为了简单返回0
        }

        // 获取开始时间并记录结束时间
        const auto startTime = it->second;
        const auto endTime = std::chrono::high_resolution_clock::now();

        // 计算时间差，并转换为毫秒 (double 类型)
        std::chrono::duration<double, std::milli> timeSpentDuration = endTime - startTime;
        double timeSpentMs = timeSpentDuration.count();

        // 更新总时间 (如果 functionName 不存在，operator[] 会默认初始化为 0)
        stats.functionTimes[functionName] += timeSpentMs;

        // 更新调用次数 (如果 functionName 不存在，operator[] 会默认初始化为 0)
        stats.callCounts[functionName]++; // 直接递增

        // 清除本次计时的开始时间记录 (使用迭代器删除效率更高)
        stats.startTimes.erase(it);

        return timeSpentMs;
    }

    // 获取指定函数的总执行时间（单位：秒）
    double getTotalTime(const std::string& functionName) {
        auto it = stats.functionTimes.find(functionName);
        if (it != stats.functionTimes.end()) {
            return it->second / 1000.0; // 从毫秒转换为秒
        }
        return 0.0; // 如果没有记录，返回 0
    }

    // 获取指定函数的总调用次数
    size_t getTotalCount(const std::string& functionName) {
        auto it = stats.callCounts.find(functionName);
        if (it != stats.callCounts.end()) {
            return it->second;
        }
        return 0; // 如果没有记录，返回 0
    }

    // 获取指定函数的平均执行时间（单位：秒）
    double getAvgTime(const std::string& functionName) {
        auto timeIt = stats.functionTimes.find(functionName);
        auto countIt = stats.callCounts.find(functionName);

        // 确保时间和次数都有记录
        if (timeIt != stats.functionTimes.end() && countIt != stats.callCounts.end()) {
            double totalTimeMs = timeIt->second;
            size_t count = countIt->second;
            if (count > 0) {
                // 先转换为秒，再除以次数
                return (totalTimeMs / 1000.0) / count;
            } else {
                return 0.0; // 避免除以零
            }
        } else {
            // 保持与 JS 版本一致的输出（虽然在 C++ 中返回 NaN 或抛异常可能更常见）
            std::cout << "No timing data for " << functionName << std::endl;
            return 0.0;
        }
    }


    // 输出指定函数的统计数据（总时间毫秒，调用次数）
    void timetotal(const std::string& functionName) {
        auto timeIt = stats.functionTimes.find(functionName);
        auto countIt = stats.callCounts.find(functionName);

        // 确保时间和次数都有记录
        if (timeIt != stats.functionTimes.end() && countIt != stats.callCounts.end()) {
            double totalTimeMs = timeIt->second;
            size_t count = countIt->second;
            // 使用 iomanip 来格式化输出，保留两位小数，与 JS 的 toFixed(2) 类似
            std::cout << "Total time for " << functionName << ": "
                      << std::fixed << std::setprecision(2) << totalTimeMs << " ms, called "
                      << count << " times" << std::endl;
        } else {
            std::cout << "No timing data for " << functionName << std::endl;
        }
    }

} // namespace Timing