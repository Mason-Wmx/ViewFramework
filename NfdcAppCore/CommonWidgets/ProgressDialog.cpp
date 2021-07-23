#include "stdafx.h"
#include "ProgressDialog.h"
#include "QProgressBarDlg.h"

using namespace SIM;

ProgressDialog::ProgressDialog(bool modal, QWidget *parent)
{
    _progressDialog = std::make_unique<QProgressBarDlg>(parent, false);
    _progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    if (modal)
        _progressDialog->setWindowModality(Qt::ApplicationModal);
    
    _progressDialog->show();
}

ProgressDialog::~ProgressDialog()
{

}

void ProgressDialog::SetProgress(const Progress &progress)
{
    _progressDialog->sync(progress);
}

void ProgressDialog::Show()
{
    _progressDialog->show();
}

void ProgressDialog::Hide()
{
    _progressDialog->hide();
}