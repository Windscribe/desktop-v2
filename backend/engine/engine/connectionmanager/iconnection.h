#ifndef ICONNECTION_H
#define ICONNECTION_H

#include <QThread>
#include "engine/proxy/proxysettings.h"
#include "engine/types/types.h"
#include "connectionadapterinfo.h"

class IHelper;
class WireGuardConfig;

enum class ConnectionType { IKEV2, OPENVPN, WIREGUARD };

class IConnection : public QThread
{
    Q_OBJECT

public:
    explicit IConnection(QObject *parent, IHelper *helper): QThread(parent), helper_(helper) {}
    virtual ~IConnection() {}

    // config path for openvpn, url for ikev2
    virtual void startConnect(const QString &configPathOrUrl, const QString &ip, const QString &dnsHostName,
                              const QString &username, const QString &password, const ProxySettings &proxySettings,
                              const WireGuardConfig *wireGuardConfig, bool isEnableIkev2Compression,
                              bool isAutomaticConnectionMode) = 0;
    virtual void startDisconnect() = 0;
    virtual bool isDisconnected() const = 0;
    virtual ConnectionType getConnectionType() const = 0;
    virtual bool isAllowFirewallAfterCustomConfigConnection() const { return true; }

    virtual void continueWithUsernameAndPassword(const QString &username, const QString &password) = 0;
    virtual void continueWithPassword(const QString &password) = 0;

signals:
    void connected(const ConnectionAdapterInfo &connectionAdapterInfo);
    void disconnected();
    void reconnecting();
    void error(CONNECTION_ERROR err);
    void statisticsUpdated(quint64 bytesIn, quint64 bytesOut, bool isTotalBytes);
    void interfaceUpdated(const QString &interfaceName);  // WireGuard-specific.

    void requestUsername();
    void requestPassword();

protected:
    IHelper *helper_;
};

#endif // ICONNECTION_H
