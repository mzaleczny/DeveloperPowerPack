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

        template <typename... Children>
        TXMLElement(std::string_view name, std::string_view text, Children&&... children)
            : m_Name(name), m_Text(text), m_Children{ std::forward<Children>(children)... }
        {}

        struct TIndentedXMLElement
        {
            const TXMLElement& element;
            size_t indent_size;
        };

        void Print(const Tilc::TXMLElement::TIndentedXMLElement& indented_element)
        {
            const std::string indent(indented_element.indent_size, ' ');
            std::cout << indent << "<" << indented_element.element.m_Name << ">\n";
            if (!indented_element.element.m_Text.empty())
            {
                std::cout << indent << "  " << indented_element.element.m_Text << "\n";
            }
            for (const auto& e : indented_element.element.m_Children)
            {
                Print(Tilc::TXMLElement::TIndentedXMLElement{ e, indented_element.indent_size + Tilc::XmlDefaultIndentSize });
            }
            std::cout << indent << "</" << indented_element.element.m_Name << ">" << std::endl;
        }

        void Print()
        {
            Print(Tilc::TXMLElement::TIndentedXMLElement{ *this, 0 });
        }
    };
}
