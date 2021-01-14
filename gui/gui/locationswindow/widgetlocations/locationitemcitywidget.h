#ifndef LOCATIONITEMCITYWIDGET_H
#define LOCATIONITEMCITYWIDGET_H

#include <QLabel>
#include "../backend/locationsmodel/basiclocationsmodel.h"
#include "selectablelocationitemwidget.h"

namespace GuiLocations {

class LocationItemCityWidget : public SelectableLocationItemWidget
{
    Q_OBJECT
public:
    explicit LocationItemCityWidget(CityModelItem cityModelItem, QWidget *parent = nullptr);
    ~LocationItemCityWidget();

    const QString name() const override;
    SelectableLocationItemWidgetType type() override;
    void setSelected(bool select) override;
    bool isSelected() const override;
    bool containsCursor() const override;

    void setShowLatencyMs(bool showLatencyMs);
    void updateScaling();

    static const int HEIGHT = 50;

signals:
    void selected(SelectableLocationItemWidget *itemWidget);
    void clicked(SelectableLocationItemWidget *itemWidget);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QSharedPointer<QLabel> cityLabel_;
    QSharedPointer<QLabel> nickLabel_;
    CityModelItem cityModelItem_;

    bool selected_;
    const QString labelStyleSheetWithOpacity(double opacity);


};

}

#endif // LOCATIONITEMCITYWIDGET_H

