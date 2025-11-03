#pragma once

#include "Types.h"
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <span>
#include <map>
#include <functional>

namespace aby::log {

    enum class ELevel {
        NONE,
        TRACE,
        INFO,
        WARN,
        DEBUG,
        ERROR,
        ASSERT,
        ALL,
    };

    struct Message {
        std::string timestamp;
        std::string text;
        ELevel      level = ELevel::NONE;
    };

    /**
    * @brief  Callback when message is logged.
    * @param  Message: message that got logged, already formatted. contains ansii codes.
    * @return Optional error message if something went wrong during callback.
    * @throws DO NOT CALL LOGGER FUNCTIONS IN CALLBACK!
    */
    using Callback = std::function<std::optional<std::string>(const Message&)>;

    struct Config {
        Config& set_level(ELevel level);
        Config& set_to_console(bool to_console);
        Config& set_color(ELevel level, const std::string& color);
        Config& set_level_name(ELevel level, const std::string& name);
        Config& add_file(const fs::path& path);
        Config& add_callback(Callback&& callback);

        ELevel                        level      = ELevel::ALL;                       
        bool                          to_console = true;
        std::vector<fs::path>         log_files;                            
        std::vector<Callback>         callbacks; // Do not make logger calls inside callback.
        std::map<ELevel, std::string> level_names;
        std::map<ELevel, std::string> level_colors;
    };

    class Logger {
    public:
        Logger();
        ~Logger() = default; 

        static auto get() -> Logger&;
        auto config() -> Config&;

        void write(ELevel level, const std::string& msg);
        void trace(const std::string& msg);
        void info(const std::string& msg);
        void warn(const std::string& msg);
        void debug(const std::string& msg);
        void error(const std::string& msg);
        void assertion(const char* file, int line, const char* func, const char* expr, const std::string& msg);
    private:
        auto format(ELevel level, const std::string& msg) -> std::string;
        void write_files(const std::string& formatted_msg);
        void write_console(const std::string& formatted_msg);
        void handle_callbacks(const Message& msg);
    private:
        Config     m_Cfg;
        std::mutex m_Mutex;
        bool       bInCallbacks;
    private:
        static Logger s_Logger;
    }; 

    auto current_time() -> std::string;
    auto format_with_commas(int64_t value) -> std::string;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[ ";
    for (auto it = v.begin(); it != v.end(); ++it) {
        os << '\'' << *it << '\'';
        if (std::next(it) != v.end()) os << ", ";
    }
    os << " ]";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::span<T>& s) {
    os << "[ ";
    for (auto it = s.begin(); it != s.end(); ++it) {
        os << '\'' << *it << '\'';
        if (std::next(it) != s.end()) os << ", ";
    }
    os << " ]";
    return os;
}
