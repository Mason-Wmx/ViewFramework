#include "stdafx.h"
#include "ProgressView.h"
#include "mainwindow.h"
#include "../NfdcAppCore/CommonWidgets/ProgressDialog.h"

#include <algorithm>

using namespace SIM;

ProgressView::ProgressView(MainWindow & mainWindow) :
    QObject(nullptr),
    _mainWindow(mainWindow)
{
    _updateTimer.setInterval(100);
    _updateTimer.setSingleShot(true);
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(UpdateProgressWidget()), Qt::QueuedConnection);
}

ProgressView::~ProgressView()
{

}

void ProgressView::AddProgress(const std::shared_ptr<ProgressItem> & progressItem)
{
    std::unique_lock<std::mutex> l(_mutex);

    if (!progressItem->isValid()) {
        Q_ASSERT(false && "Progress name and label cannot be empty");
        return;
    }

    progressItem->SetProgressView(this);

    _progressItems[progressItem->GetName()] = progressItem;

    _progressQueue.remove_if([&progressItem](const std::string &name) { return name==progressItem->GetName(); });
    _progressQueue.push_back(progressItem->GetName());

    l.unlock();

    UpdateProgressWidget();
}

void ProgressView::UpdateProgress(const std::string & name, std::string & label)
{
    std::unique_lock<std::mutex> l(_mutex);

    if (_progressItems.find(name) == _progressItems.end())
        return;

    auto item = _progressItems[name];

    l.unlock();

    item->SetLabel(label);
}

void ProgressView::UpdateProgress(const std::string & name, int value)
{
    std::unique_lock<std::mutex> l(_mutex);

    if (_progressItems.find(name) == _progressItems.end())
        return;

    auto item = _progressItems[name];

    l.unlock();

    item->SetValue(value);
}

void ProgressView::RemoveProgress(const std::string & name)
{
    std::unique_lock<std::mutex> l(_mutex);

    _progressItems.erase(name);
    _progressQueue.erase(std::remove(_progressQueue.begin(), _progressQueue.end(), name), _progressQueue.end());

    l.unlock();

    UpdateProgressWidget();
}

void ProgressView::ShowProgressWidget()
{
    if (_modalProgressDialog)
        _modalProgressDialog->Show();
}

void ProgressView::HideProgressWidget()
{
    if (_modalProgressDialog)
        _modalProgressDialog->Hide();
}

ProgressItem * ProgressView::GetProgressItem(const std::string & name)
{
    std::unique_lock<std::mutex> l(_mutex);

    return _progressItems[name].get();
}

void ProgressView::UpdateProgressWidget()
{
    std::unique_lock<std::mutex> l(_mutex);

    // If functions was called from another thread use timer to update progress widget in the future
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        if (!_updateTimer.isActive())
            _updateTimer.start();

        return;
    }

    if (_updateTimer.isActive())
        _updateTimer.stop();

    if (_progressItems.empty() || _progressQueue.empty()) {
        if (_modalProgressDialog)
            _modalProgressDialog.reset();

        return;
    }

    const std::string currentItem = _progressQueue.front();
    const auto progressItem = _progressItems[currentItem];

    if (!progressItem)
        return;

    if (!_modalProgressDialog)
        _modalProgressDialog = std::make_unique<ProgressDialog>(/* modal */ true, & _mainWindow);

    const auto & progress = progressItem->GetProgress();

    _modalProgressDialog->SetProgress(progress);
}

void ProgressView::Notify(Event & ev)
{
    if (ev.Is<UpdateProgressWidgetEvent>())
        UpdateProgressWidget();
    else if (auto updateProgressEvent = dynamic_cast<UpdateProgressEvent*>(&ev))
        UpdateProgress(updateProgressEvent->GetName(), updateProgressEvent->GetLabel());
}
