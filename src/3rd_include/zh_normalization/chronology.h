#ifndef CHRONOLOGY_H
#define CHRONOLOGY_H

#include <regex>
#include <string>
#include <map>

#include "num.h"

class Chronology {
public:
    // 静态处理函数
    static std::string time_num2str(const std::string& num_string);
    static std::string replace_time(const std::smatch& match);
    static std::string replace_date(const std::smatch& match);
    static std::string replace_date2(const std::smatch& match);

    // 静态正则表达式
    static inline auto RE_TIME = std::regex(R"(([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?)");
    static inline auto RE_TIME_RANGE = std::regex(R"(([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?(~|-)([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?)");
    // static inline auto RE_DATE =  std::regex(R"(((\d{4}|\d{2})年)?((0?[1-9]|1[0-2])月)?(((0?[1-9])|((1|2)[0-9])|30|31)([日号]))?)");
    static inline auto RE_DATE =     std::regex(R"(((\d{2,4})年)?(([1-9]|1[0-2])月)?(((0?[1-9])|([12][0-9])|30|31)(日|号))?)");
    static inline auto RE_DATE2 = std::regex(R"((\d{4})([-/.])(0[1-9]|1[012])\2(0[1-9]|[12][0-9]|3[01]))");
};

#endif //CHRONOLOGY_H 