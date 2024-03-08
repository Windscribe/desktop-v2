#pragma once

#include "firewallcontroller.h"
#include "engine/helper/helper_linux.h"

//thread safe
class FirewallController_linux : public FirewallController
{
    Q_OBJECT
public:
    explicit FirewallController_linux(QObject *parent, IHelper *helper);
    ~FirewallController_linux() override;

    bool firewallOn(const QString &connectingIp, const QSet<QString> &ips, bool bAllowLanTraffic, bool bIsCustomConfig) override;
    bool firewallOff() override;
    bool firewallActualState() override;

    bool whitelistPorts(const api_responses::StaticIpPortsVector &ports) override;
    bool deleteWhitelistPorts() override;

    void setInterfaceToSkip_posix(const QString &interfaceToSkip) override;
    void enableFirewallOnBoot(bool bEnable, const QSet<QString>& ipTable = QSet<QString>()) override;

private:
    Helper_linux *helper_;
    QString interfaceToSkip_;
    bool forceUpdateInterfaceToSkip_;
    QRecursiveMutex mutex_;
    QString pathToTempTable_;
    QString comment_;

    bool firewallOnImpl(const QString &connectingIp, const QSet<QString> &ips, bool bAllowLanTraffic, bool bIsCustomConfig, const api_responses::StaticIpPortsVector &ports);
    QStringList getWindscribeRules(const QString &comment, bool modifyForDelete, bool isIPv6);
    void removeWindscribeRules(const QString &comment, bool isIPv6);
    QStringList getLocalAddresses(const QString iface) const;
    QString getHotspotAdapter() const;
};
