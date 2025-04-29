//
// Created by tao on 25-4-9.
//

#include <zh_normalization/num.h>

// 静态成员变量定义
const std::map<int, std::string> Num::UNITS = {
    {1, "十"},
    {2, "百"},
    {3, "千"},
    {4, "万"},
    {8, "亿"}
};

const std::map<std::string, std::string> Num::DIGITS = {
    {"0", "零"}, {"1", "一"}, {"2", "二"}, {"3", "三"}, {"4", "四"},
    {"5", "五"}, {"6", "六"}, {"7", "七"}, {"8", "八"}, {"9", "九"}
};

const std::string Num::COM_QUANTIFIERS = "(封|艘|把|目|套|段|人|所|朵|匹|张|座|回|场|尾|条|个|首|阙|阵|网|炮|顶|丘|棵|只|支|袭|辆|挑|担|颗|壳|窠|曲|墙|群|腔|砣|座|客|贯|扎|捆|刀|令|打|手|罗|坡|山|岭|江|溪|钟|队|单|双|对|出|口|头|脚|板|跳|枝|件|贴|针|线|管|名|位|身|堂|课|本|页|家|户|层|丝|毫|厘|分|钱|两|斤|担|铢|石|钧|锱|忽|(千|毫|微)克|毫|厘|(公)分|分|寸|尺|丈|里|寻|常|铺|程|(千|分|厘|毫|微)米|米|撮|勺|合|升|斗|石|盘|碗|碟|叠|桶|笼|盆|盒|杯|钟|斛|锅|簋|篮|盘|桶|罐|瓶|壶|卮|盏|箩|箱|煲|啖|袋|钵|年|月|日|季|刻|时|周|天|秒|分|小时|旬|纪|岁|世|更|夜|春|夏|秋|冬|代|伏|辈|丸|泡|粒|颗|幢|堆|条|根|支|道|面|片|张|颗|块|元|(亿|千万|百万|万|千|百)|(亿|千万|百万|万|千|百|美|)元|(亿|千万|百万|万|千|百|十|)吨|(亿|千万|百万|万|千|百|)块|角|毛|分|(公(里|引|丈|尺|寸|分|釐)))";

// 方法实现
std::string Num::num2str(const std::string& value_string) {
    size_t dot_pos = value_string.find('.');
    std::string integer, decimal;

    if (dot_pos == std::string::npos) {
        integer = value_string;
        decimal = "";
    } else {
        integer = value_string.substr(0, dot_pos);
        decimal = value_string.substr(dot_pos + 1);
    }

    std::string result = verbalize_cardinal(integer);

    // 去除decimal末尾的0
    while (!decimal.empty() && decimal.back() == '0') {
        decimal.pop_back();
    }

    if (!decimal.empty()) {
        // '.22' is verbalized as '零点二二'
        // '3.20' is verbalized as '三点二'
        if (result.empty()) {
            result = "零";
        }
        result += "点" + verbalize_digit(decimal);
    }

    return result;
}

// 分数表达式
std::string Num::replace_frac(const std::smatch& match) {
    std::string sign = match[1].str();
    std::string nominator = match[2].str();
    std::string denominator = match[3].str();
    sign = sign.empty() ? "" : "负";
    nominator = num2str(nominator);
    denominator = num2str(denominator);
    return sign + denominator + "分之" + nominator;
}

std::string Num::replace_percentage(const std::smatch& match) {
    bool is_negative = !match[1].str().empty();  // Check if '-' exists
    std::string integer_part = match[2].str();   // Main number part
    std::string decimal_part = match[3].matched ? match[3].str() : "";  // Decimal part if exists

    std::string result;
    if (is_negative) {
        result += "负";
    }
    result += "百分之" + num2str(integer_part + decimal_part);
    return result;
}

// 处理负数
std::string Num::replace_negative_num(const std::smatch& match) {
    std::string sign = match[1].str();
    std::string number = match[2].str();
    sign = sign.empty() ? "" : "负";
    number = num2str(number);
    return sign + number;
}

// 编号-无符号整形
std::string Num::replace_default_num(const std::smatch& match) {
    std::string number = match[0].str();
    return verbalize_digit(number, true);
}

// 正整数 + 量词
std::string Num::replace_positive_quantifier(const std::smatch& match) {
    std::string number = match[1].str();
    std::string match_2 = match[2].str();
    if (match_2 == "+") {
        match_2 = "多";
    }
    std::string quantifiers = match[3].str();
    number = num2str(number);
    return number + match_2 + quantifiers;
}

// 数字表达式
std::string Num::replace_number(const std::smatch& match) {
    std::string sign = match[1].str();
    std::string number = match[2].str();
    std::string pure_decimal = match[5].str();

    if (!pure_decimal.empty()) {
        return num2str(pure_decimal);
    } else {
        sign = sign.empty() ? "" : "负";
        number = num2str(number);
        return sign + number;
    }
}

// 范围表达式
std::string Num::replace_range(const std::smatch& match) {
    std::string first = match[1].str();
    std::string second = match[8].str();

    // 使用正则替换first和second
    std::regex re_number(R"((-?)((\d+)(\.\d+)?)|(\.(\d+)))");

    // 使用迭代器方式处理first字符串
    std::string first_result;
    std::sregex_iterator first_it(first.begin(), first.end(), re_number);
    std::sregex_iterator end;

    size_t last_pos = 0;
    while (first_it != end) {
        std::smatch match = *first_it;
        // 添加未匹配部分
        first_result += first.substr(last_pos, match.position() - last_pos);
        // 添加转换后的数字
        first_result += replace_number(match);
        last_pos = match.position() + match.length();
        ++first_it;
    }
    // 添加剩余部分
    first_result += first.substr(last_pos);

    // 使用相同方式处理second字符串
    std::string second_result;
    std::sregex_iterator second_it(second.begin(), second.end(), re_number);

    last_pos = 0;
    while (second_it != end) {
        std::smatch match = *second_it;
        second_result += second.substr(last_pos, match.position() - last_pos);
        second_result += replace_number(match);
        last_pos = match.position() + match.length();
        ++second_it;
    }
    second_result += second.substr(last_pos);

    return first_result + "到" + second_result;
}

inline std::vector<std::string> Num::_get_value(const std::string& value_string, bool use_zero) {
    // 去除前导零
    size_t start_pos = value_string.find_first_not_of('0');
    std::string stripped;
    if (start_pos != std::string::npos) {
        stripped = value_string.substr(start_pos);
    } else {
        stripped = "";
    }

    if (stripped.empty()) {
        return {};
    } else if (stripped.length() == 1) {
        if (use_zero && stripped.length() < value_string.length()) {
            return {DIGITS.at("0"), DIGITS.at(stripped)};
        } else {
            return {DIGITS.at(stripped)};
        }
    } else {
        // 找到最大单位
        int largest_unit = 0;
        for (auto it = UNITS.rbegin(); it != UNITS.rend(); ++it) {
            if (it->first < stripped.length()) {
                largest_unit = it->first;
                break;
            }
        }

        std::string first_part = value_string.substr(0, value_string.length() - largest_unit);
        std::string second_part = value_string.substr(value_string.length() - largest_unit);

        std::vector<std::string> result = _get_value(first_part);
        result.push_back(UNITS.at(largest_unit));

        std::vector<std::string> second_result = _get_value(second_part);
        result.insert(result.end(), second_result.begin(), second_result.end());

        return result;
    }
}

std::string Num::verbalize_cardinal(const std::string& value_string) {
    if (value_string.empty()) {
        return "";
    }

    // 去除前导零
    size_t start_pos = value_string.find_first_not_of('0');
    std::string stripped;
    if (start_pos != std::string::npos) {
        stripped = value_string.substr(start_pos);
    } else {
        stripped = "";
    }

    // 000 -> '零' , 0 -> '零'
    if (stripped.empty()) {
        return DIGITS.at("0");
    }

    std::vector<std::string> result_symbols = _get_value(stripped);

    // verbalized number starting with '一十*' is abbreviated as `十*`
    if (result_symbols.size() >= 2 && result_symbols[0] == DIGITS.at("1") && result_symbols[1] == UNITS.at(1)) {
        result_symbols.erase(result_symbols.begin());
    }

    std::string result;
    for (const auto& s : result_symbols) {
        result += s;
    }

    return result;
}

std::string Num::verbalize_digit(const std::string& value_string, bool alt_one) {
    std::string result;
    for (char digit : value_string) {
        std::string digit_str(1, digit);
        result += DIGITS.at(digit_str);
    }

    if (alt_one) {
        // 将"一"替换为"幺"
        size_t pos = 0;
        while ((pos = result.find("一", pos)) != std::string::npos) {
            result.replace(pos, 3, "幺");  // UTF-8中"一"和"幺"都是3字节
            pos += 3;
        }
    }

    return result;
}