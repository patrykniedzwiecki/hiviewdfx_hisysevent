/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HIVIEWDFX_NAPI_HISYSEVENT_UTIL_H
#define HIVIEWDFX_NAPI_HISYSEVENT_UTIL_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "hisysevent_manager.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_hisysevent_adapter.h"
#include "napi_callback_context.h"

namespace OHOS {
namespace HiviewDFX {
class NapiHiSysEventUtil {
public:
    static constexpr char DOMAIN_ATTR[] = "domain";
    static constexpr char NAME_ATTR[] = "name";
    static constexpr char EVENT_TYPE_ATTR[] = "eventType";

public:
    static void ParseHiSysEventInfo(const napi_env env, napi_value* param, size_t paramNum, HiSysEventInfo& info);
    static void CreateHiSysEventInfoJsObject(const napi_env env, const std::string& eventDetail,
        napi_value& sysEventInfo);
    static void AppendStringPropertyToJsObject(const napi_env env, const std::string& key,
        const std::string& value, napi_value& jsObj);
    static void AppendInt32PropertyToJsObject(const napi_env env, const std::string& key,
        const int32_t& value, napi_value& jsObj);
    static void CreateJsSysEventInfoArray(const napi_env env, const std::vector<std::string>& originValues,
        napi_value& array);
    static void CreateJsInt64Array(const napi_env env, const std::vector<int64_t>& originValues, napi_value& array);
    static int32_t ParseListenerRules(const napi_env env, napi_value& jsObj,
        std::vector<ListenerRule>& listenerRules);
    static int32_t ParseQueryRules(const napi_env env, napi_value& jsObj, std::vector<QueryRule>& queryRules);
    static int32_t ParseQueryArg(const napi_env env, napi_value& jsObj, QueryArg& queryArg);
    static void CreateInt32Value(const napi_env env, int32_t value, napi_value& ret);
    static void CreateBigInt64Value(const napi_env env, int64_t value, napi_value& ret);
    static void CreateStringValue(const napi_env env, std::string value, napi_value& ret);
    static napi_value GetPropertyByName(const napi_env env, const napi_value& object,
        const std::string& propertyName);

public:
    template<typename T>
    static typename std::unordered_map<napi_ref, std::shared_ptr<T>>::iterator
    CompareAndReturnCacheItem(const napi_env env, napi_value& standard,
        std::unordered_map<napi_ref, std::shared_ptr<T>>& resources)
    {
        bool found = false;
        napi_status status;
        auto iter = resources.begin();
        for (; iter != resources.end(); iter++) {
            napi_value val = nullptr;
            status = napi_get_reference_value(env, iter->first, &val);
            if (status != napi_ok) {
                continue;
            }
            status = napi_strict_equals(env, standard, val, &found);
            if (status != napi_ok) {
                continue;
            }
            if (found) {
                break;
            }
        }
        return iter;
    }
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HIVIEWDFX_NAPI_HISYSEVENT_UTIL_H
