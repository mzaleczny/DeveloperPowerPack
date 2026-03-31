#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Timer.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    struct DECLSPEC TProfileSample
    {
        unsigned int m_ProfileInstances;       // # of times profile_begin/profile_end called
        int m_OpenProfiles;                    // # of times profile_begin w/o profile_end
        Tilc::TExtString m_Name;               // Name of sample
        time_t m_StartTime;                    // The current open profile start time
        time_t m_Accumulator;                  // All samples this frame added together
        time_t m_ChildrenSampleTime;           // Time taken by all children
        unsigned int m_NumParents;             // Number of profile parents
        unsigned char m_Valid;                 // Whether this data is valid
    };

    struct DECLSPEC TProfileSampleHistory
    {
        Tilc::TExtString m_Name;               // Name of the sample
        time_t m_Ave;                          // Average time per frame (percentage)
        time_t m_Min;                          // Minimum time per frame (percentage)
        time_t m_Max;                          // Maximum time per frame (percentage)
        time_t m_Sum;                          // sum of all the samples of this name
        time_t m_SamplesCount;                 // # of samples
        unsigned char m_Valid;                 // Whether the data is valid
    };

	class DECLSPEC TProfiler
	{
    public:
        TProfiler(int ProfileNumSamples);
        ~TProfiler();
        void Reset(int ProfileNumSamples);
        void Cleanup();
        void Begin(const char* name);
        void End(const char* name);
        void DumpToBuffer(Tilc::TExtString& Buffer);
        void UpdateProfileHistory(const char* name, time_t Tm);
        void GetProfileFromHistory(const char* name, time_t* ave, time_t* min, time_t* max, time_t* sum, time_t* samples_count);
    protected:
        TTimer m_Timer;
        TProfileSample* m_Samples = nullptr;
        TProfileSampleHistory* m_History = nullptr;
        int m_NumProfileSamples;
        time_t m_StartProfileTime;
        time_t m_EndProfileTime;
    };
}
