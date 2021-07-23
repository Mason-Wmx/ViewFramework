#pragma once

#include "export.h"


namespace SIM
{
    enum NFDCAPPCORE_EXPORT CloudJobStatus
    {                                 // Generate button enabled    Cancel button enabled      Application close button enabled       Message
        eJobStatusUnknown       = -1, // true,                      false,                     true                                  
        eJobUploading           = 0,  // false,                     false,                     false                                  Uploading
        eJobUploaded            = 1,  // false,                     false,                     false                                  Pending
        eJobRequested           = 2,  // false,                     false,                     false                                  Pending
        eJobSubmitted           = 3,  // false,                     true                       true                                   Pending
        eJobRunning             = 4,  // false,                     true                       true                                   Generating
        eJobCancelRequested     = 5,  // false,                     false                      true                                   Cancelling or Canceling ?
        eJobCancelled           = 6,  // true,                      false                      true                                   Cancelled
        eJobCancelFailed        = 7,  // false,                     true                       true                                   Failed to cancel
        eJobTimeOut             = 8,  // true,                      false                      true                                   Time out
        eJobCompleted           = 9,  // true,                      false                      true                                   Completed
        eJobTerminated          = 10, // true,                      false                      true                                   Terminated
        eJobFailed              = 11  // true,                      false                      true                                   Failed
    };
}