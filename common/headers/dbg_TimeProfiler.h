#include <string>
#include <time.h>
#include <map>


//
//
//  ###################################################################################################
//  #                                                                                                 #
//  #                  T I M E    P R O F I L E R   - to measure code execution time                  #
//  #                                                                                                 #
//  ###################################################################################################
//
//
//
//
//  How to use time profiler:
//  =================================
//      1) Put in code line like that in place when you want to start counting time:
//              START_TIMED_BLOCK("Open Dialog, loading project list from cloud");
//              Key string HAVE TO BE VERY DESCRIPTIVE, so QA could understand what is actually measured.
//              First general area ("Open Dialog"), next particular action that is timed
//              ("loading project list from cloud").
//
//      2) Put in code line like that in place when you want to finish counting time:
//              END_TIMED_BLOCK("Open Dialog, loading project list from cloud");
//              You have to use the same descriptive string.
//
//      3) When END_TIMED_BLOCK; is executed then in the "Output" window in Visual Studio
//          following line will be displayed:
//              TimeProfiler: Open Dialog, loading project list from cloud: 3.821000[s]. 
//
//       To dump time notifications to file, do following:
//            - right mouse click on AppBase module, select "Properties" next "Debugging" tab,
//                  in "Command arguments" row add ">console.txt". This will redirect
//                  std::cout, which is used to output timings, to "console.txt" file.
//                  File will be created on Your HDD drive in 
//                  "$\lakota\target\vc14\Win64\dry\appbase\" directory, when Lakota (Appbase.exe)
//                  is started. When next time You run program "console.txt" will be purged.
//
//
//
//
//     CONSIDER THAT FOLLOWING CODE WAS ONLY DONE TO MESURE ONE PARTICULAR PROBLEM
//          Code itself is working, but have several serious flaws.
//          Fell free to extend and fix it.
//
//
//


namespace dbg
{
    static std::map<std::string, clock_t> dbg_TimeProfilers; //std.string is key that identify particular timed block (IT SHOULD BE VERY DESCRIPTIVE, so QA can unnderstadn it), clock_t is start time for measuring
}



std::string dbg_Print_to_cout(const std::string & str);
std::string dbg_Print(const std::string & str);



#define START_TIMED_BLOCK(traceString) \
        dbg::dbg_TimeProfilers.insert(std::pair<std::string, clock_t>(traceString, clock()));


#define END_TIMED_BLOCK(traceString) \
        auto timeProfilerIterator = dbg::dbg_TimeProfilers.find(traceString); \
        if (timeProfilerIterator != dbg::dbg_TimeProfilers.end()) \
        { \
            float time_diff = float(clock() - timeProfilerIterator->second) / CLOCKS_PER_SEC; \
            /* In fact we should not count seconds (machine dependent) but clicks!! */ \
            std::string time_diff_msg = std::string("TimeProfiler: ") + timeProfilerIterator->first + std::string(": ") + std::to_string(time_diff) + std::string("[s]."); \
            dbg::dbg_TimeProfilers.erase(timeProfilerIterator); \
            dbg_Print_to_cout(time_diff_msg); \
            dbg_Print(time_diff_msg); \
        } 

