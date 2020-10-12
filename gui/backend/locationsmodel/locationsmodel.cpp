#include "locationsmodel.h"

#include "alllocationsmodel.h"
#include "configuredcitiesmodel.h"
#include "staticipscitiesmodel.h"
#include "favoritecitiesmodel.h"
#include "sortlocationsalgorithms.h"

#include <QFile>
#include <QSharedPointer>


LocationsModel::LocationsModel(QObject *parent) : QObject(parent)
  , deviceName_("")
{
    favoriteLocationsStorage_.readFromSettings();
    allLocations_ = new AllLocationsModel(this);
    configuredLocations_ = new ConfiguredCitiesModel(this);
    staticIpsLocations_ = new StaticIpsCitiesModel(this);
    favoriteLocations_ = new FavoriteCitiesModel(this);
}

LocationsModel::~LocationsModel()
{
    favoriteLocationsStorage_.writeToSettings();
}

void LocationsModel::updateApiLocations(const ProtoTypes::LocationId &bestLocation, const ProtoTypes::ArrayLocations &locations)
{
    apiLocations_.clear();

    bestLocationId_ = LocationID::createFromProtoBuf(bestLocation);

    int cnt = locations.locations_size();
    for (int i = 0; i < cnt; ++i)
    {
        const ProtoTypes::Location &location = locations.locations(i);

        QSharedPointer<LocationModelItem> lmi(new LocationModelItem());
        lmi->initialInd_ = i;
        lmi->id = LocationID::createFromProtoBuf(location.id());
        lmi->title = QString::fromStdString(location.name());
        lmi->isShowP2P = location.is_p2p_supported();
        lmi->countryCode = QString::fromStdString(location.country_code()).toLower();
        lmi->isPremiumOnly = location.is_premium_only();

        int cities_cnt = location.cities_size();
        for (int c = 0; c < cities_cnt; ++c)
        {
            const ProtoTypes::City &city = location.cities(c);
            CityModelItem cmi;
            cmi.id = LocationID::createFromProtoBuf(city.id());
            cmi.city = QString::fromStdString(city.name());
            cmi.nick = QString::fromStdString(city.nick());
            cmi.countryCode = lmi->id.isStaticIpsLocation() ? QString::fromStdString(city.static_ip_country_code()) : lmi->countryCode;
            cmi.pingTimeMs = city.ping_time();
            cmi.bShowPremiumStarOnly = city.is_premium_only();
            cmi.isFavorite = favoriteLocationsStorage_.isFavorite(cmi.id);
            cmi.isDisabled = city.is_disabled();
            cmi.staticIpCountryCode = QString::fromStdString(city.static_ip_country_code());
            cmi.staticIpType = QString::fromStdString(city.static_ip_type());
            cmi.staticIp = QString::fromStdString(city.static_ip());
            lmi->cities << cmi;
        }

        // sort cities alphabetically
        std::sort(lmi->cities.begin(), lmi->cities.end(), SortLocationsAlgorithms::lessThanByAlphabeticallyCityItem);

        apiLocations_ << lmi;

        // if this is the best location then insert copy to top list
        if (!lmi->id.isStaticIpsLocation() && !lmi->id.isCustomConfigsLocation() && lmi->id.apiLocationToBestLocation() == bestLocationId_)
        {
            QSharedPointer<LocationModelItem> lmiBestLocation(new LocationModelItem());
            *lmiBestLocation = *lmi;
            lmiBestLocation->id = bestLocationId_;
            lmiBestLocation->title = tr("Best Location");
            for (int c = 0; c < lmiBestLocation->cities.count(); ++c)
            {
                lmiBestLocation->cities[c].id = lmiBestLocation->cities[c].id.apiLocationToBestLocation();
            }
            apiLocations_.insert(0, lmiBestLocation);
        }

        if (lmi->id.isStaticIpsLocation())
        {
            deviceName_ = QString::fromStdString(location.static_ip_device_name());
            if (!deviceName_.isEmpty())
            {
                emit deviceNameChanged(deviceName_);
            }
        }
    }

    allLocations_->update(apiLocations_);
    staticIpsLocations_->update(apiLocations_);
    favoriteLocations_->update(apiLocations_);
}

void LocationsModel::updateCustomConfigLocations(const ProtoTypes::ArrayLocations &locations)
{
    customConfigLocations_.clear();

    int cnt = locations.locations_size();
    for (int i = 0; i < cnt; ++i)
    {
        const ProtoTypes::Location &location = locations.locations(i);

        QSharedPointer<LocationModelItem> lmi(new LocationModelItem());
        lmi->initialInd_ = i;
        lmi->id = LocationID::createFromProtoBuf(location.id());
        lmi->title = QString::fromStdString(location.name());
        lmi->isShowP2P = location.is_p2p_supported();
        lmi->countryCode = QString::fromStdString(location.country_code()).toLower();
        lmi->isPremiumOnly = location.is_premium_only();

        int cities_cnt = location.cities_size();
        for (int c = 0; c < cities_cnt; ++c)
        {
            const ProtoTypes::City &city = location.cities(c);
            CityModelItem cmi;
            cmi.id = LocationID::createFromProtoBuf(city.id());
            cmi.city = QString::fromStdString(city.name());
            cmi.nick = QString::fromStdString(city.nick());
            cmi.countryCode = lmi->id.isStaticIpsLocation() ? QString::fromStdString(city.static_ip_country_code()) : lmi->countryCode;
            cmi.pingTimeMs = city.ping_time();
            cmi.bShowPremiumStarOnly = city.is_premium_only();
            cmi.isFavorite = favoriteLocationsStorage_.isFavorite(cmi.id);
            cmi.isDisabled = city.is_disabled();
            cmi.staticIpCountryCode = QString::fromStdString(city.static_ip_country_code());
            cmi.staticIpType = QString::fromStdString(city.static_ip_type());
            cmi.staticIp = QString::fromStdString(city.static_ip());
            lmi->cities << cmi;
        }

        // sort cities alphabetically
        std::sort(lmi->cities.begin(), lmi->cities.end(), SortLocationsAlgorithms::lessThanByAlphabeticallyCityItem);

        customConfigLocations_ << lmi;
    }

    configuredLocations_->update(customConfigLocations_);
}

BasicLocationsModel *LocationsModel::getAllLocationsModel()
{
    return allLocations_;
}

BasicCitiesModel *LocationsModel::getConfiguredLocationsModel()
{
    return configuredLocations_;
}

BasicCitiesModel *LocationsModel::getStaticIpsLocationsModel()
{
    return staticIpsLocations_;
}

BasicCitiesModel *LocationsModel::getFavoriteLocationsModel()
{
    return favoriteLocations_;
}

void LocationsModel::setOrderLocationsType(ProtoTypes::OrderLocationType orderLocationsType)
{
    if (orderLocationsType != orderLocationsType_)
    {
        orderLocationsType_ = orderLocationsType;
        allLocations_->setOrderLocationsType(orderLocationsType_);
        configuredLocations_->setOrderLocationsType(orderLocationsType_);
        staticIpsLocations_->setOrderLocationsType(orderLocationsType_);
        favoriteLocations_->setOrderLocationsType(orderLocationsType_);
    }
}

void LocationsModel::switchFavorite(const LocationID &id, bool isFavorite)
{
    if (isFavorite)
    {
        favoriteLocationsStorage_.addToFavorites(id);
    }
    else
    {
        favoriteLocationsStorage_.removeFromFavorites(id);
    }
    allLocations_->setIsFavorite(id, isFavorite);
    configuredLocations_->setIsFavorite(id, isFavorite);
    staticIpsLocations_->setIsFavorite(id, isFavorite);
    favoriteLocations_->setIsFavorite(id, isFavorite);
}

bool LocationsModel::getLocationInfo(const LocationID &id, LocationsModel::LocationInfo &li)
{
    if (id.isCustomConfigsLocation())
    {
        for (int i = 0; i < customConfigLocations_.count(); ++i)
        {
            if (customConfigLocations_[i]->id == id)
            {
                if (customConfigLocations_[i]->cities.count() > 0)
                {
                    li.id = id;
                    li.firstName = customConfigLocations_[i]->cities[0].city;
                    li.secondName = customConfigLocations_[i]->cities[0].nick;
                    li.countryCode = customConfigLocations_[i]->countryCode;
                    li.pingTime = customConfigLocations_[i]->cities[0].pingTimeMs;
                    return true;
                }
            }
            for (int k = 0; k < customConfigLocations_[i]->cities.count(); ++k)
            {
                if (customConfigLocations_[i]->cities[k].id == id)
                {
                    li.id = id;
                    li.firstName = customConfigLocations_[i]->cities[k].city;
                    li.secondName = customConfigLocations_[i]->cities[k].nick;
                    li.countryCode = customConfigLocations_[i]->cities[k].countryCode;
                    li.pingTime = customConfigLocations_[i]->cities[k].pingTimeMs;
                    return true;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < apiLocations_.count(); ++i)
        {
            if (apiLocations_[i]->id == id)
            {
                if (apiLocations_[i]->cities.count() > 0)
                {
                    li.id = id;
                    li.firstName = apiLocations_[i]->cities[0].city;
                    li.secondName = apiLocations_[i]->cities[0].nick;
                    li.countryCode = apiLocations_[i]->countryCode;
                    li.pingTime = apiLocations_[i]->cities[0].pingTimeMs;
                    return true;
                }
            }
            for (int k = 0; k < apiLocations_[i]->cities.count(); ++k)
            {
                if (apiLocations_[i]->cities[k].id == id)
                {
                    li.id = id;
                    li.firstName = apiLocations_[i]->cities[k].city;
                    li.secondName = apiLocations_[i]->cities[k].nick;
                    li.countryCode = apiLocations_[i]->cities[k].countryCode;
                    li.pingTime = apiLocations_[i]->cities[k].pingTimeMs;
                    return true;
                }
            }
        }
    }

    return false;
}

/*QList<CityModelItem> LocationsModel::activeCityModelItems()
{
    QList<CityModelItem> cities;

    for (int  i = 0; i < locations_.count(); ++i)
    {
        LocationModelItem *lmi = locations_[i];
        LocationID id = lmi->id;

        for (int k = 0; k < locations_[i]->cities.count(); ++k)
        {
            // if (!lmi->cities[k].isDisabled)
            {
                cities.append(lmi->cities[k]);
            }
        }
    }
    return cities;
}*/

/*QVector<LocationModelItem *> LocationsModel::locationModelItems()
{
    return locations_;
}*/

void LocationsModel::setFreeSessionStatus(bool isFreeSessionStatus)
{
    allLocations_->setFreeSessionStatus(isFreeSessionStatus);
    configuredLocations_->setFreeSessionStatus(isFreeSessionStatus);
    staticIpsLocations_->setFreeSessionStatus(isFreeSessionStatus);
    favoriteLocations_->setFreeSessionStatus(isFreeSessionStatus);
}

void LocationsModel::changeConnectionSpeed(LocationID id, PingTime speed)
{
    [&]() {
        for (auto lmi : apiLocations_) {
            for (auto &cmi : lmi->cities) {
                if (cmi.id == id) {
                    cmi.pingTimeMs = speed;
                    return;
                }
            }
        }
        for (auto lmi : customConfigLocations_) {
            for (auto &cmi : lmi->cities) {
                if (cmi.id == id) {
                    cmi.pingTimeMs = speed;
                    return;
                }
            }
        }
    }();

    allLocations_->changeConnectionSpeed(id, speed);
    configuredLocations_->changeConnectionSpeed(id, speed);
    staticIpsLocations_->changeConnectionSpeed(id, speed);
    favoriteLocations_->changeConnectionSpeed(id, speed);
    emit locationSpeedChanged(id, speed);
}

// example of location string: NL, Toronto #1, etc
LocationID LocationsModel::getLocationIdByName(const QString &location) const
{
    for (const QSharedPointer<LocationModelItem> lmi: apiLocations_)
    {
        if (lmi->countryCode.compare(location, Qt::CaseInsensitive) == 0)
        {
            return LocationID(lmi->id);
        }
        for (const CityModelItem &city: lmi->cities)
        {
            if (city.city.compare(location, Qt::CaseInsensitive) == 0)
            {
                return city.id;
            }
        }
    }

    return LocationID();
}

LocationID LocationsModel::getBestLocationId() const
{
    Q_ASSERT(bestLocationId_.isValid());
    return bestLocationId_;
}
