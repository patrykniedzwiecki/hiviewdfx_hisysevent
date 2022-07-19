/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HISYSEVENT_FRAMEWORKS_NATIVE_INCLUDE_FLAT_JSON_PARSER_H
#define HISYSEVENT_FRAMEWORKS_NATIVE_INCLUDE_FLAT_JSON_PARSER_H

#include <functional>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
using KV = std::pair<std::string, std::string>;
using PrintKvHandler = std::function<std::string(KV&)>;
class FlatJsonParser {
public:
    FlatJsonParser(const std::string& jsonStr);

public:
    static void Initialize();

public:
    void AppendStringValue(const std::string& key, const std::string& value);
    void AppendUInt64Value(const std::string& key, const uint64_t value);
    void Parse(const std::string& jsonStr);
    std::string Print(PrintKvHandler handler);

public:
    static constexpr uint8_t CHAR_RANGE {128};
    static constexpr uint8_t NUMBER_FLAG {1};
    static constexpr uint8_t STRING_FLAG {2};
    static constexpr uint8_t BRACKET_FLAG {3};

private:
    std::string ParseKey(const std::string& jsonStr);
    std::string ParseValue(const std::string& jsonStr);
    std::string ParseNumer(const std::string& jsonStr);
    std::string ParseString(const std::string& jsonStr);
    std::string ParseBrackets(const std::string& jsonStr, char leftBracket);

private:
    static uint8_t charFilter_[CHAR_RANGE];

private:
    std::vector<KV> kvList_;
    size_t index_ {0};
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HISYSEVENT_FRAMEWORKS_NATIVE_INCLUDE_FLAT_JSON_PARSER_H
