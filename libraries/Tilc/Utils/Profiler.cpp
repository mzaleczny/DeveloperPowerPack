#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <assert.h>
#include "Tilc/Utils/Profiler.h"


Tilc::TProfiler::TProfiler(int ProfileNumSamples)
{
    Reset(ProfileNumSamples);
}

void Tilc::TProfiler::Reset(int ProfileNumSamples)
{
    if (m_Samples)
    {
        Cleanup();
    }

    m_NumProfileSamples = ProfileNumSamples;

    m_Samples = new (std::nothrow) TProfileSample[m_NumProfileSamples];
    if (!m_Samples) return;

    m_History = new (std::nothrow) TProfileSampleHistory[m_NumProfileSamples];
    if (!m_History)
    {
        Cleanup();
        return;
    }

    memset(m_Samples, 0, m_NumProfileSamples * sizeof(TProfileSample));
    memset(m_History, 0, m_NumProfileSamples * sizeof(TProfileSampleHistory));

    m_Timer.Reset();
    m_StartProfileTime = m_Timer.GetCurTime();
}

Tilc::TProfiler::~TProfiler()
{
    Cleanup();
}

void Tilc::TProfiler::Cleanup()
{
    if (m_Samples)
    {
        delete[] m_Samples;
        m_Samples = nullptr;
    }
    if (m_History)
    {
        delete[] m_History;
        m_History = nullptr;
    }
}

void Tilc::TProfiler::Begin(const char* name)
{
    unsigned int i = 0;

    while (i < m_NumProfileSamples  && m_Samples[i].m_Valid == 1)
    {
        if (m_Samples[i].m_Name == name)
        {
            // Found the sample
            ++m_Samples[i].m_OpenProfiles;
            ++m_Samples[i].m_ProfileInstances;
            m_Samples[i].m_StartTime = m_Timer.GetCurTime();
            assert("Allowed only one opened profile of the same name" && m_Samples[i].m_OpenProfiles <= 1); // max 1 open at once
            return;
        }
        ++i;
    }

    if (i >= m_NumProfileSamples)
    {
        assert(!"Exceeded Max Available Profile Samples");
        return;
    }

    m_Samples[i].m_Name = name;
    m_Samples[i].m_Valid = 1;
    m_Samples[i].m_OpenProfiles = 1;
    m_Samples[i].m_ProfileInstances = 1;
    m_Samples[i].m_Accumulator = 0L;
    m_Samples[i].m_StartTime = m_Timer.GetCurTime();
    m_Samples[i].m_ChildrenSampleTime = 0LL;
}

void Tilc::TProfiler::End(const char* name)
{
    unsigned int i = 0;
    unsigned int NumParents = 0;
    time_t Tm;
    time_t EndTime = m_Timer.GetCurTime();

    while (i < m_NumProfileSamples  && m_Samples[i].m_Valid == 1)
    {
        if (m_Samples[i].m_Name == name)
        {
            //Found the sample
            unsigned int Inner = 0;
            int Parent = -1;
            --m_Samples[i].m_OpenProfiles;
            Tm = EndTime - m_Samples[i].m_StartTime;

            //Count all parents and find the immediate parent
            while (m_Samples[Inner].m_Valid == 1)
            {
                if (m_Samples[Inner].m_OpenProfiles > 0)
                {
                    //Found a parent (any open profiles are parents)
                    ++NumParents;
                    if (Parent < 0)
                    {
                        //Replace invalid parent (index)
                        Parent = Inner;
                    }
                    else if (m_Samples[Inner].m_StartTime >= m_Samples[Parent].m_StartTime)
                    {
                        //Replace with more immediate parent
                        Parent = Inner;
                    }
                }
                Inner++;
            }

            //Remember the current number of parents of the sample
            m_Samples[i].m_NumParents = NumParents;

            if (Parent >= 0)
            {
                m_Samples[Parent].m_ChildrenSampleTime += Tm;
            }

            //Save sample time in accumulator
            m_Samples[i].m_Accumulator += Tm;

            // update history
            UpdateProfileHistory(m_Samples[i].m_Name.c_str(), Tm);
            return;
        }
        ++i;
    }
}

void Tilc::TProfiler::DumpToBuffer(Tilc::TExtString& Buffer)
{
    std::stringstream os;
    unsigned int i = 0;
    time_t total_time;

    m_EndProfileTime = m_Timer.GetCurTime();

    os << "         Ave :         Min :         Max :       Total :     Total % :      Sample :    Sample % :       Ave % :           # : Profile Name\n";
    os << "-------------------------------------------------------------------------------------------------------------------------------------------\n";

    while (i < m_NumProfileSamples && m_Samples[i].m_Valid == 1)
    {
        unsigned int indent = 0;
        time_t sample_time, total_sample_time, ave_time, min_time, max_time, sum_time, samples_count;
        time_t percent_time, percent_time_sample, percent_time_total_sample;
        Tilc::TExtString name, indented_name;
        Tilc::TExtString s_percent_time, s_percent_time_sample, s_percent_time_total_sample, s_sample_time, s_total_sample_time;

        if (m_Samples[i].m_OpenProfiles < 0)
        {
            assert(!"profile_end() called without a profile_begin()");
        }
        else if (m_Samples[i].m_OpenProfiles > 0)
        {
            assert(!"profile_begin() called without a profile_end()");
        }

        total_time = m_EndProfileTime - m_StartProfileTime;
        sample_time = m_Samples[i].m_Accumulator - m_Samples[i].m_ChildrenSampleTime;
        total_sample_time = m_Samples[i].m_Accumulator;
        GetProfileFromHistory(m_Samples[i].m_Name.c_str(), &ave_time, &min_time, &max_time, &sum_time, &samples_count);

        //Format the data
        os << std::setw(12) << ave_time << " |";
        os << std::setw(12) << min_time << " |";
        os << std::setw(12) << max_time << " |";

        indented_name = m_Samples[i].m_Name;
        for (indent = 0; indent < m_Samples[i].m_NumParents; indent++)
        {
            name = "   " + indented_name;
            indented_name = name;
        }

        percent_time = ave_time * 100 / total_time;
        percent_time_sample = sample_time * 100 / total_time;
        percent_time_total_sample = total_sample_time * 100 / total_time;
        s_percent_time = std::to_string(percent_time) + " %";
        s_percent_time_sample = std::to_string(percent_time_sample) + " %";
        s_sample_time = std::to_string(sample_time);
        s_total_sample_time = std::to_string(total_sample_time);
        s_percent_time_total_sample = std::to_string(percent_time_total_sample) + " %";
        os << std::setw(12) << s_total_sample_time << " |"
            << std::setw(12) << s_percent_time_total_sample << " |"
            << std::setw(12) << s_sample_time << " |"
            << std::setw(12) << s_percent_time_sample << " |"
            << std::setw(12) << s_percent_time << " |"
            << std::setw(12) << m_Samples[i].m_ProfileInstances << " | "
            << indented_name << std::endl;
        ++i;
    }
    os << "Total time: " << total_time << "\n" \
        "Total is time of the sample with times of all its children\n" \
        "Sample is time of the sample without times of all its children\n" \
        "Measured times are in miliseconds\n";

    Buffer = os.str();

    // Reset samples for next frame
    memset(m_Samples, 0, m_NumProfileSamples * sizeof(TProfileSample));
    memset(m_History, 0, m_NumProfileSamples * sizeof(TProfileSampleHistory));
    m_StartProfileTime = m_Timer.GetCurTime();
}

void Tilc::TProfiler::UpdateProfileHistory(const char* name, time_t Tm)
{
    unsigned int i = 0;

    while (i < m_NumProfileSamples && m_History[i].m_Valid == 1)
    {
        if (m_History[i].m_Name == name) // Found the sample
        {
            m_History[i].m_Sum += Tm;
            m_History[i].m_SamplesCount += 1;
            m_History[i].m_Ave = m_History[i].m_Sum / m_History[i].m_SamplesCount;

            if (Tm < m_History[i].m_Min)
            {
                m_History[i].m_Min = Tm;
            }

            if (Tm > m_History[i].m_Max)
            {
                m_History[i].m_Max = Tm;
            }

            return;
        }
        ++i;
    }

    if (i < m_NumProfileSamples) // Add to history
    {
        m_History[i].m_Name = name;
        m_History[i].m_Valid = 1;
        m_History[i].m_Sum = m_History[i].m_Ave = m_History[i].m_Min = m_History[i].m_Max = Tm;
        m_History[i].m_SamplesCount = 1;
    }
    else
    {
        assert(!"Exceeded Max Available Profile Samples!");
    }
}

void Tilc::TProfiler::GetProfileFromHistory(const char* name, time_t* ave, time_t* min, time_t* max, time_t* sum, time_t* samples_count)
{
    unsigned int i = 0;
    while (i < m_NumProfileSamples && m_History[i].m_Valid == 1)
    {
        if (m_History[i].m_Name == name)  // Found the sample
        {
            *ave = m_History[i].m_Ave;
            *min = m_History[i].m_Min;
            *max = m_History[i].m_Max;
            *sum = m_History[i].m_Sum;
            *samples_count = m_History[i].m_SamplesCount;
            return;
        }
        ++i;
    }
    *ave = *min = *max = *sum = *samples_count = 0LL;
}
