#ifndef CHINESE_CONVERTER_H
#define CHINESE_CONVERTER_H

#include <string>
#include <unordered_map>

class ChineseConverter {
public:
    static std::string traditionalToSimplified(const std::string& text);
    static std::string simplifiedToTraditional(const std::string& text);

private:
    static std::unordered_map<char32_t, char32_t> t2s_dict;
    static std::unordered_map<char32_t, char32_t> s2t_dict;
    static void initializeDicts();
    static bool is_initialized;
};

#endif // CHINESE_CONVERTER_H 