#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <iomanip>

// Detects OS at compile time
#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <fstream>
    #include <sstream>
#elif defined (__APPLE__)
    #include <sys/sysctl.h>
#endif

#if defined(_WIN32)
    #define PLATFORM_NAME "Windows"
#elif defined(__linux__)
    #define PLATFORM_NAME "Linux"
#elif defined(__APPLE__)
    #define PLATFORM_NAME "macOS"
#else
    #define PLATFORM_NAME "Unknown"
#endif

#if defined(__APPLE__)
    #include <sys/time.h>
    #include <ctime>
#endif

std::string getOSName();
int getCpuCoreCount();
void printMemoryInfo();
void printDiskInfo();
void printUpTime();

int main() {
    std::cout << "---- System Health Report ----\n\n";
    std::cout << "OS: " << getOSName() << "\n";
    std::cout << "Cores: " << getCpuCoreCount() << "\n\n";

    printMemoryInfo();
    std::cout << "\n";
    printDiskInfo();
    std::cout << "\n";
    printUpTime();

    return 0;
}

std::string getOSName() {
    return PLATFORM_NAME;
}

int getCpuCoreCount() {
    return std::thread::hardware_concurrency();
}


void printMemoryInfo() {
    #if defined(_WIN32)
        // Reports total/available physical RAM
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        GlobalMemoryStatusEx(&statex);

        double totalGB = statex.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
        double availGB = statex.ullAvailPhys / (1024.0 * 1024.0 * 1024.0);
        double usedGB = totalGB - availGB;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Memory: " << usedGB << " GB used / " << totalGB << " GB total\n";
    #elif defined (__linux__)
        // /proc/meminfo is a plain text psuedofile
        std::ifstream meminfo("/proc/meminfo");
        std::string key;
        long value;
        std::string unit;
        long totalKB = 0, availKB = 0;

        while (meminfo >> key >> value >> unit) {
            if (key == "MemTotal:") totalKB = value;
            else if (key == "MemAvailable:") availKB = value;
        }

        doulbe totalGB = totalKB / (1024.0 * 1024.0);
        double availGB = availKB / (1024.0 * 1024.0);
        double usedGB = totalGB - availGB;

        std::cout << std::Fixed << std::setprecision(2);
        std::cout << "Memory: " << usedGB << " GB used / " << totalGB << " GB total\n";
    
    #elif defined(__APPLE__)
        // sysctl gives total RAM only, "available" requires the Mach API
        int64_t totalBytes = 0;
        size_t size = sizeof(totalBytes);
        sysctlbyname("hw.memsize", &totalbytes, &size, nullptr, 0);
        double totalGB = totalBytes / (1024.0 * 1024.0 * 1024.0);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Memory: " << totalGB << " GB total (used memory not implemented on mac)\n";
    
    #else
        std::cout << "Memory: unsupported platform\n"
    #endif
}

void printDiskInfo() {
    // Works identically regardless of OS, no need for an ifdef
    std::filesystem::space_info si = std::filesystem::space("/");

    double totalGB = si.capacity / (1024.0 * 1024.0 * 1024.0);
    double freeGB = si.free / (1024.0 * 1024.0 * 1024.0);
    double usedGB = totalGB - freeGB;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Disk (/): " << usedGB << " GB used / " << totalGB << " GB total\n";
}

void printUpTime() {
    #if defined(_WIN32)
    // Milliseconds since boot
        ULONGLONG millis = GetTickCount64();
        long totalSeconds = millis / 1000;
    #elif defined(__linux__)
    // First value in /proc/uptime is seconds since boot
        std::ifstream uptimeFile("/proc/uptime");
        double upSeconds = 0;
        uptimeFile >> upSeconds;
        long totalSeconds = static_cast<long>(upSeconds);

    #elif defined(__APPLE__)
    // kern.boottime gives the boot timestamp. Subtract from now for elapsed seconds
        struct timeval boottime;
        size_t size = sizeof(boottime);
        sysctlbyname("kern.boottime", &boottime, &size, nullptr, 0);
        time_t now = time(nullptr);
        long totalSeconds = now - boottime.tv_sec;

    #else
        long totalSeconds = 0;
    
    #endif
        long hours = totalSeconds / 3600;
        long minutes = (totalSeconds % 3600) / 60;

        std::cout << "Uptime: " << hours << "h " << minutes << "m\n";
}