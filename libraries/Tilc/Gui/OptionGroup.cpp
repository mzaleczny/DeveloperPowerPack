#include "Tilc/Gui/OptionGroup.h"
#include "Tilc/Gui/Option.h"
#include <algorithm>
#include <ranges>

Tilc::Gui::TOptionGroup::TOptionGroup() : m_GroupId(m_GroupIdSequence)
{
    ++m_GroupIdSequence;
}

Tilc::Gui::TOptionGroup::TOptionGroup(std::initializer_list<TOption*> OptionsInGroup) : m_Options(OptionsInGroup), m_GroupId(m_GroupIdSequence)
{
    TOptionGroup* ThisGroup = this;
    std::ranges::for_each(m_Options, [ThisGroup](TOption* item) {
        item->AddToGroup(ThisGroup);
    });
    ++m_GroupIdSequence;
}

void Tilc::Gui::TOptionGroup::AddOptions(std::initializer_list<TOption*> OptionsInGroup)
{
    TOptionGroup* ThisGroup = this;
    m_Options.reserve(OptionsInGroup.size());
    std::ranges::for_each(OptionsInGroup, [ThisGroup, this](TOption* item) {
        item->AddToGroup(ThisGroup);
        m_Options.push_back(item);
    });
}

void Tilc::Gui::TOptionGroup::CheckOption(TOption* Option)
{
    std::ranges::for_each(m_Options, [Option](TOption* item) {
        item->SetChecked(item == Option);
    });
}
