#ifndef QUANTIFIER_H
#define QUANTIFIER_H

#include <string>
#include <map>
#include <regex>

class Quantifier {
public:
    static std::string replace_temperature(const std::smatch& match);
    static std::string replace_measure(std::string& sentence);
    static inline auto RE_TEMPERATURE = std::regex(R"((-?)(\d+(\.\d+)?)(°C|℃|度|摄氏度))");
    static const std::regex RE_PUNCTUATION;

private:
    static const std::map<std::string, std::string> measure_dict;
};

#endif //QUANTIFIER_H 