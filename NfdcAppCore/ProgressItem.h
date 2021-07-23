#pragma once

#include "stdafx.h"
#include "export.h"
#include "../Common/Progress.h"

namespace SIM
{
    class ProgressView;

    /**
        Single class for managing all types of progress objects within application. 
        Progress items are identified using their name.
        
        ProgressItem stores progress information using Progress class defined in eptcom.
        Additionally it allows to specify whether the progress object should be modal, have cancel button, etc.
    */
    class NFDCAPPCORE_EXPORT ProgressItem
    {
    public:
        ProgressItem(const std::string & name, const std::string & label);                              // Create progress item with specified name, label and unknown range
        ProgressItem(const std::string & name, const std::string & label, int value, int range = 100);  // Create progress item with specified name, label and known range
        ~ProgressItem();

        void SetProgressView(ProgressView * progressView);                              // Called within ProgressView::AddProgress, no need to call manually
        void SetModal(bool modal);                                                      // Set whether the progress item should block user input (not used for now, only modal progress is supported)
        void SetCancelButton(bool button);                                              // Not used for now
        void SetLabel(const std::string & label);                                       // Set label and specify the range as unknown
        void SetValue(int value, int range = -1);                                       // Set value and range (for -1 range is not changed)
        void SetLabelAndValue(const std::string & label, int value, int range = -1);    // Set label, value and range (for -1 range is not changed)
        void UpdateProgressWidget();                                                    // Used to manually update progress widget, when some parallel task blocks GUI thread

        std::string GetName() const { return _name; }
        const Progress & GetProgress() const { return _progress; }
        Progress & GetProgress() { return _progress; }
        bool HasCancelButton() const { return _cancelButton; }
        bool IsModal() const { return _modal; }
        bool isValid() const;

    private:
        std::string _name;                          // Unique name of progress item
        Progress _progress;                         // Progress object used to store progress info (value, range, label, etc.)

        ProgressView * _progressView{ nullptr };

        bool _modal{ true }; 
        bool _cancelButton{ false };
    };
} // SIM