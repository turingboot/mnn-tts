#include "zh_normalization/quantifier.h"
#include "zh_normalization/num.h"

const std::map<std::string, std::string> Quantifier::measure_dict = {
    {"cm2", "平方厘米"},
    {"cm²", "平方厘米"},
    {"cm3", "立方厘米"},
    {"cm³", "立方厘米"},
    {"cm", "厘米"},
    {"db", "分贝"},
    {"ds", "毫秒"},
    {"kg", "千克"},
    {"km", "千米"},
    {"m2", "平方米"},
    {"m²", "平方米"},
    {"m³", "立方米"},
    {"m3", "立方米"},
    {"ml", "毫升"},
    {"m", "米"},
    {"mm", "毫米"},
    {"s", "秒"},
    {"h", "小时"},
    {"mg", "毫克"}
};

std::string Quantifier::replace_temperature(const std::smatch& match) {
    std::string sign = match[1].str();
    std::string temperature = match[2].str();
    std::string unit = match[4].str();

    sign = sign.empty() ? "" : "零下";
    temperature = Num::num2str(temperature);
    unit = (unit == "摄氏度") ? "摄氏度" : "度";

    return sign + temperature + unit;
}

std::string Quantifier::replace_measure(std::string sentence) {
    for (const auto& pair : measure_dict) {
        const std::string& q_notation = pair.first;
        const std::string& replacement = pair.second;

        size_t pos = 0;
        while ((pos = sentence.find(q_notation, pos)) != std::string::npos) {
            sentence.replace(pos, q_notation.length(), replacement);
            pos += replacement.length();
        }
    }
    return sentence;
} 