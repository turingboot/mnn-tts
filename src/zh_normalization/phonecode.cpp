/*
 * Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "zh_normalization/phonecode.h"
#include "zh_normalization/num.h"
#include <sstream>
#include <algorithm>
#include <unordered_map>

std::string PhoneNormalizer::phone2str(const std::string& phone_string, bool mobile) {
    std::string processed_string = phone_string;
    if (mobile) {
        // 处理手机号码
        // 去除开头的'+'号
        if (!processed_string.empty() && processed_string[0] == '+') {
            processed_string = processed_string.substr(1);
        }

        // 按空格分割
        std::istringstream iss(processed_string);
        std::vector<std::string> parts;
        std::string part;
        while (iss >> part) {
            parts.push_back(Num::verbalize_digit(part, true));
        }

        // 用逗号连接
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result += "，";
            result += parts[i];
        }
        return result;
    } else {
        // 处理固定电话
        std::stringstream ss(processed_string);
        std::string part;
        std::vector<std::string> parts;

        // 按'-'分割
        while (std::getline(ss, part, '-')) {
            parts.push_back(Num::verbalize_digit(part, true));
        }

        // 用逗号连接
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result += "，";
            result += parts[i];
        }
        return result;
    }
}

std::string PhoneNormalizer::replace_phone(const std::smatch& match) {
    return phone2str(match.str(0), false);
}

std::string PhoneNormalizer::replace_mobile(const std::smatch& match) {
    return phone2str(match.str(0), true);
} 