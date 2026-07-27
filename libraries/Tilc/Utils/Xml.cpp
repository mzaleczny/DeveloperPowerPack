#include "Tilc/Utils/Xml.h"

std::ostream& Tilc::operator<<(std::ostream& out, Tilc::TXMLElement::TIndentedXMLElement indented_element)
{
    const auto& [element, indent_size] = indented_element;
    const std::string indent(indent_size, ' ');
    out << indent << "<" << element.m_Name << ">\n";
    if (!element.m_Text.empty()) out << indent << "  " << element.m_Text << "\n";
    for (const auto& e : element.m_Children)
    {
        out << Tilc::TXMLElement::TIndentedXMLElement(e, indent_size + Tilc::XmlDefaultIndentSize);
    }
    out << indent << "</" << element.m_Name << ">" << std::endl;
    return out;
}
