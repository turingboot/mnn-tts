#pragma once

#include <string>
#include <unordered_map>
#include <regex>

class Constants {
public:
    // 获取单例实例
    static Constants& getInstance();

    // 全角转半角
    std::string fullToHalf(const std::string& text);
    
    // 半角转全角
    std::string halfToFull(const std::string& text);
    
    // 获取非汉字部分（NSW: Non-Standard-Word）
    std::vector<std::string> getNSWs(const std::string& text);

private:
    Constants();  // 私有构造函数
    ~Constants() = default;
    
    // 禁止拷贝和赋值
    Constants(const Constants&) = delete;
    Constants& operator=(const Constants&) = delete;

    void initializeMaps();
    
    // 全角半角映射表
    std::unordered_map<char32_t, char32_t> f2h_ascii_letters;
    std::unordered_map<char32_t, char32_t> h2f_ascii_letters;
    std::unordered_map<char32_t, char32_t> f2h_digits;
    std::unordered_map<char32_t, char32_t> h2f_digits;
    std::unordered_map<char32_t, char32_t> f2h_punctuations;
    std::unordered_map<char32_t, char32_t> h2f_punctuations;
    std::unordered_map<char32_t, char32_t> f2h_space;
    std::unordered_map<char32_t, char32_t> h2f_space;
    
    // 用于NSW提取的正则表达式
    std::regex re_nsw;
    bool is_initialized;
    
    // 常量定义
    static const std::string ASCII_LETTERS;
    static const std::string DIGITS;
    static const std::string PUNCTUATIONS;
    
    // NSW正则表达式模式
    #ifdef SUPPORT_UCS4
    static const std::string NSW_PATTERN;
    #else
    static const std::string NSW_PATTERN_NO_UCS4;
    #endif
}; 