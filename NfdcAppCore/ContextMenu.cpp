#include "stdafx.h"
#include "ContextMenu.h"

using namespace SIM;



SIM::ContextMenuAction::ContextMenuAction(const QString & title, ContextMenu* parent) :
    QAction(parent->Elide(title), parent),
    _title(title)
{
}

/*
QWidget* SIM::ContextMenuAction::createWidget(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	widget->setObjectName("ContextMenuAction");
	widget->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	bool hasLeadingIcon = false;
	ContextMenu* menu = dynamic_cast<ContextMenu*>(parent);
	if (menu)
	{
		hasLeadingIcon = menu->HasBeforeIcons();
	}

	if (hasLeadingIcon)
	{
		QLabel* iconLabel = new QLabel(widget);
		if (icon().isNull())
		{
			QPixmap pmap(16, 16);
			pmap.fill(Qt::transparent);
			iconLabel->setPixmap(pmap);
		}
		else
		{
			QPixmap pmap = icon().pixmap(16, 16);
			iconLabel->setPixmap(pmap);
		}

		layout->addWidget(iconLabel);
	}

	QLabel* label = new QLabel(widget);
	label->setText(_title);

	int num = _title.count('\n');
	num = num > 2 ? 2 : num;
	label->setProperty("LINE_BREAKS", QVariant::fromValue<int>(num));

	layout->addWidget(label);

	widget->setLayout(layout);
	return widget;
}
*/

void SIM::ContextMenuAction::setText(const QString & text)
{
    ContextMenu* menu = dynamic_cast<ContextMenu*>(parent());
    if (menu)
    {
        QAction::setText(menu->Elide(text));
    }
}

void SIM::ContextMenuAction::setCheckable(bool checkable)
{
    QAction::setCheckable(checkable);

    ContextMenu* menu = dynamic_cast<ContextMenu*>(parent());
    if (menu)
    {
        menu->SetHasBeforeIcons(true);        
    }
}

void SIM::ContextMenuAction::setIcon(const QIcon & newIcon)
{
	QAction::setIcon(newIcon);

	ContextMenu* menu = dynamic_cast<ContextMenu*>(parent());
	if (menu)
	{
		if (!icon().isNull())
		{
			menu->SetHasBeforeIcons(true);
		}
	}
}

void SIM::ContextMenuAction::setShortcut(const QKeySequence& keySequence)
{
    QAction::setShortcut(keySequence);

    ContextMenu* menu = dynamic_cast<ContextMenu*>(parent());
    if (menu)
    {
        if (!keySequence.isEmpty())
        {
            menu->SetHasAfterIcons(true);
        }
    }
}

SIM::ContextMenu::ContextMenu(QWidget* parent) :
	QMenu(parent)
{
	Initialize();
}

SIM::ContextMenu::ContextMenu(const QString & title, ContextMenu* parent) :
	QMenu(parent->Elide(title), parent)
{
	Initialize();
}

void SIM::ContextMenu::Initialize(void)
{
    bool connected = connect(this, SIGNAL(aboutToShow()), this, SLOT(OnAboutToShow()));

	SetHasBeforeIcons(false);
    SetHasAfterIcons(false);
    SetHasElidedLabel(false);
	SetSubmenuVisible(false);
}

void SIM::ContextMenu::SetHasElidedLabel(bool elided)
{
    _hasElidedLabel = elided;
    setProperty("HAS_ELIDED_LABEL", QVariant::fromValue<int>(_hasElidedLabel ? 1 : 0));
}

void SIM::ContextMenu::SetHasBeforeIcons(bool hasIcons) 
{ 
    _hasBeforeIcons = hasIcons;
	setProperty("HAS_BEFORE_ICONS", QVariant::fromValue<int>(hasIcons ? 1 : 0));
}

void SIM::ContextMenu::SetHasAfterIcons(bool hasIcons)
{
    _hasAfterIcons = hasIcons;
    setProperty("HAS_AFTER_ICONS", QVariant::fromValue<int>(hasIcons ? 1 : 0));
}

void SIM::ContextMenu::addMenu(ContextMenu* menu)
{
	bool connected = connect(menu, SIGNAL(aboutToShow()), this, SLOT(OnSubmenuAboutToShow()));
	connected      = connect(menu, SIGNAL(aboutToHide()), this, SLOT(OnSubmenuAboutToHide()));
	QMenu::addMenu(menu);
    SetHasAfterIcons(true);
}

ContextMenuAction * SIM::ContextMenu::addContextMenuAction(const QString & text, bool isCheckable)
{
    ContextMenuAction* action = new ContextMenuAction(text, this);
    action->setCheckable(isCheckable);
    QMenu::addAction(action);

    return action;
}

void SIM::ContextMenu::OnAboutToShow(void)
{
    SetItemWidthStyle();
}

void SIM::ContextMenu::OnSubmenuAboutToShow(void)
{
	SetSubmenuVisible(true);
}

void SIM::ContextMenu::OnSubmenuAboutToHide(void)
{
	SetSubmenuVisible(false);
}

void SIM::ContextMenu::SetSubmenuVisible(bool visible)
{
    setProperty("HAS_SUBMENU_VISIBLE", QVariant::fromValue<int>(visible ? 1 : 0));
    SetItemWidthStyle();
}

QString SIM::ContextMenu::Elide(const QString & text)
{
    QString myTxt = text;
    int ampersandIndex = myTxt.indexOf('&');
    myTxt = myTxt.remove('&');

    QFont font("Artifakt Element");
    font.setPixelSize(12);

    QFontMetrics metrics(font);
    QString elidedTxt = metrics.elidedText(myTxt, Qt::ElideRight, CONTEXT_MENU_ITEM_MAX_WIDTH);

    bool isElided = elidedTxt != myTxt;
    if (isElided)
        SetHasElidedLabel(true);

    _minItemWidth = std::max(_minItemWidth, metrics.width(elidedTxt));

    if (ampersandIndex != -1 && (!isElided || ampersandIndex < elidedTxt.length() - 3))
        elidedTxt = elidedTxt.insert(ampersandIndex, '&');
    
    return elidedTxt;
}

void SIM::ContextMenu::SetItemWidthStyle()
{
    int width = _minItemWidth;

    if (HasAfterIcons())
    {
        // whitespace
        width += 20;
    }

    std::string style = "SIM--ContextMenu::item{min-width:" + std::to_string(width) + "px ;}";
    qDebug() << style.c_str();
    setStyleSheet(style.c_str());
}

