//
// Created by tao on 25-4-9.
//

#ifndef NUM_H
#define NUM_H

/**
 * Rules to verbalize numbers into Chinese characters.
 * https://zh.wikipedia.org/wiki/中文数字#現代中文
 */

#include <string>
#include <map>
#include <vector>
#include <regex>

class Num {
public:
    // 静态成员变量
    static const std::map<int, std::string> UNITS;
    static const std::map<std::string, std::string> DIGITS;
    static const std::string COM_QUANTIFIERS;

    // 辅助方法
    static std::vector<std::string> _get_value(const std::string& value_string, bool use_zero = true);
    static std::string verbalize_cardinal(const std::string& value_string);
    static std::string verbalize_digit(const std::string& value_string, bool alt_one = false);

    // 公共方法
    static std::string num2str(const std::string& value_string);
    static std::string replace_frac(const std::smatch& match);
    static std::string replace_percentage(const std::smatch& match);
    static std::string replace_negative_num(const std::smatch& match);
    static std::string replace_default_num(const std::smatch& match);
    static std::string replace_positive_quantifier(const std::smatch& match);
    static std::string replace_number(const std::smatch& match);
    static std::string replace_range(const std::smatch& match);
};


// 示例用法
/*
int main() {
    std::string test = "123.45";
    std::cout << ChineseNumberConverter::num2str(test) << std::endl;
    return 0;
}
*/
#endif //NUM_H
