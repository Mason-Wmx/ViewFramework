#include "stdafx.h"
#include "BaseDialog.h"
#include "../NfdcAppCore/Application.h"

BaseDialog::BaseDialog(QWidget *parent, bool bHeader/* = true*/)
	: QDialog(parent),
	m_DialogFrame(NULL),
	m_pDialogLayout(NULL),
	m_pMainFrame(NULL),
	m_pMainLayout(NULL),
	m_pHeaderFrame(NULL),
	m_pTitleLabel(NULL),
	m_pTitleLayout(NULL),
	m_pBottomLayout(NULL),
	m_pOKButton(NULL),
	m_pCancelButton(NULL),
	m_pointMouseOffset(0, 0),
	m_bMouseMoving(false)
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setStyleSheet(getDialogStyle());
	setEnabled(true);
	setContentsMargins(0, 0, 0, 0);

	BaseDialog::SetupMainLayout(bHeader);
}


BaseDialog::~BaseDialog()
{

}

void BaseDialog::OkButtonClicked()
{
	OnOk();
	this->accept();
}

void BaseDialog::CancelButtonClicked()
{
	OnCancel();
	this->reject();
}

void BaseDialog::ApplyButtonClicked()
{
	OnApply();
}
void BaseDialog::CreateHeaderFrame()
{
	m_pHeaderFrame = new QFrame(this);
	m_pHeaderFrame->setObjectName("HeaderFrame");
	m_pTitleLayout = new QHBoxLayout(m_pHeaderFrame);
	m_pHeaderFrame->setLayout(m_pTitleLayout);

	m_pTitleLayout->setObjectName("titleHorizontalLayout");
	m_pTitleLayout->setSizeConstraint(QLayout::SetFixedSize);

	m_pTitleLayout->addSpacerItem(new QSpacerItem(0, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));

	m_pTitleLabel = new QLabel(m_pHeaderFrame);
	m_pTitleLabel->setObjectName("titleLabel");
	m_pTitleLabel->setText(windowTitle());
	m_pTitleLayout->addWidget(m_pTitleLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
}

void BaseDialog::CreateBottomFrame(bool bOKCancel, bool bApply)
{
	m_pBotFrameDivider = new QFrame(this);
	m_pBottomVBoxLayout = new QVBoxLayout(m_pMainFrame);
	m_pBotFrameDivider->setFrameShape(QFrame::HLine);
	m_pBotFrameDivider->setObjectName("separator1");
	m_pBottomVBoxLayout->addWidget(m_pBotFrameDivider);
	QFrame* pBottomFrame = new QFrame(this);
	pBottomFrame->setObjectName("BottomFrame");
	m_pBottomLayout = new QHBoxLayout(pBottomFrame);
	m_pBottomLayout->setContentsMargins(0, 0, 14, 14);
	m_pBottomLayout->setSpacing(22);

	if (bOKCancel || bApply)
	{
		QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		m_pBottomLayout->addItem(horizontalSpacer);
	}
	if (bApply)
	{
		m_pApplyButton = new QPushButton(pBottomFrame);
		m_pApplyButton->setObjectName(QString::fromUtf8("ApplyButton"));
		m_pBottomLayout->addWidget(m_pApplyButton);
		m_pApplyButton->setText(QApplication::translate("ApplyButton", "Apply", 0));
		connect(m_pApplyButton, SIGNAL(clicked()), this, SLOT(ApplyButtonClicked()));
	}
	if (bOKCancel)
	{
		m_pOKButton = new QPushButton(pBottomFrame);
		m_pOKButton->setObjectName(QString::fromUtf8("OKButton"));
		m_pBottomLayout->addWidget(m_pOKButton);
		m_pOKButton->setText(QApplication::translate("BaseDialog", "OK", 0));
		connect(m_pOKButton, SIGNAL(clicked()), this, SLOT(OkButtonClicked()));

		m_pCancelButton = new QPushButton(pBottomFrame);
		m_pCancelButton->setObjectName(QString::fromUtf8("CancelButton"));
		m_pBottomLayout->addWidget(m_pCancelButton);
		m_pCancelButton->setText(QApplication::translate("BaseDialog", "Cancel", 0));
		connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonClicked()));
	}

	pBottomFrame->setLayout(m_pBottomLayout);


	m_pBottomVBoxLayout->addWidget(pBottomFrame);
	m_pBottomVBoxLayout->setAlignment(Qt::AlignBottom);
	m_pBottomVBoxLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	m_pMainLayout->addLayout(m_pBottomVBoxLayout);
}

void BaseDialog::SetLayoutStyle(QLayout* layout)
{
	if (layout) {
		layout->setSpacing(0);
		layout->setMargin(0);
	}
}

void BaseDialog::SetupMainLayout(bool bHeader)
{
	m_DialogFrame = new QFrame(this);
	m_DialogFrame->setObjectName("DialogFrame");
	m_pDialogLayout = new QHBoxLayout(m_DialogFrame);
	m_DialogFrame->setLayout(m_pDialogLayout);


	m_pMainFrame = new QFrame(this);
	m_pMainFrame->setObjectName("MainFrame");
	m_pMainLayout = new QVBoxLayout(m_pMainFrame);
	m_pMainLayout->setContentsMargins(8, 8, 8, 8);
	m_pMainFrame->setLayout(m_pMainLayout);

	m_pDialogLayout->addLayout(m_pMainLayout);

	GetDialogLayout()->addWidget(m_pMainFrame, 0, Qt::AlignLeft);

	setLayout(m_pDialogLayout);
	BaseDialog::SetLayoutStyle(m_pDialogLayout);

    if (!bHeader)
        return;
	CreateHeaderFrame();
	if (m_pHeaderFrame == nullptr)
		return;

	m_pHeaderVBoxLayout = new QVBoxLayout(m_pMainFrame);
	m_pHeaderVBoxLayout->addWidget(m_pHeaderFrame);

	QFrame *pFrameDivider = new QFrame(this);
	pFrameDivider->setFrameShape(QFrame::HLine);
	pFrameDivider->setObjectName("separator1");
	m_pHeaderVBoxLayout->addWidget(pFrameDivider);
	m_pHeaderVBoxLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	m_pMainLayout->addLayout(m_pHeaderVBoxLayout);
}

void BaseDialog::SetHeaderTitle(const QString& sTitle)
{
	m_pTitleLabel->setText(sTitle);
}

void BaseDialog::SetHeaderVisible(bool visible)
{
    for (int i = 0; i < m_pHeaderVBoxLayout->count(); ++i) {
        QWidget *widget = m_pHeaderVBoxLayout->itemAt(i)->widget();
        
        if (widget)
            widget->setVisible(visible);
    }
}

void
BaseDialog::mousePressEvent(QMouseEvent* event)
{
    QDialog::mousePressEvent(event);

	if (event->button() != Qt::LeftButton)
		return;

	m_bMouseMoving = true;
	m_pointMouseOffset = event->pos();
}

void
BaseDialog::mouseMoveEvent(QMouseEvent* event)
{
    QDialog::mouseMoveEvent(event);

	if (m_bMouseMoving) {
		move(event->globalPos() - m_pointMouseOffset);
	}
}

void
BaseDialog::mouseReleaseEvent(QMouseEvent* event)
{
    QDialog::mouseReleaseEvent(event);

	if (event->button() != Qt::LeftButton)
		return;
	m_bMouseMoving = false;
}

QString
BaseDialog::getDialogStyle()
{
	QString path = QString::fromStdString(SIM::Application::GetFileInStandardLocation("Dialog.qss", "styles"));
	QFile file(path);
	file.open(QFile::ReadOnly);

	QString result = QLatin1String(file.readAll());
	file.close();

	return result;
}

