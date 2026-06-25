#pragma once

#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <initializer_list>
#include <functional>


namespace Tilc::Gui
{
    class TOption;

    class DECLSPEC TOptionGroup
    {
    public:
        TOptionGroup();
        TOptionGroup(std::initializer_list<TOption*> OptionsInGroup);
        void AddOptions(std::initializer_list<TOption*> OptionsInGroup);
        void CheckOption(TOption* Option);
    protected:
        std::vector<TOption*> m_Options;
        int m_GroupId{ 1 };
        static inline int m_GroupIdSequence = 1;
    };
}
