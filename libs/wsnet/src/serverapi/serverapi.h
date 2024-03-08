#pragma once

#include "WSNetServerAPI.h"
#include <BS_thread_pool.hpp>
#include "WSNetHttpNetworkManager.h"
#include "WSNetAdvancedParameters.h"
#include "failover/ifailovercontainer.h"
#include "serverapi_settings.h"
#include "connectstate.h"

namespace wsnet {

class ServerAPI_impl;

class ServerAPI : public WSNetServerAPI
{
public:
    explicit ServerAPI(BS::thread_pool &taskQueue, WSNetHttpNetworkManager *httpNetworkManager, IFailoverContainer *failoverContainer,
                       const std::string &settings, WSNetAdvancedParameters *advancedParameters, ConnectState &connectState);
    virtual ~ServerAPI();

    std::string currentSettings() override;

    void setApiResolutionsSettings(bool isAutomatic, std::string manualAddress) override;
    void setIgnoreSslErrors(bool bIgnore) override;
    std::shared_ptr<WSNetCancelableCallback> setTryingBackupEndpointCallback(WSNetTryingBackupEndpointCallback tryingBackupEndpointCallback) override;

    std::shared_ptr<WSNetCancelableCallback> login(const std::string &username, const std::string &password,
                                                   const std::string &code2fa, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> session(const std::string &authHash, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> deleteSession(const std::string &authHash, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> serverLocations(const std::string &language, const std::string &revision,
                                                                     bool isPro, const std::vector<std::string> &alcList,
                                                                     WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> serverCredentials(const std::string &authHash, bool isOpenVpnProtocol, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> serverConfigs(const std::string &authHash, const std::string &ovpnVersion, WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> portMap(const std::string &authHash, std::uint32_t version, const std::vector<std::string> &forceProtocols, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> recordInstall(const std::string &platform, WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> addEmail(const std::string &authHash, const std::string &email, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> confirmEmail(const std::string &authHash, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> signup(const std::string &username, const std::string &password,
                                                            const std::string &referringUsername, const std::string &email,
                                                            WSNetRequestFinishedCallback callback) override;


    std::shared_ptr<WSNetCancelableCallback> webSession(const std::string &authHash, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> checkUpdate(UpdateChannel updateChannel,
                                                         const std::string &appVersion, const std::string &appBuild,
                                                         const std::string &osVersion, const std::string &osBuild,
                                                         WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> debugLog(const std::string &username, const std::string &strLog, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> speedRating(const std::string &authHash, const std::string &hostname, const std::string &ip,
                                                                 std::int32_t rating, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> staticIps(const std::string &authHash, const std::string &platform, const std::string &deviceId, WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> pingTest(std::uint32_t timeoutMs, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> notifications(const std::string &authHash, const std::string &pcpid, WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> getRobertFilters(const std::string &authHash, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> setRobertFilter(const std::string &authHash, const std::string &id,  std::int32_t status, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> syncRobert(const std::string &authHash, WSNetRequestFinishedCallback callback) override;


    std::shared_ptr<WSNetCancelableCallback> wgConfigsInit(const std::string &authHash, const std::string &clientPublicKey,
                                                           bool deleteOldestKey, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> wgConfigsConnect(const std::string &authHash, const std::string &clientPublicKey,
                                                                      const std::string &hostname, const std::string &deviceId,
                                                                      WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> myIP(WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> mobileBillingPlans(const std::string &mobilePlanType, int version, WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> verifyPayment(const std::string &authHash, const std::string &purchaseToken, const std::string &gpPackageName,
                                                           const std::string &gpProductId, const std::string &type, const std::string &amazonUserId,
                                                           WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> postBillingCpid(const std::string &authHash, const std::string &payCpid, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> getXpressLoginCode(WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> verifyXpressLoginCode(const std::string &xpressCode, const std::string &sig, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> sendSupportTicket(const std::string &supportEmail, const std::string &supportName,
                                                                       const std::string &supportSubject, const std::string &supportMessage,
                                                                       const std::string &supportCategory,
                                                                       const std::string &type,
                                                                       const std::string &channel,
                                                                       const std::string &platform,
                                                                       WSNetRequestFinishedCallback callback) override;

    std::shared_ptr<WSNetCancelableCallback> regToken(WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> signupUsingToken(const std::string &token, WSNetRequestFinishedCallback callback) override;
    std::shared_ptr<WSNetCancelableCallback> claimAccount(const std::string &authHash, const std::string &username, const std::string &password,
                                                                  const std::string &email, const std::string &claimAccount,
                                                                  WSNetRequestFinishedCallback callback) override;

private:
    std::unique_ptr<ServerAPI_impl> impl_;
    BS::thread_pool &taskQueue_;
    ServerAPISettings settings_;
    WSNetAdvancedParameters *advancedParameters_;
    ConnectState &connectState_;
    std::uint32_t subscriberId_;

    void onVPNConnectStateChanged(bool isConnected);
};

} // namespace wsnet
