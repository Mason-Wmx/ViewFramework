#include "stdafx.h"
#include "ProgressItem.h"
#include "ProgressView.h"

using namespace SIM;

ProgressItem::ProgressItem(const std::string & name, const std::string & label)
{
    _name = name;

    _progress.text(label);
    _progress.unknown(true);
}

ProgressItem::ProgressItem(const std::string & name, const std::string & label, int value, int range) : 
    ProgressItem(name, label)
{
    _progress.value(value);
    _progress.total(range);
    _progress.unknown(false);
}

ProgressItem::~ProgressItem()
{
}

void ProgressItem::SetProgressView(ProgressView * progressView)
{
    _progressView = progressView;
}

bool ProgressItem::isValid() const
{
    return !_name.empty();
}

void ProgressItem::SetModal(bool modal)
{
    _modal = modal;
}

void ProgressItem::SetCancelButton(bool button)
{
    _cancelButton = button;
}

void ProgressItem::SetLabel(const std::string & label)
{
    _progress.text(label);

    if (!_progress.unknown())
        _progress.unknown(true);

     UpdateProgressWidget();
}

void ProgressItem::SetLabelAndValue(const std::string & label, int value, int range)
{
    _progress.text(label);

    SetValue(value, range);
}

void ProgressItem::SetValue(int value, int range)
{
    if (_progress.unknown())
        _progress.unknown(false);

    _progress.value(value);

    if (range != -1)
        _progress.total(range);

    UpdateProgressWidget();
}

void ProgressItem::UpdateProgressWidget()
{
    if (_progressView)
        _progressView->UpdateProgressWidget();
}