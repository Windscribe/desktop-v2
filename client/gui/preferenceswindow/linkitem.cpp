#include "linkitem.h"

#include <QDesktopServices>
#include <QPainter>
#include <QUrl>

#include "graphicresources/fontmanager.h"
#include "graphicresources/imageresourcessvg.h"
#include "graphicresources/independentpixmap.h"
#include "dpiscalemanager.h"
#include "preferencesconst.h"

namespace PreferencesWindow {

LinkItem::LinkItem(ScalableGraphicsObject *parent, LinkType type, const QString &title, const QString &url)
  : BaseItem(parent, PREFERENCE_GROUP_ITEM_HEIGHT*G_SCALE), title_(title), url_(url), linkText_(""), type_(type), icon_(nullptr), linkIcon_(nullptr), inProgress_(false), spinnerRotation_(0)
{
    curArrowOpacity_= OPACITY_HALF;
    if (type == LinkType::TEXT_ONLY) {
        curTextOpacity_ = OPACITY_FULL;
    } else {
        setClickable(true);
        curTextOpacity_ = OPACITY_HALF;
        connect(this, &BaseItem::clicked, this, &LinkItem::onOpenUrl);
        connect(this, &BaseItem::hoverEnter, this, &LinkItem::onHoverEnter);
        connect(this, &BaseItem::hoverLeave, this, &LinkItem::onHoverLeave);
        connect(&textOpacityAnimation_, &QVariantAnimation::valueChanged, this, &LinkItem::onTextOpacityChanged);
        connect(&arrowOpacityAnimation_, &QVariantAnimation::valueChanged, this, &LinkItem::onArrowOpacityChanged);
        connect(&spinnerAnimation_, &QVariantAnimation::valueChanged, this, &LinkItem::onSpinnerRotationChanged);
        connect(&spinnerAnimation_, &QVariantAnimation::finished, this, &LinkItem::onSpinnerRotationFinished);
    }
}

void LinkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // link text
    QFont font = FontManager::instance().getFont(12, true);
    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->setOpacity(curTextOpacity_);

    int linkTextWidth = CommonGraphics::textWidth(linkText_, font);
    int linkTextPosX = boundingRect().width() - linkTextWidth - PREFERENCES_MARGIN*G_SCALE;
    if (type_ == LinkType::EXTERNAL_LINK || type_ == LinkType::SUBPAGE_LINK)
    {
        linkTextPosX -= 19*G_SCALE;
    }
    painter->drawText(boundingRect().adjusted(linkTextPosX, PREFERENCES_MARGIN*G_SCALE, -PREFERENCES_MARGIN*G_SCALE, -PREFERENCES_MARGIN*G_SCALE), Qt::AlignLeft, linkText_);

    // arrow or external link
    QSharedPointer<IndependentPixmap> p;
    if (inProgress_) {
        painter->setOpacity(1.0);
        p = ImageResourcesSvg::instance().getIndependentPixmap("SPINNER");
        painter->save();
        painter->translate(static_cast<int>(boundingRect().width() - p->width()/2 - PREFERENCES_MARGIN*G_SCALE), PREFERENCES_MARGIN*G_SCALE + p->height()/2);
        painter->rotate(spinnerRotation_);
        p->draw(-p->width()/2, -p->height()/2, painter);
        painter->restore();
    } else {
        painter->setOpacity(curArrowOpacity_);
        if (type_ == LinkType::EXTERNAL_LINK) {
            p = linkIcon_ ? linkIcon_ : ImageResourcesSvg::instance().getIndependentPixmap("preferences/EXTERNAL_LINK_ICON");
            p->draw(static_cast<int>(boundingRect().width() - p->width() - PREFERENCES_MARGIN*G_SCALE), static_cast<int>((boundingRect().height() - p->height()) / 2), painter);
        } else if (type_ == LinkType::SUBPAGE_LINK) {
            p = linkIcon_ ? linkIcon_ : ImageResourcesSvg::instance().getIndependentPixmap("preferences/FRWRD_ARROW_WHITE_ICON");
            p->draw(static_cast<int>(boundingRect().width() - p->width() - PREFERENCES_MARGIN*G_SCALE), static_cast<int>((boundingRect().height() - p->height()) / 2), painter);
        }
    }

    // text
    int xOffset = PREFERENCES_MARGIN*G_SCALE;
    if (icon_)
    {
        painter->setOpacity(OPACITY_FULL);
        icon_->draw(PREFERENCES_MARGIN*G_SCALE, PREFERENCES_MARGIN*G_SCALE, ICON_WIDTH*G_SCALE, ICON_HEIGHT*G_SCALE, painter);
        xOffset = (2*PREFERENCES_MARGIN + ICON_WIDTH)*G_SCALE;
    }
    else
    {
        painter->setOpacity(curTextOpacity_);
    }

    QFontMetrics fm(font);
    painter->drawText(boundingRect().adjusted(xOffset,
                                              PREFERENCES_MARGIN*G_SCALE,
                                              -PREFERENCES_MARGIN*G_SCALE,
                                              -PREFERENCES_MARGIN*G_SCALE),
                      Qt::AlignLeft,
                      fm.elidedText(title_, Qt::ElideRight, linkTextPosX - xOffset - PREFERENCES_MARGIN*G_SCALE));
}

void LinkItem::hideOpenPopups()
{
    // overwrite
}

QString LinkItem::title()
{
    return title_;
}

void LinkItem::setTitle(const QString &title)
{
    title_ = title;
    update();
}

QString LinkItem::linkText()
{
    return linkText_;
}

void LinkItem::setLinkText(const QString &text)
{
    linkText_ = text;
    update();
}

void LinkItem::updateScaling()
{
    BaseItem::updateScaling();
    setHeight(PREFERENCE_GROUP_ITEM_HEIGHT*G_SCALE);
}

void LinkItem::onHoverEnter()
{
    startAnAnimation<double>(textOpacityAnimation_, curTextOpacity_, OPACITY_FULL, ANIMATION_SPEED_FAST);
    startAnAnimation<double>(arrowOpacityAnimation_, curArrowOpacity_, OPACITY_FULL, ANIMATION_SPEED_FAST);
}

void LinkItem::onHoverLeave()
{
    startAnAnimation<double>(textOpacityAnimation_, curTextOpacity_, OPACITY_HALF, ANIMATION_SPEED_FAST);
    startAnAnimation<double>(arrowOpacityAnimation_, curArrowOpacity_, OPACITY_HALF, ANIMATION_SPEED_FAST);
}

void LinkItem::onTextOpacityChanged(const QVariant &value)
{
    curTextOpacity_ = value.toDouble();
    update();
}

void LinkItem::onArrowOpacityChanged(const QVariant &value)
{
    curArrowOpacity_ = value.toDouble();
    update();
}

void LinkItem::setUrl(const QString &url)
{
    url_ = url;
}

void LinkItem::onOpenUrl()
{
    // this call does nothing when url is empty string, as is the case for edit account details
    QDesktopServices::openUrl(QUrl(url_));
}

void LinkItem::setIcon(QSharedPointer<IndependentPixmap> icon)
{
    icon_ = icon;
    update();
}

void LinkItem::setLinkIcon(QSharedPointer<IndependentPixmap> icon)
{
    linkIcon_ = icon;
    update();
}

void LinkItem::setInProgress(bool inProgress)
{
    inProgress_ = inProgress;
    if (inProgress_) {
        spinnerRotation_ = 0;
        startAnAnimation<int>(spinnerAnimation_, spinnerRotation_, 360, ANIMATION_SPEED_VERY_SLOW);
    } else {
        spinnerAnimation_.stop();
        update();
    }
}

void LinkItem::onSpinnerRotationChanged(const QVariant &value)
{
    spinnerRotation_ = value.toInt();
    update();
}

void LinkItem::onSpinnerRotationFinished()
{
    startAnAnimation<int>(spinnerAnimation_, 0, 360, ANIMATION_SPEED_VERY_SLOW);
}

} // namespace PreferencesWindow
