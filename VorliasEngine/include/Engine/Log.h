#ifndef LOG_H
#define LOG_H

namespace ENGINE_NS {
    void print(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
}

#endif