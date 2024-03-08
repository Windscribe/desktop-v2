#pragma once

#include <map>
#include <BS_thread_pool.hpp>
#include "WSNetEmergencyConnect.h"
#include "WSNetDnsResolver.h"
#include "failover/ifailovercontainer.h"
#include "utils/cancelablecallback.h"

namespace wsnet {

class EmergencyConnect : public WSNetEmergencyConnect
{
public:
    explicit EmergencyConnect(BS::thread_pool &taskQueue, IFailoverContainer *failoverContainer, WSNetDnsResolver *dnsResolver);
    virtual ~EmergencyConnect();

    std::string ovpnConfig() const override;
    std::string username() const override;
    std::string password() const override;

    std::shared_ptr<WSNetCancelableCallback> getIpEndpoints(WSNetEmergencyConnectCallback callback) override;

private:
    BS::thread_pool &taskQueue_;
    IFailoverContainer *failoverContainer_;
    WSNetDnsResolver *dnsResolver_;

    std::mutex mutex_;
    std::uint64_t curRequestId_ = 0;
    std::map<std::uint64_t, std::pair< std::shared_ptr<WSNetCancelableCallback>, std::shared_ptr<CancelableCallback<WSNetEmergencyConnectCallback>>> > dnsRequests_;


    void onDnsResolved(std::uint64_t requestId, const std::string &hostname, std::shared_ptr<WSNetDnsRequestResult> result);
};

} // namespace wsnet
