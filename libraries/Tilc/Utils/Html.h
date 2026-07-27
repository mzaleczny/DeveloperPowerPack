#pragma once

#include <stdint.h>
#include "Tilc/Utils/Xml.h"

namespace Tilc
{
    class THTML : public TXMLElement
    {
    public:
        THTML() : TXMLElement("html", "") {}
        THTML(std::initializer_list<TXMLElement> children) : TXMLElement("html", "", children) {};
    };

    class THead : public TXMLElement {
    public:
        THead() : TXMLElement("head", "") {}
        THead(std::initializer_list<TXMLElement> children) : TXMLElement("head", "", children) {};
    };

    class TBody : public TXMLElement {
    public:
        TBody() : TXMLElement("body", "") {}
        TBody(std::initializer_list<TXMLElement> children) : TXMLElement("body", "", children) {};
    };

    class TTitle : public TXMLElement {
    public:
        TTitle(std::string_view title) : TXMLElement("title", title) {}
    };

    class TOL : public TXMLElement {
    public:
        TOL() : TXMLElement("ol", "") {}
        TOL(std::initializer_list<TXMLElement> children) : TXMLElement("ol", "", children) {};
    };

    class TUL : public TXMLElement {
    public:
        TUL() : TXMLElement("ul", "") {}
        TUL(std::initializer_list<TXMLElement> children) : TXMLElement("ul", "", children) {};
    };

    class TLI : public TXMLElement {
    public:
        explicit TLI(std::string_view text) : TXMLElement("li", text) {}
        TLI(std::string_view text, std::initializer_list<TXMLElement> children) : TXMLElement("li", text, children) {}
    };

    class TP : public TXMLElement {
    public:
        TP(std::string_view text) : TXMLElement("p", text) {}
    };

    class TDiv : public TXMLElement {
    public:
        explicit TDiv(std::string_view text) : TXMLElement("div", text) {}
        TDiv(std::string_view text, std::initializer_list<TXMLElement> children) : TXMLElement("div", text, children) {}
    };

    class TSpan : public TXMLElement {
    public:
        explicit TSpan(std::string_view text) : TXMLElement("span", text) {}
        TSpan(std::string_view text, std::initializer_list<TXMLElement> children) : TXMLElement("span", text, children) {}
    };

    class TA : public TXMLElement {
    public:
        explicit TA(std::string_view text) : TXMLElement("a", text) {}
        TA(std::string_view text, std::initializer_list<TXMLElement> children) : TXMLElement("a", text, children) {}
    };

}
