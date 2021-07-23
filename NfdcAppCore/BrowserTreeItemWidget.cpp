
#include "stdafx.h"
#include "BrowserTreeItemWidget.h"
#include "BrowserTreeItem.h"

using namespace SIM;

const QString SIM::BrowserTreeItemWidget::OBJ_NAME_WIDGET = "TREE_ITEM_WIDGET";
const QString SIM::BrowserTreeItemWidget::OBJ_NAME_MAIN_LABEL = "TREE_ITEM_MAIN_LABEL";
const QString SIM::BrowserTreeItemWidget::OBJ_NAME_PRECHECK_LABEL = "TREE_ITEM_PRECHECK";
const QString SIM::BrowserTreeItemWidget::OBJ_NAME_ATTRIBUTE_LABEL = "TREE_ITEM_ATTRIBUTE";
const QString SIM::BrowserTreeItemWidget::OBJ_NAME_ICON_LABEL = "TREE_ITEM_ICON";

const char* SIM::BrowserTreeItemWidget::PROP_LABEL_TYPE = "LABEL_TYPE";
const char* SIM::BrowserTreeItemWidget::PROP_ICON_TYPE = "ICON_TYPE";
const char* SIM::BrowserTreeItemWidget::PROP_ACTIVITY_STATE_VALUE = "ACTIVITY_STATE_VALUE";
const char* SIM::BrowserTreeItemWidget::PROP_VISIBILITY_STATE_VALUE = "VISIBILITY_STATE_VALUE";
const char* SIM::BrowserTreeItemWidget::PROP_PRECHECK_VALUE = "PRECHECK_VALUE";

SIM::BrowserTreeItemWidget::BrowserTreeItemWidget(void)	: 
	QWidget(nullptr), 
	_item(nullptr), 
	_layout(nullptr), 
	_label(nullptr), 
	_labelIcon(nullptr),
	_labelPreCheckInfo(nullptr),
	_labelAttributeIcon(nullptr),
	_isActive(false),
	_isVisibleOnView(true)
{
	_isHovered = false;
    installEventFilter(this);

	setObjectName(OBJ_NAME_WIDGET);
    setAttribute(Qt::WA_Hover, true);

	_layout = new QHBoxLayout(this);
	_layout->setMargin(0);
	_layout->setSpacing(0);
	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setAlignment(Qt::AlignVCenter);
	
	_label = new QLabel(this);
	_label->setObjectName(OBJ_NAME_MAIN_LABEL);
    _label->installEventFilter(this);
	_layout->addWidget(_label);

	_layout->addStretch(10000);

	setLayout(_layout);

	SetActive(false, true);
	SetVisibleOnView(true, true);
}

SIM::BrowserTreeItemWidget::~BrowserTreeItemWidget(void)
{}

int SIM::BrowserTreeItemWidget::GetMaxMainLabelWidth(void)
{
	int width = this->width();

	if (_labelIcon && _labelIcon->isVisible())
	{
		width -= _labelIcon->width();
		width -= 8; // left margin = 4px, right margin = 4px
	}

	if (_labelPreCheckInfo && _labelPreCheckInfo->isVisible())
	{
		width -= _labelPreCheckInfo->width();
		width -= 8; // left margin = 4px, right margin = 4px
	}

	if (_labelAttributeIcon && _labelAttributeIcon->isVisible())
	{
		width -= _labelAttributeIcon->width();
		width -= 8; // left margin = 4px, right margin = 4px
	}

	for (auto it : _buttons)
	{
		if (it.second && it.second->isVisible())
		{
			width -= it.second->width();
			width -= 8; // left margin = 4px, right margin = 4px
		}
	}

	return width;
}

void SIM::BrowserTreeItemWidget::SetMainLabel(const QString & label)
{
	_labelText = label;
	setToolTip(_labelText);
	SetMainLabelElidedText();
}

const QString & SIM::BrowserTreeItemWidget::GetMainLabel(void)
{
	return _labelText;
}

QLabel* SIM::BrowserTreeItemWidget::SetMainLabelQss(const QString & qss)
{
	_label->setProperty(PROP_LABEL_TYPE, qss);
	_label->style()->unpolish(_label);
	_label->style()->polish(_label);
	return _label;
}

void SIM::BrowserTreeItemWidget::InitMainIcon(void)
{
	if (!_labelIcon)
	{
		_labelIcon = new QLabel(this);
		_labelIcon->setObjectName(OBJ_NAME_ICON_LABEL);
        _labelIcon->installEventFilter(this);
        _layout->insertWidget(_layout->indexOf(_label), _labelIcon);
	}
}

QLabel* SIM::BrowserTreeItemWidget::SetMainIcon(const QString & url)
{
	SetMainIconQss("");

	if (url.size() > 0)
	{
		_labelIcon->setPixmap(QPixmap(url));
	}

	return _labelIcon;
}

QLabel* SIM::BrowserTreeItemWidget::SetMainIconQss(const QString & qss)
{
	InitMainIcon();

	_labelIcon->clear();
	_labelIcon->setProperty(PROP_ICON_TYPE, qss);
	
	_labelIcon->style()->unpolish(_labelIcon);
	_labelIcon->style()->polish(_labelIcon);

	return _labelIcon;
}

void SIM::BrowserTreeItemWidget::SetActive(bool active, bool force)
{
	if ((active != _isActive) || force)
	{
		_isActive = active;
		setProperty(PROP_ACTIVITY_STATE_VALUE, QVariant::fromValue<int>(_isActive ? 1 : 0));
		RefreshStyle();
		VerifyButtonsVisibility();
	}
}

void SIM::BrowserTreeItemWidget::SetVisibleOnView(bool visible, bool force)
{
	if ((visible != _isVisibleOnView) || force)
	{
		_isVisibleOnView = visible;
		setProperty(PROP_VISIBILITY_STATE_VALUE, QVariant::fromValue<int>(_isVisibleOnView? 1 : 0));
		RefreshStyle();
		VerifyButtonsVisibility();
	}
}

void SIM::BrowserTreeItemWidget::RefreshStyle(void)
{
	style()->unpolish(this);
	style()->polish(this);

	if (_label)
	{
		_label->style()->unpolish(_label);
		_label->style()->polish(_label);
	}

	if (_labelIcon)
	{
		_labelIcon->style()->unpolish(_labelIcon);
		_labelIcon->style()->polish(_labelIcon);
	}
	for (auto it : _buttons)
	{
		it.second->style()->unpolish(it.second);
		it.second->style()->polish(it.second);
	}
}

QLabel* SIM::BrowserTreeItemWidget::SetPreCheckInfo(PreCheckInfo info, const QString & iconUrl)
{
	if (!_labelPreCheckInfo)
	{
		_labelPreCheckInfo = new QLabel(this);
		_labelPreCheckInfo->setObjectName(OBJ_NAME_PRECHECK_LABEL);
        _labelPreCheckInfo->installEventFilter(this);
		_layout->insertWidget(_layout->indexOf(_label) + (_labelAttributeIcon ? 2 : 1), _labelPreCheckInfo);
	}

	_labelPreCheckWasVisible = info != PreCheckInfo::None;
	_labelPreCheckInfo->setVisible(_labelPreCheckWasVisible);

	setProperty(PROP_PRECHECK_VALUE, QVariant::fromValue<int>((int)info));
	_labelPreCheckInfo->clear();
	
	_labelPreCheckInfo->style()->unpolish(_labelPreCheckInfo);
	_labelPreCheckInfo->style()->polish(_labelPreCheckInfo);

	if (iconUrl.size() > 0)
	{
		_labelPreCheckInfo->setPixmap(QPixmap(iconUrl));
	}
	
	return _labelPreCheckInfo;
}

QLabel* SIM::BrowserTreeItemWidget::SetAttributeLabel(bool visible, const QString & url)
{
	if (!_labelAttributeIcon)
	{
		_labelAttributeIcon = new QLabel(this);
		_labelAttributeIcon->setObjectName(OBJ_NAME_ATTRIBUTE_LABEL);
        _labelAttributeIcon->installEventFilter(this);
		_layout->insertWidget(_layout->indexOf(_label) + 1, _labelAttributeIcon);
	}

	_labelAttributeIcon->setVisible(visible);
	if (url.size() > 0)
	{
		_labelAttributeIcon->setPixmap(QPixmap(url));
	}

	return _labelAttributeIcon;
}

QPushButton* SIM::BrowserTreeItemWidget::AddButton(int code, VisibilityPolicy policy, const QString & qssObjectName, const QString & tooltip)
{
	QPushButton* btn = new QPushButton(this);
    btn->installEventFilter(this);

	bool connected = connect(btn, &QPushButton::clicked, [this, code](bool checked) {
		this->OnButtonClicked(code);
	});

	if (qssObjectName.length() > 0)
	{
		btn->setObjectName(qssObjectName);
	}

	if (tooltip.length() > 0)
	{
		btn->setToolTip(tooltip);
	}

	_buttons.insert(std::pair<int, QPushButton*>(code, btn));
	_buttonVisibility.insert(std::pair<int, VisibilityPolicy>(code, policy));

	CheckButtonVisibility(code);
	_layout->addWidget(btn);

	return btn;
}

void SIM::BrowserTreeItemWidget::CheckButtonVisibility(int code)
{
    if (_labelPreCheckInfo)
    {
        if (_isHovered)
        {
            if (_labelPreCheckInfo->isVisible())
            {
                _labelPreCheckWasVisible = true;
                _labelPreCheckInfo->setVisible(false);
            }
        }
        else
        {
            _labelPreCheckInfo->setVisible(_labelPreCheckWasVisible);
        }
    }

    bool toBeVisible = false;

    switch (_buttonVisibility[code])
    {
    case VisibilityPolicy::AlwaysVisible:
        toBeVisible = true;
        break;
    case VisibilityPolicy::VisibleWhenActive:
        toBeVisible = _item && _item->IsActive();
		break;
	case VisibilityPolicy::VisibleWhenHovered:
		toBeVisible = _isHovered;
		break;
	case VisibilityPolicy::VisibleWhenActiveOrHovered:
		toBeVisible = (_item && _item->IsActive()) || _isHovered;
		break;
	default:
		break;
	}

    if (toBeVisible)
    {
        _buttons[code]->show();
    }
    else
    {
        _buttons[code]->hide();
    }
}

bool SIM::BrowserTreeItemWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this)
    {
        switch (event->type())
        {
        case QEvent::Enter:
            OnHoverEnterEvent(event);
            break;
        case QEvent::Leave:
            OnHoverLeaveEvent(event);
            break;
        default:
            break;
        }
    }

    return false;
}

void SIM::BrowserTreeItemWidget::OnHoverLeaveEvent(QEvent* event)
{
	_isHovered = false;
    _isHoverStateChanged = true;
}

void SIM::BrowserTreeItemWidget::OnHoverEnterEvent(QEvent* event)
{
	_isHovered = true;
    _isHoverStateChanged = true;
}

void SIM::BrowserTreeItemWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
    
    if (_isHoverStateChanged)
    {
        VerifyButtonsVisibility();
        _isHoverStateChanged = false;
    }

	SetMainLabelElidedText();
}

void SIM::BrowserTreeItemWidget::SetMainLabelElidedText(void)
{
	QFontMetrics metrics(_label->font());

	_label->setText(_labelText);
	int width = GetMaxMainLabelWidth();

	QString elidedText = _labelText;
	elidedText = metrics.elidedText(elidedText, Qt::ElideRight, width);
	std::string txt = elidedText.toStdString();

	_label->setText(elidedText);
}

void SIM::BrowserTreeItemWidget::VerifyButtonsVisibility(void)
{
	for (auto it : _buttons)
	{
		CheckButtonVisibility(it.first);
	}
}

void SIM::BrowserTreeItemWidget::OnButtonClicked(int code)
{
	emit ButtonClicked(_item, code);
}

