#pragma once

#include <QDialog>
#include "../NfdcAppCore/BaseDialog.h"

class DemoDialog : public BaseDialog
{
	Q_OBJECT

public:
	DemoDialog(QWidget *parent = 0);
	~DemoDialog();

private slots:
	void cancelAction();
	
};


