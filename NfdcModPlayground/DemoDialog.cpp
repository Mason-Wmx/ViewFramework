#include "stdafx.h"
#include "DemoDialog.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include <QtWidgets/QRadioButton>

DemoDialog::DemoDialog(QWidget *parent)
	: BaseDialog(parent)
{
	BaseDialog::SetHeaderTitle(QApplication::translate("DemoStyle", "DemoDialog", 0));
	QVBoxLayout* pMainLayout = GetMainLayout();

	QVBoxLayout* verticalLayout = new QVBoxLayout();
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	QLabel* label = new QLabel(parent);
	label->setObjectName(QString::fromUtf8("label"));

	horizontalLayout->addWidget(label);

	QLineEdit* lineEdit = new QLineEdit(parent);
	lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

	horizontalLayout->addWidget(lineEdit);
	verticalLayout->addLayout(horizontalLayout);

	QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	QLabel* label_2 = new QLabel(parent);
	label_2->setObjectName(QString::fromUtf8("label_2"));

	horizontalLayout_2->addWidget(label_2);

	QLineEdit* lineEdit_2 = new QLineEdit(parent);
	lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

	horizontalLayout_2->addWidget(lineEdit_2);
	verticalLayout->addLayout(horizontalLayout_2);

	QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
	horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	QLabel* label_3 = new QLabel(parent);
	label_3->setObjectName(QString::fromUtf8("label_3"));

	horizontalLayout_3->addWidget(label_3);

	QLineEdit* lineEdit_3 = new QLineEdit(parent);
	lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

	horizontalLayout_3->addWidget(lineEdit_3);
	verticalLayout->addLayout(horizontalLayout_3);
	pMainLayout->addLayout(verticalLayout);

	QHBoxLayout* horizontalLayout_5 = new QHBoxLayout();
	horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
	QRadioButton* radioButton = new QRadioButton(parent);
	radioButton->setObjectName(QString::fromUtf8("radioButton"));

	horizontalLayout_5->addWidget(radioButton);

	QRadioButton* radioButton_2 = new QRadioButton(parent);
	radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

	horizontalLayout_5->addWidget(radioButton_2);
	pMainLayout->addLayout(horizontalLayout_5);

	BaseDialog::CreateBottomFrame();

	//retranslateUi
	label->setText(QApplication::translate("DemoDialog", "TextLabel", 0));
	label_2->setText(QApplication::translate("DemoDialog", "TextLabel", 0));
	label_3->setText(QApplication::translate("DemoDialog", "TextLabel", 0));

	radioButton->setText(QApplication::translate("DemoDialog", "RadioButton", 0));
	radioButton_2->setText(QApplication::translate("DemoDialog", "RadioButton", 0));

	QMetaObject::connectSlotsByName(this);

	QObject::connect(GetCancelButton(), SIGNAL(clicked()), this, SLOT(cancelAction()));
} 

DemoDialog::~DemoDialog()
{

}


void DemoDialog::cancelAction()
{
	reject();
}