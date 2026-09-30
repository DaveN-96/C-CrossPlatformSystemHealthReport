# C++ System Health Report

Basic system health info reporting tool with cross-platform functionality.

## Reports
Lists:
OS name
CPU core count
Memory usage / total
Disk space usage / total
System uptime

## Build
Requires CMake and a C++ compiler.

````powershell
cmake -B build
cmake --build build
./build/Debug/sysinfo.exe     #For windows
./build/sysinfo     #For Linux/macOS
