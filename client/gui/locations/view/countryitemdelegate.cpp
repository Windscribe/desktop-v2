#include "countryitemdelegate.h"
#include "dpiscalemanager.h"
#include "../locationsmodel_roles.h"
#include "graphicresources/fontmanager.h"
#include "graphicresources/imageresourcessvg.h"
#include "commongraphics/commongraphics.h"
#include "types/locationid.h"
#include <QPainter>
#include <QtMath>

namespace gui_locations {

void CountryItemDelegate::paint(QPainter *painter, const ItemStyleOption &option, const QModelIndex &index) const
{
    painter->save();

    // background
    painter->fillRect(option.rect, FontManager::instance().getMidnightColor());

    int left_offs = option.rect.left();
    int top_offs = option.rect.top();

    // flag
    QSharedPointer<IndependentPixmap> flag = ImageResourcesSvg::instance().getFlag(index.data(COUNTRY_CODE).toString());
    if (flag)
    {
        const int pixmapFlagHeight = flag->height();
        flag->draw(left_offs + LOCATION_ITEM_MARGIN*G_SCALE, top_offs + (option.rect.height() - pixmapFlagHeight) / 2, painter);
    }

    // pro star
    if (index.data(IS_SHOW_AS_PREMIUM).toBool())
    {
        QSharedPointer<IndependentPixmap> proRegionStar = ImageResourcesSvg::instance().getIndependentPixmap("locations/PRO_REGION_STAR_LIGHT");
        proRegionStar->draw(left_offs + 8 * G_SCALE, top_offs + (option.rect.height() - 16*G_SCALE) / 2 - 9*G_SCALE, painter);
    }

    double textOpacity = OPACITY_UNHOVER_TEXT + (OPACITY_FULL - OPACITY_UNHOVER_TEXT) * option.selectedOpacity();

    // text
    painter->setOpacity(textOpacity);
    painter->setPen(Qt::white);
    painter->setFont(*FontManager::instance().getFont(16, true));
    QRect rc = option.rect;
    rc.adjust(64*G_SCALE, 0, 0, 0);
    painter->drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());

    // p2p icon
    if (index.data(IS_SHOW_P2P).toBool())
    {
        painter->setOpacity(OPACITY_HALF);

        QSharedPointer<IndependentPixmap> p = ImageResourcesSvg::instance().getIndependentPixmap("locations/NO_P2P_ICON");
        QRect p2pr = QRect(option.rect.width() - 65*G_SCALE,
                           (option.rect.height() - p->height()) / 2,
                           p->width(),
                           p->height());
        p->draw(left_offs + p2pr.x(), top_offs + p2pr.y(), painter);
    }

    LocationID lid = qvariant_cast<LocationID>(index.data(LOCATION_ID));
    if (lid.isBestLocation())
    {
        // 10gbps icon
        if (index.data(IS_10GBPS).toBool())
        {
            painter->setOpacity(OPACITY_FULL);
            QSharedPointer<IndependentPixmap> tenGbpsPixmap = ImageResourcesSvg::instance().getIndependentPixmap("locations/10_GBPS_ICON");
            tenGbpsPixmap->draw(left_offs + option.rect.width() - LOCATION_ITEM_MARGIN*G_SCALE - tenGbpsPixmap->width(), top_offs + (option.rect.height() - tenGbpsPixmap->height()) / 2, painter);
        }
    }
    // plus/cross
    else
    {
        double plusIconOpacity_ = OPACITY_THIRD + (OPACITY_FULL - OPACITY_THIRD) * option.selectedOpacity();
        painter->setOpacity(plusIconOpacity_);
        QSharedPointer<IndependentPixmap> expandPixmap = ImageResourcesSvg::instance().getIndependentPixmap("locations/EXPAND_ICON");

        // this part is kind of magical - could use some more clear math
        painter->save();
        painter->translate(QPoint(left_offs + option.rect.width() - LOCATION_ITEM_MARGIN*G_SCALE - expandPixmap->width()/2, top_offs + option.rect.height()/2));
        painter->rotate(45 * option.expandedProgress());
        expandPixmap->draw( - expandPixmap->width() / 2,
                           - expandPixmap->height()/ 2,
                            painter);
        painter->restore();
    }

    // bottom lines
    int left = left_offs + static_cast<int>(24 * G_SCALE);
    int right = left_offs + static_cast<int>(option.rect.width() - 8*G_SCALE);
    int bottom = top_offs + option.rect.height() - 1; // 1 is not scaled since we want bottom-most pixel inside geometry
    painter->setOpacity(1.0);

    // TODO: lines not scaled since we draw just single pixels
    // background line (darker line)
    QPen pen(QColor(0x29, 0x2E, 0x3E));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(left, bottom - 1, right, bottom - 1);
    painter->drawLine(left, bottom, right, bottom);


    if (option.isShowLocationLoad())
    {
        int locationLoad = index.data(LOAD).toInt();
        if (locationLoad > 0)
        {
            Qt::GlobalColor penColor;
            if (locationLoad < 60) {
                penColor = Qt::green;
            }
            else if (locationLoad < 90) {
                penColor = Qt::yellow;
            }
            else {
                penColor = Qt::red;
            }
            int rightX = left + ((right - left) * locationLoad / 100);
            QPen penLoad(penColor);
            penLoad.setWidth(1);
            painter->setOpacity(textOpacity);
            painter->setPen(penLoad);
            painter->drawLine(left, bottom - 1, rightX, bottom - 1);
            painter->drawLine(left, bottom, rightX, bottom);
            painter->setOpacity(1.0);
        }
    }

    // top-most line (white)
    if( qFabs(1.0 - option.expandedProgress()) < 0.000001 )
    {
        QPen white_pen(Qt::white);
        white_pen.setWidth(1);
        painter->setPen(white_pen);
        painter->drawLine(left, bottom, right, bottom);
        painter->drawLine(left, bottom - 1, right, bottom - 1);
    }
    else if (option.expandedProgress() > 0.000001)
    {
        int w = (right - left) * option.expandedProgress();
        QPen white_pen(Qt::white);
        white_pen.setWidth(1);
        painter->setPen(white_pen);
        painter->drawLine(left, bottom, left + w, bottom);
        painter->drawLine(left, bottom - 1, left + w, bottom - 1);
    }

    painter->restore();
}

QSize CountryItemDelegate::sizeHint(const QModelIndex &index) const
{
    return QSize(WINDOW_WIDTH * G_SCALE, LOCATION_ITEM_HEIGHT * G_SCALE);
}

bool CountryItemDelegate::isForbiddenCursor(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return false;
    }

    LocationID lid = qvariant_cast<LocationID>(index.data(LOCATION_ID));
    if (!lid.isBestLocation())
    {
        // if no child items
        return (!index.model()->index(0, 0, index).isValid());
    }
    return false;
}

int CountryItemDelegate::isInClickableArea(const QModelIndex &index, const QPoint &point) const
{
    return -1;
}

int CountryItemDelegate::isInTooltipArea(const QModelIndex &index, const QPoint &point) const
{
    return -1;
}

} // namespace gui_locations
