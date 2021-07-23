#pragma once
#include "stdafx.h"
#include "export.h"
#include "../Common/Progress.h"

class QProgressBarDlg;

namespace SIM
{
	class NFDCAPPCORE_EXPORT ProgressDialog
	{
	public:
        ProgressDialog(bool modal = false, QWidget *parent = nullptr);
        ~ProgressDialog();

        void SetProgress(const Progress &progress);
        void Show();
        void Hide();

	private:
        std::unique_ptr<QProgressBarDlg> _progressDialog;
	};
} // SIM
