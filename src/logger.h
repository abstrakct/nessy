
// Convenience functions for logging stuff

#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

#define LOG_LEVEL_NOP   0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 2

#define LOG_LEVEL LOG_LEVEL_NOP

class Logger {
    private:
        std::ofstream f;
        std::string filename;
    public:
        Logger()
        {
            std::time_t t = std::time(nullptr);
            filename = "log/log-nessy-" + std::to_string(t) + ".txt";
            f = std::ofstream(filename);
        };

        ~Logger()
        {
            f.close();
        };

        // write
        void w(std::string s)
        {
#if LOG_LEVEL > LOG_LEVEL_NOP
            f << s << std::endl;
#endif
        };

        // write debug level
        void wd(std::string s)
        {
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
            f << s;
#endif
        };
};
