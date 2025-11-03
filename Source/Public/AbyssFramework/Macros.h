#pragma once

#include <string_view>

// =============================================================
// Platform Detection
// =============================================================
#ifndef _WIN32
#   if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#       define POSIX
#   endif // defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#endif // !(_WIN32)
// =============================================================
// Debug Break Macro
// =============================================================
#ifndef ABY_DBG_BREAK
// ---- MSVC compiler ----
#   ifdef _MSC_VER
#       define ABY_DBG_BREAK() __debugbreak();
// ---- Clang/GCC with builtins ----
#   elif defined(__has_builtin)
#       if __has_builtin(__builtin_debugtrap)
#           define ABY_DBG_BREAK() __builtin_debugtrap()
#       elif __has_builtin(__builtin_trap)
#           define ABY_DBG_BREAK() __builtin_trap()
#       endif // __has_builtin(__builtin_debugtrap) / __has_builtin(__builtin_trap)
// ---- Generic POSIX fallback ----
#   elif defined(POSIX)
extern "C" int raise(int sig);
#       define ABY_DBG_BREAK() raise(SIGTRAP)
// ---- Windows non-MSVC fallback ----
#   elif defined(_WIN32)
extern "C" __declspec(dllimport) void __stdcall DebugBreak();
#       define ABY_DBG_BREAK() DebugBreak()
// ---- Ultimate fallback (non-standard system pause + abort) ----
#   else
#       define ABY_DBG_BREAK() std::system("pause"); std::abort(-1);
#   endif // platform selection branches

#endif // !(ABY_DBG_BREAK)
// =============================================================
// Function Signature & File Macro 
// =============================================================
#ifndef ABY_FUNC_SIG
// ---- GCC / Clang ----
#   ifdef __GNUC__
#       define ABY_FUNC_SIG __PRETTY_FUNCTION__
// ---- MSVC ----
#   elif defined(_MSC_VER)
#       define ABY_FUNC_SIG __FUNCSIG__
// ---- Other compilers ----
#   else
// Intel / IBM compilers
#       if (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || \
(defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#           define ABY_FUNC_SIG __FUNCTION__
// Borland compiler
#       elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#           define ABY_FUNC_SIG __FUNC__
// Generic fallback (C99 standard)
#       else
#           define ABY_FUNC_SIG __func__
#       endif // compiler-specific fallbacks
#   endif // __GNUC__ / _MSC_VER / others
#endif // !(ABY_FUNC_SIG)

#ifdef __FILE__
#   define ABY_SOURCE_FILE std::string_view(__FILE__).substr(std::string_view(__FILE__).find_last_of("/\\") + 1)
#else  // !(__FILE__)
#   define ABY_SOURCE_FILE "<No Source Info>"
#endif // __FILE__
// =============================================================
// Other Macro Helpers
// =============================================================
#ifndef NDEBUG
#   define ABY_IF_DBG(x, el) x 
#else // !(IF_DBG)
#   define ABY_IF_DBG(x, el) el
#endif // 
// =============================================================
// ANSI Text Colors
// =============================================================
#define COLOR_RESET   "\033[0m"   
#define COLOR_WHITE   "\033[37m"  
#define COLOR_GREEN   "\033[32m"  
#define COLOR_YELLOW  "\033[33m"  
#define COLOR_RED     "\033[31m"  
#define COLOR_CYAN    "\033[36m"  
// =============================================================
// Text Styles
// =============================================================
#define STYLE_UNDERLINE "\033[4m"
// =============================================================
// Logging Macros
// =============================================================
#define log_trace(fmt, ...) ::aby::log::Logger::get().trace(std::format(fmt __VA_OPT__(,) __VA_ARGS__ ))
#define log_info(fmt, ...)  ::aby::log::Logger::get().info(std::format(fmt __VA_OPT__(,) __VA_ARGS__ ))
#define log_warn(fmt, ...)  ::aby::log::Logger::get().warn(std::format(fmt __VA_OPT__(,) __VA_ARGS__ ))
#define log_err(fmt, ...)   ::aby::log::Logger::get().error(std::format(fmt __VA_OPT__(,) __VA_ARGS__ ))
#define log_dbg(fmt, ...)   ABY_IF_DBG(::aby::log::Logger::get().debug(std::format(fmt __VA_OPT__(,) __VA_ARGS__ )), )
#define log_assert(condition, fmt, ...)                                                                                                          \
    ABY_IF_DBG(do {                                                                                                                              \
        if (!(condition)) {                                                                                                                      \
            ::aby::log::Logger::get().assertion(ABY_SOURCE_FILE, __LINE__, ABY_FUNC_SIG, #condition, std::format(fmt __VA_OPT__(,) __VA_ARGS__)) \
            ABY_DBG_BREAK();                                                                                                                     \
        }                                                                                                                                        \
    } while(0), condition;)