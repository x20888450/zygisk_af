#ifndef YING_LOG_HPP_
#define YING_LOG_HPP_

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <android/log.h>
#include <sys/stat.h>
#include <mutex>
#include <stdarg.h>
#include <ctime>

#define LOG_TAG "樱大牛内部群徒弟"  // 设置你的日志标签
#define LOGD(...) Log::getInstance().logToFile("DEBUG", __VA_ARGS__)
#define LOGW(...) Log::getInstance().logToFile("WARN", __VA_ARGS__)
#define LOGE(...) Log::getInstance().logToFile("ERROR", __VA_ARGS__)
#define LOGI(...) Log::getInstance().logToFile("INFO", __VA_ARGS__)

class Log {
public:
    static Log& getInstance() {
        static Log instance;
        return instance;
    }

    void logToFile(const char* level, const char* format, ...) {
        std::lock_guard<std::mutex> guard(log_mutex);

        va_list args;
        va_start(args, format);

        // 格式化日志消息
        char buffer[1024];
        if (vsnprintf(buffer, sizeof(buffer), format, args) < 0) {
            va_end(args);
            return;  // 如果格式化失败，提前返回
        }

        va_end(args);

        // 获取当前文件大小
        struct stat stat_buf;
        if (stat(log_file.c_str(), &stat_buf) == 0 && stat_buf.st_size > max_file_size) {
            // 如果文件超过最大大小则清空文件
            log_stream.close();
            log_stream.open(log_file, std::ofstream::out | std::ofstream::trunc);
        }

        // 获取当前时间
        std::time_t now = std::time(nullptr);
        char time_buffer[20];  // "YYYY-MM-DD HH:MM:SS" 格式的时间
        std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        // 将日志消息写入文件
        log_stream << "[" << time_buffer << "] [" << level << "] " << buffer << std::endl;
        log_stream.flush();

        // 同时将日志打印到Android日志
        // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s] %s", level, buffer);
    }

private:
    Log() {
        log_file = "/storage/emulated/0/Android/data/com.tencent.af/files/log.txt";  // 设置你的日志文件路径
        log_stream.open(log_file, std::ofstream::out | std::ofstream::app);
        max_file_size = 10 * 1024 * 1024; // 10 MB
    }

    ~Log() {
        if (log_stream.is_open()) {
            log_stream.close();
        }
    }

    std::string log_file;
    std::ofstream log_stream;
    std::mutex log_mutex;
    size_t max_file_size;
};

#endif // YING_LOG_HPP_
