#include <zh_normalization/chronology.h>
#include <iostream>

std::string Chronology::time_num2str(const std::string& num_string) {
    // 去除前导零后转换
    size_t start_pos = num_string.find_first_not_of('0');
    std::string stripped;
    if (start_pos != std::string::npos) {
        stripped = num_string.substr(start_pos);
    } else {
        stripped = "";
    }

    std::string result = Num::num2str(stripped.empty() ? "0" : stripped);

    // 如果原始数字以0开头，添加"零"
    if (!num_string.empty() && num_string[0] == '0') {
        result = Num::DIGITS.at("0") + result;
    }

    return result;
}

std::string Chronology::replace_time(const std::smatch& match) {
    bool is_range = match.size() > 6;  // 判断是否为时间范围

    std::string hour = match[1].str();
    std::string minute = match[2].str();
    std::string second = match[4].str();

    std::string hour_2, minute_2, second_2;
    if (is_range) {
        hour_2 = match[6].str();
        minute_2 = match[7].str();
        second_2 = match[9].str();
    }

    std::string result = Num::num2str(hour) + "点";

    // 处理分钟
    if (!minute.empty()) {
        size_t start_pos = minute.find_first_not_of('0');
        std::string stripped_minute;
        if (start_pos != std::string::npos) {
            stripped_minute = minute.substr(start_pos);
        } else {
            stripped_minute = "";
        }

        if (!stripped_minute.empty()) {
            if (std::stoi(minute) == 30) {
                result += "半";
            } else {
                result += time_num2str(minute) + "分";
            }
        }
    }

    // 处理秒数
    if (!second.empty()) {
        size_t start_pos = second.find_first_not_of('0');
        std::string stripped_second;
        if (start_pos != std::string::npos) {
            stripped_second = second.substr(start_pos);
        } else {
            stripped_second = "";
        }

        if (!stripped_second.empty()) {
            result += time_num2str(second) + "秒";
        }
    }

    // 如果是时间范围，处理第二部分
    if (is_range) {
        result += "至";
        result += Num::num2str(hour_2) + "点";

        if (!minute_2.empty()) {
            size_t start_pos = minute_2.find_first_not_of('0');
            std::string stripped_minute;
            if (start_pos != std::string::npos) {
                stripped_minute = minute_2.substr(start_pos);
            } else {
                stripped_minute = "";
            }

            if (!stripped_minute.empty()) {
                if (std::stoi(minute_2) == 30) {
                    result += "半";
                } else {
                    result += time_num2str(minute_2) + "分";
                }
            }
        }

        if (!second_2.empty()) {
            size_t start_pos = second_2.find_first_not_of('0');
            std::string stripped_second;
            if (start_pos != std::string::npos) {
                stripped_second = second_2.substr(start_pos);
            } else {
                stripped_second = "";
            }

            if (!stripped_second.empty()) {
                result += time_num2str(second_2) + "秒";
            }
        }
    }

    return result;
}

std::string Chronology::replace_date(const std::smatch& match) {
    // xx年
    std::string year = match[1].str();
    // xx月
    std::string month = match[3].str();
    // xx[日号]
    std::string day = match[6].str();

    std::string tmp = match[9].str();

    std::string result;

    if (!year.empty()) {
        year = Num::get_digit(year);
        result += Num::verbalize_digit(year) + "年";
    }

    if (!month.empty()) {
        month = Num::get_digit(month);
        result += Num::verbalize_cardinal(month) + "月";
    }

    if (!day.empty()) {
        day = Num::get_digit(day);
        result += Num::verbalize_cardinal(day) + match[9].str(); // 日或号
    }

    return result;
}

std::string Chronology::replace_date2(const std::smatch& match) {
    std::string year = match[1].str();
    std::string month = match[3].str();
    std::string day = match[4].str();

    std::string result = "";

    if (!year.empty()) {
        result += Num::verbalize_digit(year) + "年";
    }

    if (!month.empty()) {
        result += Num::verbalize_cardinal(month) + "月";
    }

    if (!day.empty()) {
        result += Num::verbalize_cardinal(day) + "日";
    }

    return result;
}