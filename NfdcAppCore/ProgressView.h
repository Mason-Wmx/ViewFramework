#pragma once
#include "stdafx.h"
#include "export.h"
#include "../NfdcAppCore/ProgressItem.h"
#include "../NfdcAppCore/SubView.h"

class MainWindow;

namespace SIM
{
    class ProgressDialog;

    /*
        Common class for displaying progress within application.
        It manages progress queue (only displays the last added progress item) and for now only supports modal dialogs.
        In future it should be possible to add background progress items (non-modal, e.g. displayed in status bar).

        If UpdateProgressWidget() is called from non-GUI thread, it starts a timer and updates itself from GUI thread in the future.
    */
	class NFDCAPPCORE_EXPORT ProgressView : public QObject, public SubView
	{
        Q_OBJECT
	public:
        ProgressView(MainWindow & mainWindow);
		~ProgressView();

        void AddProgress(const std::shared_ptr<ProgressItem> & progressItem);     // Adds progress item to the queue                      
        void UpdateProgress(const std::string & name, std::string & label);       // Updates label of the progress item with specific name
        void UpdateProgress(const std::string & name, int value);                 // Updates value of the progress item with specific name
        void RemoveProgress(const std::string & name);                            // Removes progress item from queue

        ProgressItem * GetProgressItem(const std::string & name);   // Return progress item with specific name
        
        virtual void Notify(Event & ev) override;

    public slots:
        void UpdateProgressWidget();    // Use to update the GUI of the progress widget (change its value, switch to another active progress item, etc.) [can be called from seperate thread]
        void ShowProgressWidget();      // Only use when HideProgressWidget was called manually
        void HideProgressWidget();      // Use to manually hide an open modal progress widget

    private:
        MainWindow & _mainWindow;

        std::mutex _mutex;
        std::map<std::string, std::shared_ptr<ProgressItem>> _progressItems;
        std::list<std::string> _progressQueue;

        std::unique_ptr<ProgressDialog> _modalProgressDialog;
        QTimer _updateTimer;
	};

    class UpdateProgressEvent : public Event
    {
    public:
        UpdateProgressEvent(const std::string & name, std::string & label, Observable & source) : Event(source), _name(name), _label(label) {}
        ~UpdateProgressEvent(void) {}

        std::string GetName() const { return _name; }
        std::string GetLabel() const { return _label; }

    private:
        std::string _name;
        std::string _label;
    };

    class UpdateProgressWidgetEvent : public Event
    {
    public:
        UpdateProgressWidgetEvent(Observable & source) : Event(source) {}
        ~UpdateProgressWidgetEvent(void) {}
    };

} // SIM

