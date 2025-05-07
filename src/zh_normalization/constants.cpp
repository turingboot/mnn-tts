#include <zh_normalization/constants.h>
#include <codecvt>
#include <locale>

// 定义静态常量
const std::string Constants::ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string Constants::DIGITS = "0123456789";
const std::string Constants::PUNCTUATIONS = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

#ifdef SUPPORT_UCS4
const std::string Constants::NSW_PATTERN = 
    "(?:[^"
    "\u3007"  // 〇
    "\u3400-\u4dbf"  // CJK扩展A:[3400-4DBF]
    "\u4e00-\u9fff"  // CJK基本:[4E00-9FFF]
    "\uf900-\ufaff"  // CJK兼容:[F900-FAFF]
    "\U00020000-\U0002A6DF"  // CJK扩展B:[20000-2A6DF]
    "\U0002A703-\U0002B73F"  // CJK扩展C:[2A700-2B73F]
    "\U0002B740-\U0002B81D"  // CJK扩展D:[2B740-2B81D]
    "\U0002F80A-\U0002FA1F"  // CJK兼容扩展:[2F800-2FA1F]
    "])+";
#else
const std::string Constants::NSW_PATTERN_NO_UCS4 = 
    "(?:[^"
    "\u3007"  // 〇
    "\u3400-\u4dbf"  // CJK扩展A:[3400-4DBF]
    "\u4e00-\u9fff"  // CJK基本:[4E00-9FFF]
    "\uf900-\ufaff"  // CJK兼容:[F900-FAFF]
    "])+";
#endif

Constants& Constants::getInstance() {
    static Constants instance;
    return instance;
}

Constants::Constants() : is_initialized(false) {
    initializeMaps();
    #ifdef SUPPORT_UCS4
    re_nsw = std::regex(NSW_PATTERN, std::regex::ECMAScript | std::regex::optimize);
    #else
    re_nsw = std::regex(NSW_PATTERN_NO_UCS4, std::regex::ECMAScript | std::regex::optimize);
    #endif
}

void Constants::initializeMaps() {
    if (is_initialized) return;
    
    // 初始化ASCII字母映射
    for (char c : ASCII_LETTERS) {
        f2h_ascii_letters[static_cast<char32_t>(c) + 65248] = static_cast<char32_t>(c);
        h2f_ascii_letters[static_cast<char32_t>(c)] = static_cast<char32_t>(c) + 65248;
    }
    
    // 初始化数字映射
    for (char c : DIGITS) {
        f2h_digits[static_cast<char32_t>(c) + 65248] = static_cast<char32_t>(c);
        h2f_digits[static_cast<char32_t>(c)] = static_cast<char32_t>(c) + 65248;
    }
    
    // 初始化标点符号映射
    for (char c : PUNCTUATIONS) {
        f2h_punctuations[static_cast<char32_t>(c) + 65248] = static_cast<char32_t>(c);
        h2f_punctuations[static_cast<char32_t>(c)] = static_cast<char32_t>(c) + 65248;
    }
    
    // 初始化空格映射
    f2h_space[0x3000] = 0x0020;  // 全角空格到半角空格
    h2f_space[0x0020] = 0x3000;  // 半角空格到全角空格
    
    is_initialized = true;
}

std::string Constants::fullToHalf(const std::string& text) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32str = converter.from_bytes(text);
    
    std::u32string result;
    result.reserve(u32str.length());
    
    for (char32_t c : u32str) {
        // 检查ASCII字母
        auto it_letter = f2h_ascii_letters.find(c);
        if (it_letter != f2h_ascii_letters.end()) {
            result += it_letter->second;
            continue;
        }
        
        // 检查数字
        auto it_digit = f2h_digits.find(c);
        if (it_digit != f2h_digits.end()) {
            result += it_digit->second;
            continue;
        }
        
        // 检查标点符号
        auto it_punct = f2h_punctuations.find(c);
        if (it_punct != f2h_punctuations.end()) {
            result += it_punct->second;
            continue;
        }
        
        // 检查空格
        if (c == 0x3000) {
            result += 0x0020;
            continue;
        }
        
        // 其他字符保持不变
        result += c;
    }
    
    return converter.to_bytes(result);
}

std::string Constants::halfToFull(const std::string& text) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32str = converter.from_bytes(text);
    
    std::u32string result;
    result.reserve(u32str.length());
    
    for (char32_t c : u32str) {
        // 检查ASCII字母
        auto it_letter = h2f_ascii_letters.find(c);
        if (it_letter != h2f_ascii_letters.end()) {
            result += it_letter->second;
            continue;
        }
        
        // 检查数字
        auto it_digit = h2f_digits.find(c);
        if (it_digit != h2f_digits.end()) {
            result += it_digit->second;
            continue;
        }
        
        // 检查标点符号
        auto it_punct = h2f_punctuations.find(c);
        if (it_punct != h2f_punctuations.end()) {
            result += it_punct->second;
            continue;
        }
        
        // 检查空格
        if (c == 0x0020) {
            result += 0x3000;
            continue;
        }
        
        // 其他字符保持不变
        result += c;
    }
    
    return converter.to_bytes(result);
}

std::vector<std::string> Constants::getNSWs(const std::string& text) {
    std::vector<std::string> results;
    std::string::const_iterator searchStart(text.cbegin());
    std::smatch matches;
    
    while (std::regex_search(searchStart, text.cend(), matches, re_nsw)) {
        results.push_back(matches[0].str());
        searchStart = matches.suffix().first;
    }
    
    return results;
} 