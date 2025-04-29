#ifndef PHONE_NORMALIZER_HPP
#define PHONE_NORMALIZER_HPP

#include <string>
#include <regex>

class PhoneNormalizer {
public:
    // 将电话号码转换为字符串表示
    static std::string phone2str(const std::string& phone_string, bool mobile = true);

    // 替换固定电话号码
    static std::string replace_phone(const std::smatch& match);

    // 替换手机号码
    static std::string replace_mobile(const std::smatch& match);


    // Mobile phone numbers
    // Matches: 13812345678, +8613812345678, +86 13812345678
    static inline auto RE_MOBILE_PHONE  = std::regex(
    R"((\+?86 ?)?1([38]\d|5[0-35-9]|7[678]|9[89])\d{8})"
    );

    // Telephone numbers
    // Matches: 010-1234567, 0512-1234567, 12345678
     static inline auto RE_TELEPHONE  = std::regex(
     R"((0(10|2[1-3]|[3-9]\d{2})-?)?[1-9]\d{6,7})"
    );

    // National uniform numbers (400 numbers)
    // Matches: 400-123-4567, 4001234567
    static inline auto RE_NATIONAL_UNIFORM_NUMBER = std::regex(
    R"(400(-)?\d{3}(-)?\d{4})"
    );

    // 禁止创建实例
    PhoneNormalizer() = delete;
};
#endif // PHONE_NORMALIZER_HPP