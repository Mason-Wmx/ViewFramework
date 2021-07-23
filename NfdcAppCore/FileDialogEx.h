#pragma once
#include "export.h"
#include "BaseDialog.h"
#include <qfiledialog.h>

namespace SIM
{
    class NFDCAPPCORE_EXPORT FileDialogEx : public BaseDialog
    {
        Q_OBJECT
    public:
        FileDialogEx(QWidget * parent, QFileDialog::AcceptMode mode, const QString & title);

        virtual int exec() override;
        QFileDialog & GetDialog() { return _fileDialog; }

    private:
        void HijackButtonBox();

    private:
        QFileDialog _fileDialog;
    };
}
