#pragma once

#include <ctime>
#include <iostream>

#include "Tilc/DllGlobals.h"

namespace Tilc {
    class DECLSPEC TDate {
    public:
        enum EDateType {
            EuropeanDate,
            USADate,
            ISODate
        };
        explicit TDate() : m_Time(Now()), m_Type{ EDateType::EuropeanDate } {}
        explicit TDate(std::time_t t) : m_Time(t), m_Type{ EDateType::EuropeanDate } {}

        friend std::ostream& operator<<(std::ostream& out, const TDate& d)
        {
            //const tm local_tm = *localtime(&d.m_Time);
            tm local_tm;
            localtime_s(&local_tm, &d.m_Time);
            switch (d.m_Type)
            {
            case EDateType::EuropeanDate:
                out << local_tm.tm_mday << "/" << local_tm.tm_mon + 1 << "/" << local_tm.tm_year + 1900;
                break;
            case EDateType::USADate:
                out << local_tm.tm_mon + 1 << "/" << local_tm.tm_mday << "/" << local_tm.tm_year + 1900;
                break;
            case EDateType::ISODate:
                out << local_tm.tm_year + 1900 << "/" << local_tm.tm_mon + 1 << "/" << local_tm.tm_mday;
                break;
            }
            return out;
        }

        static std::time_t Now()
        {
            return std::time(NULL);
        }
    protected:
        const std::time_t m_Time;
        EDateType m_Type;
    };
}
