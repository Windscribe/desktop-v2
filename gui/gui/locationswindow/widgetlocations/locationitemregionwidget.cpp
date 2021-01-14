#include "locationitemregionwidget.h"

#include <QPainter>
#include "dpiscalemanager.h"
#include "commongraphics/commongraphics.h"

#include <QDebug>

namespace GuiLocations {

LocationItemRegionWidget::LocationItemRegionWidget(LocationModelItem *locationModelItem, QWidget *parent) : QAbstractButton(parent)
  , expanded_(false)
{
    regionHeaderWidget_ = QSharedPointer<LocationItemRegionHeaderWidget>(new LocationItemRegionHeaderWidget(locationModelItem, this));
    connect(regionHeaderWidget_.get(), SIGNAL(clicked()), SLOT(onRegionItemClicked()));
    connect(regionHeaderWidget_.get(), SIGNAL(selected(SelectableLocationItemWidget *)), SLOT(onRegionItemSelected(SelectableLocationItemWidget *)));
}

LocationItemRegionWidget::~LocationItemRegionWidget()
{
    // qDebug() << "Deleting region widget: " << textLabel_->text();

}

LocationID LocationItemRegionWidget::getId()
{
    return regionHeaderWidget_->getId();
}

bool LocationItemRegionWidget::expandable() const
{
    return cities_.count() > 0;
}

bool LocationItemRegionWidget::expanded() const
{
    return expanded_;
}

void LocationItemRegionWidget::setExpanded(bool expand)
{
    if (!expandable())
    {
        qDebug() << "Cannot expand/collapse region without city widgets";
        return;
    }

    // TODO: add animation
    if (expand != expanded_)
    {
        if (expand)
        {
            qDebug() << "Expanding: " << regionHeaderWidget_->name();
            foreach (auto city, cities_)
            {
                city->show();
            }
        }
        else
        {
            qDebug() << "Collapsing: " << regionHeaderWidget_->name();
            foreach (auto city, cities_)
            {
                city->hide();
            }
        }
        expanded_ = expand;
        recalcItemPos();
    }
}

void LocationItemRegionWidget::setShowLatencyMs(bool showLatencyMs)
{
    foreach (auto city, cities_)
    {
        city->setShowLatencyMs(showLatencyMs);
    }
}

void LocationItemRegionWidget::addCity(CityModelItem city)
{
    auto cityWidget = QSharedPointer<LocationItemCityWidget>(new LocationItemCityWidget(city, this));
    connect(cityWidget.get(), SIGNAL(clicked(SelectableLocationItemWidget *)), SLOT(onCityItemClicked(SelectableLocationItemWidget *)));
    connect(cityWidget.get(), SIGNAL(selected(SelectableLocationItemWidget *)), SLOT(onCityItemSelected(SelectableLocationItemWidget *)));
    cityWidget->hide();
    cities_.append(cityWidget);
    recalcItemPos();
}

QList<QSharedPointer<SelectableLocationItemWidget>> LocationItemRegionWidget::selectableWidgets()
{
    QList<QSharedPointer<SelectableLocationItemWidget>> widgets;
    widgets.append(regionHeaderWidget_);
    if (expanded())
    {
        foreach (auto city, cities_)
        {
            widgets.append(city);
        }
    }
    return widgets;
}


void LocationItemRegionWidget::updateScaling()
{
    foreach (auto city, cities_)
    {
        city->updateScaling();
    }
    recalcItemPos();
}

void LocationItemRegionWidget::recalcItemPos()
{
    qDebug() << "Recalc region height";

    int height = LocationItemRegionHeaderWidget::REGION_HEADER_HEIGHT * G_SCALE;

    if (expanded_)
    {
        foreach (auto city, cities_)
        {
            city->setGeometry(0, height, WINDOW_WIDTH * G_SCALE, LocationItemCityWidget::HEIGHT * G_SCALE);
            height += city->geometry().height();
        }
    }

    if (height != height_)
    {
        height_ = height;
        emit heightChanged(height);
    }
    update();
}

void LocationItemRegionWidget::paintEvent(QPaintEvent *event)
{

}

void LocationItemRegionWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    // qDebug() << "Region entered";
}

void LocationItemRegionWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    // qDebug() << "Region left";
    // let the LocationItemListWidget handle unselecting
}

void LocationItemRegionWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // qDebug() << "Mouse move event in region: " << textLabel_->text();

}

void LocationItemRegionWidget::onRegionItemSelected(SelectableLocationItemWidget *regionWidget)
{
    emit selected(regionWidget);
}

void LocationItemRegionWidget::onRegionItemClicked()
{
    emit QAbstractButton::clicked();
}

void LocationItemRegionWidget::onCityItemClicked(LocationItemCityWidget *cityWidget)
{
    emit clicked(cityWidget);
}

void LocationItemRegionWidget::onCityItemSelected(SelectableLocationItemWidget *cityWidget)
{
    emit selected(cityWidget);
}


} // namespace

