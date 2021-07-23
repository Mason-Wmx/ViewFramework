#include "stdafx.h"
#include "FileDialogEx.h"

using namespace SIM;

FileDialogEx::FileDialogEx(QWidget * parent, QFileDialog::AcceptMode mode, const QString & title)
    : BaseDialog(parent),
    _fileDialog(this)
{
    SetHeaderTitle(title);

    _fileDialog.setWindowFlags(Qt::Widget);
    _fileDialog.setOption(QFileDialog::DontUseNativeDialog);
    _fileDialog.setViewMode(QFileDialog::ViewMode::List);
    _fileDialog.setAcceptMode(mode);
    _fileDialog.setFixedSize(800, 480);
    _fileDialog.setSizeGripEnabled(false);

    auto toolButtons = _fileDialog.findChildren<QToolButton *>();
    for (auto toolButton : toolButtons)
    {
        auto name = toolButton->objectName();
        if (name.endsWith("ModeButton"))
            toolButton->hide();
    }

    auto sideBar = _fileDialog.findChild<QWidget *>("sidebar");
    if (sideBar != nullptr)
        sideBar->hide();

    auto pStyle = QStyleFactory::create("Plastique");
    if (pStyle != nullptr)
    {
        // set plastique style plugin for dialog combos
        // to get around a black background problem
        // when displaying a combobox list containing icons
        auto combos = _fileDialog.findChildren<QComboBox *>();
        for (auto combo : combos)
            combo->setStyle(pStyle);
    }

    auto pMainLayout = GetMainLayout();
    pMainLayout->addWidget(&_fileDialog);

    CreateBottomFrame(/*bOKCancel =*/ false);
    HijackButtonBox();
}

void SIM::FileDialogEx::HijackButtonBox()
{
    auto pBottomLayout = GetBottomLayout();
    if (pBottomLayout != nullptr)
    {
        auto buttonBox = _fileDialog.findChild<QDialogButtonBox *>();
        if (buttonBox != nullptr)
        {
            auto okButton = buttonBox->button(QDialogButtonBox::Open);
            if (okButton == nullptr)
                okButton = buttonBox->button(QDialogButtonBox::Save);
            if (okButton != nullptr)
                okButton->setObjectName("OKButton");

            auto cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            if (cancelButton != nullptr)
                cancelButton->setObjectName("CancelButton");

            buttonBox->layout()->removeWidget(buttonBox);
            buttonBox->setOrientation(Qt::Horizontal);

            pBottomLayout->addWidget(buttonBox);
        }
    }
}

int FileDialogEx::exec()
{
    show();
    return _fileDialog.exec();
}
