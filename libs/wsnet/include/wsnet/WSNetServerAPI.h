#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "scapix_object.h"
#include "WSNetCancelableCallback.h"

namespace wsnet {

//TODO: enums replace to int? kIncorrectJson possible?
enum class ServerApiRetCode { kSuccess = 0, kNetworkError, kNoNetworkConnection, kIncorrectJson, kFailoverFailed };
enum class UpdateChannel { kRelease = 0, kBeta, kGuineaPig, kInternal };

typedef std::function<void(std::uint32_t num, std::uint32_t count)> WSNetTryingBackupEndpointCallback;
typedef std::function<void(ServerApiRetCode serverApiRetCode, const std::string &jsonData)> WSNetRequestFinishedCallback;

class WSNetServerAPI : public scapix_object<WSNetServerAPI>
{
public:
    virtual ~WSNetServerAPI() {}

    virtual std::string currentSettings() = 0;

    virtual void setApiResolutionsSettings(bool isAutomatic, std::string manualAddress) = 0;
    virtual void setIgnoreSslErrors(bool bIgnore) = 0;

    // callback function allowing the caller to know which failover is used
    virtual std::shared_ptr<WSNetCancelableCallback> setTryingBackupEndpointCallback(WSNetTryingBackupEndpointCallback tryingBackupEndpointCallback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> login(const std::string &username, const std::string &password,
                                                           const std::string &code2fa, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> session(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> deleteSession(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> serverLocations(const std::string &language, const std::string &revision,
                                                                     bool isPro, const std::vector<std::string> &alcList,
                                                                     WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> serverCredentials(const std::string &authHash, bool isOpenVpnProtocol, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> serverConfigs(const std::string &authHash, const std::string &ovpnVersion, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> portMap(const std::string &authHash, std::uint32_t version, const std::vector<std::string> &forceProtocols, WSNetRequestFinishedCallback callback) = 0;

    // Platform value can be "windows", "mac", "linux", "android", "ios"
    virtual std::shared_ptr<WSNetCancelableCallback> recordInstall(const std::string &platform, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> addEmail(const std::string &authHash, const std::string &email, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> confirmEmail(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;

    // Required: username, password
    // Optionals: referringUsername, email
    virtual std::shared_ptr<WSNetCancelableCallback> signup(const std::string &username, const std::string &password,
                                                            const std::string &referringUsername, const std::string &email,
                                                            WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> webSession(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> checkUpdate(UpdateChannel updateChannel,
                                                                 const std::string &appVersion, const std::string &appBuild,
                                                                 const std::string &osVersion, const std::string &osBuild,
                                                                 WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> debugLog(const std::string &username, const std::string &strLog, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> speedRating(const std::string &authHash, const std::string &hostname, const std::string &ip,
                                                                 std::int32_t rating, WSNetRequestFinishedCallback callback) = 0;

    // Platform value can be "windows", "mac", "linux", "android", "ios"
    virtual std::shared_ptr<WSNetCancelableCallback> staticIps(const std::string &authHash, const std::string &platform, const std::string &deviceId, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> pingTest(std::uint32_t timeoutMs, WSNetRequestFinishedCallback callback) = 0;

    // pcpid parameter is optional and can be empty string
    virtual std::shared_ptr<WSNetCancelableCallback> notifications(const std::string &authHash, const std::string &pcpid, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> getRobertFilters(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> setRobertFilter(const std::string &authHash, const std::string &id,  std::int32_t status, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> syncRobert(const std::string &authHash, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> wgConfigsInit(const std::string &authHash, const std::string &clientPublicKey,
                                                                   bool deleteOldestKey, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> wgConfigsConnect(const std::string &authHash, const std::string &clientPublicKey,
                                                                   const std::string &hostname, const std::string &deviceId,
                                                                   WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> myIP(WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> mobileBillingPlans(const std::string &mobilePlanType, int version, WSNetRequestFinishedCallback callback) = 0;

    // Required: purchaseToken
    // Optionals: gpPackageName, gpProductId, type, amazonUserId
    virtual std::shared_ptr<WSNetCancelableCallback> verifyPayment(const std::string &authHash,
                                                                   const std::string &purchaseToken, const std::string &gpPackageName,
                                                                   const std::string &gpProductId, const std::string &type,
                                                                   const std::string &amazonUserId,
                                                                   WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> postBillingCpid(const std::string &authHash, const std::string &payCpid, WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> getXpressLoginCode(WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> verifyXpressLoginCode(const std::string &xpressCode, const std::string &sig, WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> sendSupportTicket(const std::string &supportEmail, const std::string &supportName,
                                                                       const std::string &supportSubject, const std::string &supportMessage,
                                                                       const std::string &supportCategory,
                                                                       const std::string &type,
                                                                       const std::string &channel,
                                                                       const std::string &platform,
                                                                       WSNetRequestFinishedCallback callback) = 0;

    virtual std::shared_ptr<WSNetCancelableCallback> regToken(WSNetRequestFinishedCallback callback) = 0;
    virtual std::shared_ptr<WSNetCancelableCallback> signupUsingToken(const std::string &token, WSNetRequestFinishedCallback callback) = 0;

    // claimAccount - optional integer but passed as a string. If the string is empty, the parameter is ignored
    virtual std::shared_ptr<WSNetCancelableCallback> claimAccount(const std::string &authHash, const std::string &username, const std::string &password,
                                                                  const std::string &email, const std::string &claimAccount,
                                                                  WSNetRequestFinishedCallback callback) = 0;
};

} // namespace wsnet
