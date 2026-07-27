#pragma once

#include "Tilc/DllGlobals.h"
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>
#include <initializer_list>
#include <iostream>

namespace Tilc
{
    constexpr const int XmlDefaultIndentSize = 2;

    class TXMLElement
    {
    public:
        const std::string m_Name;
        const std::string m_Text;
        const std::vector<TXMLElement> m_Children;
        TXMLElement(std::string_view name, std::string_view text) : m_Name(name), m_Text(text) {}
        TXMLElement(std::string_view name, std::string_view text, std::vector<TXMLElement>&& children) : m_Name(name), m_Text(text), m_Children(std::move(children)) {}

        using TIndentedXMLElement = std::pair<const TXMLElement&, size_t>;

        friend std::ostream& operator<<(std::ostream& out, Tilc::TXMLElement::TIndentedXMLElement indented_element);
        friend std::ostream& operator<<(std::ostream& out, const TXMLElement& element)
        {
            out << TIndentedXMLElement(element, 0);
            return out;
        }
    };
}
