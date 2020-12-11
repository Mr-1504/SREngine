//
// Created by Nikita on 16.11.2020.
//

#ifndef HELPER_DEBUG_H
#define HELPER_DEBUG_H

#include <fstream>
#include <ostream>
#include <mutex>
#include <Windows.h>

namespace Framework::Helper {
    class Debug {
    public:
        enum class Level {
            None = 0, Low = 1, Medium = 2, High = 3, Full = 4
        };

        enum class Theme {
            Dark, Light
        };

        enum class ConsoleColor {
            Black = 0,
            Blue = 1,
            Green = 2,
            Cyan = 3,
            Red = 4,
            Magenta = 5,
            Brown = 6,
            LightGray = 7,
            DarkGray = 8,
            LightBlue = 9,
            LightGreen = 10,
            LightCyan = 11,
            LightRed = 12,
            LightMagenta = 13,
            Yellow = 14,
            White = 15
        };
        enum class Type {
            Log, Info, Debug, Graph, Shader, Script, System, Warn, Error
        };
    private:
        Debug() {};
        ~Debug() {};
        Debug(Debug &) = delete;
    private:
        inline static Theme             g_theme                     = Theme::Light;
        inline static std::mutex        g_mutex                     = std::mutex();
        inline static HANDLE            g_console                   = nullptr;
        inline static bool              g_showUseMemory             = false;
        inline static bool              g_ColorThemeIsEnabled       = false;
        inline static volatile bool     g_isInit                    = false;
        inline static std::string       g_log_path                  = "";
        inline static std::ofstream     g_file                      = std::ofstream();
        static inline Level             g_level                     = Level::Low;
    private:
        static inline void InitColorTheme();

        static void Print(std::string &msg, Type type);
    public:
        inline static const Level GetLevel() noexcept { return g_level; }
        static const void SetLevel(Level level) { g_level = level; }

        static void Init(const std::string& log_path, bool ShowUsedMemory, Theme colorTheme = Theme::Light);
        static int Stop();
    public:
        static void Log(std::string msg)    { Print(msg, Type::Log);    }
        static void Info(std::string msg)   { Print(msg, Type::Info);   }
        static void Graph(std::string msg)  { Print(msg, Type::Graph);  }
        static void Shader(std::string msg) { Print(msg, Type::Shader); }
        static void Script(std::string msg) { Print(msg, Type::Script); }
        static void System(std::string msg) { Print(msg, Type::System); }
        static void Warn(std::string msg)   { Print(msg, Type::Warn);   }
        static void Error(std::string msg)  { Print(msg, Type::Error);  }
    };
}

#endif //HELPER_DEBUG_H
