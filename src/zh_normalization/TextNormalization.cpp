//
// Created by tao on 25-4-9.
//

#include "zh_normalization/TextNormalization.h"

#include <iostream>

#include "zh_normalization/chinese_converter.h"
#include "zh_normalization/chronology.h"
#include "zh_normalization/constants.h"
#include "zh_normalization/num.h"
#include "zh_normalization/phonecode.h"
#include "zh_normalization/quantifier.h"

TextNormalizer::TextNormalizer() : SENTENCE_SPLITOR(R"(([：、，；。？！,;?!]["']?))") {}

std::vector<std::string> TextNormalizer::_split(const std::string& text, const std::string& lang) {
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

    // 过滤特殊字符
    std::regex special_chars(R"([-——《》【】<=>{}()（）#&@""^_|…\\])");
    result = std::regex_replace(result, special_chars, "");

    return result;
}

std::string TextNormalizer::normalize_sentence(const std::string& sentence) {
    std::string result = sentence;
    std::string temp;
    std::smatch match;
    std::cout << "orgin: " << result << std::endl;

    // 基本字符转换
    result = ChineseConverter::traditionalToSimplified(result);
    std::cout << "traditionalToSimplified: " << result << std::endl;

    // 日期处理
    if (std::regex_search(result, match, Chronology::RE_DATE)) {
        temp = std::string(match.prefix()) + Chronology::replace_date(match) + std::string(match.suffix());
        result = temp;
    }
    std::cout << "RE_DATE: " << result << std::endl;

    // while (std::regex_search(result, match, Chronology::RE_DATE2)) {
    //     temp = std::string(match.prefix()) + Chronology::replace_date2(match) + std::string(match.suffix());
    //     result = temp;
    // }
    // std::cout << "RE_DATE2: " << result << std::endl;
    //
    // // 时间范围处理
    // while (std::regex_search(result, match, Chronology::RE_TIME_RANGE)) {
    //     temp = std::string(match.prefix()) + Chronology::replace_time(match) + std::string(match.suffix());
    //     result = temp;
    // }
    // std::cout << "RE_TIME_RANGE: " << result << std::endl;

    // result = _post_replace(result);

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