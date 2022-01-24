/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hisysevent_delegate.h"

#include "hilog/log.h"
#include "hisysevent_listener_proxy.h"
#include "hisysevent_query_proxy.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sys_event_service_proxy.h"
#include "system_ability_definition.h"

using namespace std;

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiView-HiSysEventDelegate" };

void HiSysEventDelegate::ConvertListenerRule(const std::vector<ListenerRule>& rules,
    std::vector<SysEventRule>& sysRules) const
{
    for_each(rules.cbegin(), rules.cend(), [&sysRules](const ListenerRule& rule) {
        if (rule.GetTag().empty()) {
            sysRules.emplace_back(rule.GetDomain(), rule.GetEventName(), rule.GetRuleType());
        } else {
            sysRules.emplace_back(rule.GetTag(), rule.GetRuleType());
        }
    });
}

void HiSysEventDelegate::ConvertQueryRule(const std::vector<QueryRule>& rules,
    std::vector<SysEventQueryRule>& sysRules) const
{
    for_each(rules.cbegin(), rules.cend(), [&sysRules](const QueryRule &rule) {
        std::vector<std::string> events;
        auto eventList = rule.GetEventList();
        for_each(eventList.cbegin(), eventList.cend(), [&](const std::string &event) {
            events.push_back(event);
        });

        sysRules.emplace_back(rule.GetDomain(), events, rule.GetRuleType());
    });
}

int HiSysEventDelegate::AddEventListener(const std::shared_ptr<HiSysEventSubscribeCallBackBase> listener,
    const std::vector<ListenerRule>& rules, const void* compFactor)
{
    auto service = GetSysEventService();
    if (service == nullptr) {
        HiLog::Error(LABEL, "Fail to get service.");
        return 0;
    }
    std::vector<SysEventRule> eventRules;
    ConvertListenerRule(rules, eventRules);

    lock_guard<mutex> lock(mutexLock);
    bool needAddListener = false;
    sptr<HiSysEventListenerProxy> spListenerCallBack;
    auto iter = find_if(listenerList.begin(), listenerList.end(), [&](auto &ele) {
        return ele->Compare(listener, compFactor);
    });
    if (iter == listenerList.end()) {
        spListenerCallBack = new HiSysEventListenerProxy(listener);
        HiLog::Debug(LABEL, "AddEventListener new handle");
        needAddListener = true;
    } else {
        spListenerCallBack = *iter;
        HiLog::Debug(LABEL, "AddEventListener old handle");
    }

    HiLog::Debug(LABEL, "AddEventListener subscribe %{public}d", static_cast<int>(listenerList.size()));
    SysEventServiceProxy sysEventService(service);
    service->RemoveDeathRecipient(spListenerCallBack->GetCallbackDeathRecipient());
    int result = sysEventService.AddListener(eventRules, spListenerCallBack);
    if (needAddListener) {
        listenerList.push_back(spListenerCallBack);
    }
    return result;
}

void HiSysEventDelegate::RemoveListener(const std::shared_ptr<HiSysEventSubscribeCallBackBase> listener,
    void* compFactor)
{
    lock_guard<mutex> lock(mutexLock);
    auto iter = find_if(listenerList.begin(), listenerList.end(), [&](auto &ele) {
        return ele->Compare(listener, compFactor);
    });
    if (iter == listenerList.end()) {
        return;
    }

    auto service = GetSysEventService();
    if (service == nullptr) {
        HiLog::Error(LABEL, "Fail to get service.");
        return;
    }
    SysEventServiceProxy sysEventService(service);
    sysEventService.RemoveListener(*iter);
    listenerList.erase(iter);
}

bool HiSysEventDelegate::SetDebugMode(const std::shared_ptr<HiSysEventSubscribeCallBackBase> listener,
    const bool mode, void* compFactor)
{
    lock_guard<mutex> lock(mutexLock);
    auto iter = find_if(listenerList.begin(), listenerList.end(), [&](auto &ele) {
        return ele->Compare(listener, compFactor);
    });
    if (iter == listenerList.end()) {
        return false;
    }

    auto service = GetSysEventService();
    if (service == nullptr) {
        HiLog::Error(LABEL, "Fail to get service.");
        return false;
    }
    SysEventServiceProxy sysEventService(service);
    return sysEventService.SetDebugMode(*iter, mode);
}

bool HiSysEventDelegate::QueryHiSysEvent(const struct QueryArg& queryArg,
    const std::vector<QueryRule>& queryRules,
    const std::shared_ptr<HiSysEventQueryCallBackBase> queryCallBack) const
{
    auto service = GetSysEventService();
    if (service == nullptr) {
        HiLog::Error(LABEL, "Fail to get service.");
        return false;
    }

    std::vector<SysEventQueryRule> aospRules;
    ConvertQueryRule(queryRules, aospRules);

    sptr<HiSysEventQueryProxy> spCallBack =
        new OHOS::HiviewDFX::HiSysEventQueryProxy(queryCallBack);

    SysEventServiceProxy sysEventService(service);
    return sysEventService.QuerySysEvent(queryArg.beginTime, queryArg.endTime,
        queryArg.maxEvents, aospRules, spCallBack);
}

void HiSysEventDelegate::BinderFunc()
{
    IPCSkeleton::JoinWorkThread();
}

sptr<IRemoteObject> HiSysEventDelegate::GetSysEventService() const
{
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        return nullptr;
    }
    return sam->CheckSystemAbility(DFX_SYS_EVENT_SERVICE_ABILITY_ID);
}
} // namespace HiviewDFX
} // namespace OHOS