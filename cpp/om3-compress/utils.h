#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <chrono>
#include <iomanip>

inline int BitScanReverse(size_t n) {
    // size_t 是无符号整数类型，位数取决于平台    
    return n ? (sizeof(size_t) * 8 - 1 - __builtin_clzl(n)) : -1;
}

// Helper function to check if a number is a power of 2
inline bool isPowerOfTwo(size_t n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

/**
 * @brief Finds the smallest power of 2 greater than or equal to n using bit manipulation.
 *
 * @param n The input number.
 * @return The smallest power of 2 >= n. Returns 1 for n = 0.
 *         Returns 0 if n is too large such that the next power of 2 would exceed
 *         the maximum value representable by size_t.
 */
inline size_t nextPowerOfTwo(size_t n)
{
    if (n == 0)
    {
        return 1;
    }

    if (n > SIZE_MAX / 2)
    {
        return SIZE_MAX; // Handle potential overflow
    }

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;                       // Only if size_t is at least 64 bits
#if defined(__LP64__) || defined(_LP64) // Check for 64-bit systems
    n |= n >> 32;                       // Guaranteed to be safe on 64-bit systems.
#endif

    return n + 1;
}

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef double PT;
typedef u64 UT;
typedef i64 ST;

extern u32 BLOCK_SIZE;

u32 ceil(const u32& a, const u32& b);

// 使用 namespace 来组织计时相关的函数和数据，避免污染全局命名空间
namespace Timing {
    // 使用结构体来聚合统计数据
    struct Statistics {
        // 存储每个函数的累计执行时间（以毫秒为单位的 double）
        std::unordered_map<std::string, double> functionTimes;
        // 存储每个函数当前计时的开始时间点
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
        // 存储每个函数被调用的总次数
        std::unordered_map<std::string, size_t> callCounts; // size_t 通常用于计数
    };

    extern Statistics stats;  // Declaration only

    void timeclear();
    void timestart(const std::string& functionName);
    double timeend(const std::string& functionName);
    double getTotalTime(const std::string& functionName);
    size_t getTotalCount(const std::string& functionName);
    double getAvgTime(const std::string& functionName);
    void timetotal(const std::string& functionName);
}
#endif //UTILS_H