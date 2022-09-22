#include "staticipsrequest.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "utils/logger.h"
#include "utils/ws_assert.h"
#include "version/appversion.h"

namespace server_api {

StaticIpsRequest::StaticIpsRequest(QObject *parent, const QString &hostname, const QString &authHash, const QString &deviceId) : BaseRequest(parent, RequestType::kGet, hostname),
    authHash_(authHash),
    deviceId_(deviceId)
{
}

QUrl StaticIpsRequest::url() const
{
    QUrl url("https://" + hostname_ + "/StaticIps");

    QUrlQuery query;

#ifdef Q_OS_WIN
    QString strOs = "win";
#elif defined Q_OS_MAC
    QString strOs = "mac";
#elif defined Q_OS_LINUX
    QString strOs = "linux";
#endif
    query.addQueryItem("os", strOs);
    query.addQueryItem("device_id", deviceId_);
    addAuthQueryItems(query, authHash_);
    addPlatformQueryItems(query);
    url.setQuery(query);
    return url;
}

QString StaticIpsRequest::name() const
{
    return "StaticIps";
}

void StaticIpsRequest::handle(const QByteArray &arr)
{
    QJsonParseError errCode;
    QJsonDocument doc = QJsonDocument::fromJson(arr, &errCode);
    if (errCode.error != QJsonParseError::NoError || !doc.isObject()) {
        qCDebugMultiline(LOG_SERVER_API) << arr;
        qCDebug(LOG_SERVER_API) << "Failed parse JSON for StaticIps";
        setRetCode(SERVER_RETURN_INCORRECT_JSON);
        return;
    }

    QJsonObject jsonObject = doc.object();
    if (!jsonObject.contains("data")) {
        qCDebugMultiline(LOG_SERVER_API) << arr;
        qCDebug(LOG_SERVER_API) << "Failed parse JSON for StaticIps";
        setRetCode(SERVER_RETURN_INCORRECT_JSON);
        return;
    }

    QJsonObject jsonData =  jsonObject["data"].toObject();
    if (!staticIps_.initFromJson(jsonData)) {
        qCDebugMultiline(LOG_SERVER_API) << arr;
        qCDebug(LOG_SERVER_API) << "Failed parse JSON for StaticIps";
        setRetCode(SERVER_RETURN_INCORRECT_JSON);
        return;
    }

    qCDebug(LOG_SERVER_API) << "StaticIps request successfully executed";
    setRetCode(SERVER_RETURN_SUCCESS);
}

} // namespace server_api {
