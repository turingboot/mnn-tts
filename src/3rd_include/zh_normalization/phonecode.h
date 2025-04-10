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

    // 手机号码正则表达式
    // 移动：139、138、137、136、135、134、159、158、157、150、151、152、188、187、182、183、184、178、198
    // 联通：130、131、132、156、155、186、185、176
    // 电信：133、153、189、180、181、177
    static const inline std::regex RE_MOBILE_PHONE{R"([^0-9](?:(?:\+?86 )?1(?:[38][0-9]|5[0-35-9]|7[678]|9[89])[0-9]{8})[^0-9])"};
    static const inline std::regex RE_TELEPHONE{R"([^0-9](?:(?:0(?:10|2[1-3]|[3-9][0-9]{2})-?)?[1-9][0-9]{6,7})[^0-9])"};
    static const inline std::regex RE_NATIONAL_UNIFORM_NUMBER{R"(400(?:-)?[0-9]{3}(?:-)?[0-9]{4})"};

    // 禁止创建实例
    PhoneNormalizer() = delete;
};
#endif // PHONE_NORMALIZER_HPP