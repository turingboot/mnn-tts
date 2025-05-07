//
// Created by tao on 25-4-9.
//

#include "zh_normalization/TextNormalizer.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <regex>
#include <codecvt>
#include <locale>
#include "zh_normalization/chinese_converter.h"
#include "zh_normalization/chronology.h"
#include "zh_normalization/num.h"
#include "zh_normalization/phonecode.h"
#include "zh_normalization/quantifier.h"

TextNormalizer::TextNormalizer() : SENTENCE_SPLITOR(R"(([：、，；。？！,;?!]["']?))") {}

std::vector<std::string> TextNormalizer::_split(const std::string& text, const std::string& lang) const {
    std::string processed_text = text;

    if (lang == "zh") {
        // 移除空格
        processed_text.erase(
            std::remove(processed_text.begin(), processed_text.end(), ' '),
            processed_text.end()
        );

        // 过滤特殊字符
        std::regex special_chars(R"([——《》【】<=>{}()（）#&@""^_|…\\])");
        processed_text = std::regex_replace(processed_text, special_chars, "");
    }

    // 分句处理
    processed_text = std::regex_replace(processed_text, SENTENCE_SPLITOR, "$1\n");

    std::vector<std::string> sentences;
    std::stringstream ss(processed_text);
    std::string sentence;

    while (std::getline(ss, sentence)) {
        if (!sentence.empty()) {
            // 去除首尾空白
            while (!sentence.empty() && std::isspace(sentence.front()))
                sentence.erase(0, 1);
            while (!sentence.empty() && std::isspace(sentence.back()))
                sentence.pop_back();
            if (!sentence.empty())
                sentences.push_back(sentence);
        }
    }

    return sentences;
}

std::string TextNormalizer::_post_replace(const std::string& sentence) {
    std::string result = sentence;

    static const std::vector<std::pair<std::string, std::string>> replacements = {
        {"/", "每"},
        {"~", "至"},
        {"～", "至"},
        {"①", "一"},
        {"②", "二"},
        {"③", "三"},
        {"④", "四"},
        {"⑤", "五"},
        {"⑥", "六"},
        {"⑦", "七"},
        {"⑧", "八"},
        {"⑨", "九"},
        {"⑩", "十"},
        {"α", "阿尔法"},
        {"β", "贝塔"},
        {"γ", "伽玛"},
        {"Γ", "伽玛"},
        {"δ", "德尔塔"},
        {"Δ", "德尔塔"},
        {"ε", "艾普西龙"},
        {"ζ", "捷塔"},
        {"η", "依塔"},
        {"θ", "西塔"},
        {"Θ", "西塔"},
        {"ι", "艾欧塔"},
        {"κ", "喀帕"},
        {"λ", "拉姆达"},
        {"Λ", "拉姆达"},
        {"μ", "缪"},
        {"ν", "拗"},
        {"ξ", "克西"},
        {"Ξ", "克西"},
        {"ο", "欧米克伦"},
        {"π", "派"},
        {"Π", "派"},
        {"ρ", "肉"},
        {"ς", "西格玛"},
        {"Σ", "西格玛"},
        {"σ", "西格玛"},
        {"τ", "套"},
        {"υ", "宇普西龙"},
        {"φ", "服艾"},
        {"Φ", "服艾"},
        {"χ", "器"},
        {"ψ", "普赛"},
        {"Ψ", "普赛"},
        {"ω", "欧米伽"},
        {"Ω", "欧米伽"}
    };

    for (const auto& [from, to] : replacements) {
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
    }

    // TODO 这里不转换wstring 正则后会出现乱码 先临时处理
    // UTF-8 转宽字符（UTF-32）
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring w_input = converter.from_bytes(result);

    // 定义宽字符正则表达式
    std::wregex special_chars(LR"([\-《》【】<=>\{\}\(\)#&@\"\^_\|…\\\\])");
    std::wstring w_result = std::regex_replace(w_input, special_chars, L"");

    // 宽字符转回 UTF-8
    return converter.to_bytes(w_result);
}

std::string TextNormalizer::normalize_sentence(const std::string& sentence) {
    std::string result = sentence;
    std::string temp;
    std::smatch match;
    // std::cout << "orgin: " << result << std::endl;

    // 基本字符转换
    result = ChineseConverter::traditionalToSimplified(result);
    // std::cout << "traditionalToSimplified: " << result << std::endl;

    // 日期处理
    if (std::regex_search(result, match, Chronology::RE_DATE)) {
        temp = std::string(match.prefix()) + Chronology::replace_date(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_DATE: " << result << std::endl;

    while (std::regex_search(result, match, Chronology::RE_DATE2)) {
        temp = std::string(match.prefix()) + Chronology::replace_date2(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_DATE2: " << result << std::endl;

    // 时间范围处理
    while (std::regex_search(result, match, Chronology::RE_TIME_RANGE)) {
        temp = std::string(match.prefix()) + Chronology::replace_time(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_TIME_RANGE: " << result << std::endl;
    // 时间处理
    while (std::regex_search(result, match, Chronology::RE_TIME)) {
        temp = std::string(match.prefix()) + Chronology::replace_time(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_TIME: " << result << std::endl;

    // 摄氏度处理
    while (std::regex_search(result, match, Quantifier::RE_TEMPERATURE)) {
        temp = std::string(match.prefix()) + Quantifier::replace_temperature(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_TEMPERATURE: " << result << std::endl;
    // 单位转换
    // TODO 需要全部测试下 某些可能不work
    Quantifier::replace_measure(result);
    // std::cout << "replace_measure: " << result << std::endl;

    //分数表达式
    while (std::regex_search(result, match, Num::RE_FRAC)) {
        temp = std::string(match.prefix()) + Num::replace_frac(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_FRAC: " << result << std::endl;

    //百分比
    while (std::regex_search(result, match, Num::RE_PERCENTAGE)) {
        temp = std::string(match.prefix()) + Num::replace_percentage(match) + std::string(match.suffix());
        result = temp;
    }
    // std::cout << "RE_PERCENTAGE: " << result << std::endl;

    // 手机
    while (std::regex_search(result, match, PhoneNormalizer::RE_MOBILE_PHONE)) {
        temp = std::string(match.prefix()) + PhoneNormalizer::replace_mobile(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_MOBILE_PHONE: " << result << std::endl;
    }


    // 固定电话
    while (std::regex_search(result, match, PhoneNormalizer::RE_TELEPHONE)) {
        temp = std::string(match.prefix()) + PhoneNormalizer::replace_phone(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_TELEPHONE: " << result << std::endl;
    }


    // 国家地区
    while (std::regex_search(result, match, PhoneNormalizer::RE_NATIONAL_UNIFORM_NUMBER)) {
        temp = std::string(match.prefix()) + PhoneNormalizer::replace_phone(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_NATIONAL_UNIFORM_NUMBER: " << result << std::endl;
    }


    //=============================================数字处理
    while (std::regex_search(result, match, Num::RE_RANGE)) {
        temp = std::string(match.prefix()) + Num::replace_range(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_RANGE: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_INTEGER)) {
        temp = std::string(match.prefix()) + Num::replace_negative_num(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_INTEGER: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_NUMBER)) {
        temp = std::string(match.prefix()) + Num::replace_number(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_NUMBER: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_DECIMAL_NUM)) {
        temp = std::string(match.prefix()) + Num::replace_number(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_DECIMAL_NUM: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_POSITIVE_QUANTIFIERS)) {
        temp = std::string(match.prefix()) + Num::replace_positive_quantifier(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_POSITIVE_QUANTIFIERS: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_DEFAULT_NUM)) {
        temp = std::string(match.prefix()) + Num::replace_default_num(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_DEFAULT_NUM: " << result << std::endl;
    }
    while (std::regex_search(result, match, Num::RE_NUMBER)) {
        temp = std::string(match.prefix()) + Num::replace_number(match) + std::string(match.suffix());
        result = temp;
        // std::cout << "RE_NUMBER: " << result << std::endl;
    }

    result = _post_replace(result);

    return result;
}

std::vector<std::string> TextNormalizer::normalize(const std::string& text) {
    std::vector<std::string> sentences = _split(text);
    std::vector<std::string> normalized_sentences;
    normalized_sentences.reserve(sentences.size());

    for (const auto& sent : sentences) {
        normalized_sentences.push_back(normalize_sentence(sent));
    }

    return normalized_sentences;
}