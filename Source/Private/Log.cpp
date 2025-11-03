#include "Log.h"
#include "Macros.h"
#include <chrono>

namespace aby::log {

    Logger Logger::s_Logger;

    Logger::Logger() :
        bInCallbacks(false),
        m_Cfg(Config{
            .level = ELevel::ALL,
            .to_console = true,
            .log_files = {},
            .callbacks = {},
            .level_names = {
                { ELevel::NONE,     "<NONE>" },
                { ELevel::TRACE,    "TRACE"  },
                { ELevel::INFO,     "INFO"   },
                { ELevel::WARN,     "WARN"   },
                { ELevel::DEBUG,    "DEBUG"  },
                { ELevel::ERROR,    "ERROR"  },
                { ELevel::ASSERT,   "ASSERT" },
                { ELevel::ALL,      "<ALL>"  },
            },
            .level_colors = {
                { ELevel::NONE,     "<NONE>"     },
                { ELevel::TRACE,    COLOR_WHITE  },
                { ELevel::INFO,     COLOR_GREEN  },
                { ELevel::WARN,     COLOR_YELLOW },
                { ELevel::DEBUG,    COLOR_CYAN   },
                { ELevel::ERROR,    COLOR_RED    },
                { ELevel::ASSERT,   STYLE_UNDERLINE COLOR_RED },
                { ELevel::ALL,      "<ALL>"      },
            }
        }) {}

    auto Logger::get() -> Logger& {
        return s_Logger;
    }

    auto Logger::config() -> Config& {
        return m_Cfg;
    }

    void Logger::write(ELevel level, const std::string& msg) {
        if (static_cast<int>(level) > static_cast<int>(m_Cfg.level)) {
            return;
        }
        
        if (bInCallbacks) {
            write_console(format(ELevel::ASSERT, std::format("File:  {}:{}", ABY_SOURCE_FILE, __LINE__)));
            write_console(format(ELevel::ASSERT, std::format("Func:  {}",  ABY_FUNC_SIG)));
            write_console(format(ELevel::ASSERT, std::format("Expr:  {}", "Logger::bInCallbacks == true")));
            write_console(format(ELevel::ASSERT, std::format("Error: {}", "Cannot call Logger::* functions inside of a callback.\n"
            "Use return mechanism if you need to log an error inside a callback.")));
            std::abort();
        }

        std::scoped_lock lock(m_Mutex);

        auto formatted_msg = format(level, msg);
        write_files(formatted_msg);
        
        if (m_Cfg.to_console) {
            write_console(formatted_msg);
        }

        if (!m_Cfg.callbacks.empty()) {
            Message message;
            message.level     = level;
            message.text      = formatted_msg;
            message.timestamp = current_time();
            handle_callbacks(message);
        }
    }

    void Logger::trace(const std::string& msg) {
        write(ELevel::TRACE, msg);
    }
    
    void Logger::info(const std::string& msg) {
        write(ELevel::INFO, msg);
    }

    void Logger::warn(const std::string& msg) {
        write(ELevel::WARN, msg);
    }

    void Logger::debug(const std::string& msg) {
        write(ELevel::DEBUG, msg);
    }

    void Logger::error(const std::string& msg) {
        write(ELevel::ERROR, msg);
    }

    void Logger::assertion(std::string_view file, int line, const char* func, const char* expr, const std::string& msg) {
        write(ELevel::ASSERT, std::format("File   {}:{}", file, line));
        write(ELevel::ASSERT, std::format("Func:  {}", func));
        write(ELevel::ASSERT, std::format("Expr:  {}", expr));
        write(ELevel::ASSERT, std::format("Error: {}", msg));
    }


    auto Logger::format(ELevel level, const std::string& msg) -> std::string {
        return std::format("[{}] [{}{}{}] {}\n",
            current_time(),
            m_Cfg.level_colors[level],
            m_Cfg.level_names[level],
            COLOR_RESET,
            msg);
    }

    void Logger::write_files(const std::string& formatted_msg) {
        for (auto& path : m_Cfg.log_files) {
            std::ofstream ofs(path, std::ios::app);
            if (ofs.is_open()) {
                ofs << formatted_msg;
                ofs.flush();
            }
        }
    }

    void Logger::write_console(const std::string& formatted_msg) {
        std::fwrite(formatted_msg.c_str(), 1, formatted_msg.size(), stdout);
    }

    void Logger::handle_callbacks(const Message& msg) {
        bInCallbacks = true;
        for (auto& cb : m_Cfg.callbacks) {
            if (auto err = cb(msg); err.has_value()) {
                auto err_msg = format(ELevel::ERROR, err.value());
                write_files(err_msg);
                if (m_Cfg.to_console) {
                    write_console(err_msg);
                }
            }
        }
        bInCallbacks = false;
    }


}

namespace aby::log {
    
    Config& Config::set_level(ELevel level) {
        this->level = level;
        return *this;
    }
    
    Config& Config::set_to_console(bool to_console) {
        this->to_console = to_console;
        return *this;
    }

    Config& Config::set_color(ELevel level, const std::string& color) {
        this->level_colors[level] = color;
        return *this;
    }

    Config& Config::set_level_name(ELevel level, const std::string& name) {
        this->level_names[level] = name;
        return *this;   
    }

    Config& Config::add_file(const std::filesystem::path& path) {
        this->log_files.push_back(path);
        return *this;
    }

    Config& Config::add_callback(Callback&& callback) {
        this->callbacks.push_back(std::forward<decltype(callback)>(callback));
        return *this;
    }

}


namespace aby::log {

    auto current_time() -> std::string {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t t = system_clock::to_time_t(now);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        static const char* months[] = {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec"
        };
        return std::format("{:02}-{}-{:04}•{:02}:{:02}:{:02}",
            tm.tm_mday, months[tm.tm_mon], tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    }


    auto format_with_commas(int64_t value) -> std::string {
        std::string num = std::to_string(value);
        int64_t insert_position = num.length() - 3;
        while (insert_position > 0) {
            num.insert(insert_position, ",");
            insert_position -= 3;
        }
        return num;
    }

}
