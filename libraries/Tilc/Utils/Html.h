#pragma once

#include <stdint.h>
#include "Tilc/Utils/Xml.h"

namespace Tilc
{
    class THTML : public TXMLElement
    {
    public:
        THTML() : TXMLElement("html", "") {}

        template <std::derived_from<TXMLElement>... Children>
        THTML(Children&&... children) : TXMLElement("html", "", std::forward<Children>(children)...) {}
    };

    class THead : public TXMLElement {
    public:
        THead() : TXMLElement("head", "") {}

        template <std::derived_from<TXMLElement>... Children>
        THead(Children&&... children) : TXMLElement("head", "", std::forward<Children>(children)...) {}
    };

    class TBody : public TXMLElement {
    public:
        TBody() : TXMLElement("body", "") {}

        template <std::derived_from<TXMLElement>... Children>
        TBody(Children&&... children) : TXMLElement("body", "", std::forward<Children>(children)...) {}
    };

    class TTitle : public TXMLElement {
    public:
        TTitle(std::string_view title) : TXMLElement("title", title) {}
    };

    class TOL : public TXMLElement {
    public:
        TOL() : TXMLElement("ol", "") {}

        template <std::derived_from<TXMLElement>... Children>
        TOL(Children&&... children) : TXMLElement("ol", "", std::forward<Children>(children)...) {}
    };

    class TUL : public TXMLElement {
    public:
        TUL() : TXMLElement("ul", "") {}

        template <std::derived_from<TXMLElement>... Children>
        TUL(Children&&... children) : TXMLElement("ul", "", std::forward<Children>(children)...) {}
    };

    class TLI : public TXMLElement {
    public:
        explicit TLI(std::string_view text) : TXMLElement("li", text) {}

        template <std::derived_from<TXMLElement>... Children>
        TLI(std::string_view text, Children&&... children) : TXMLElement("li", text, std::forward<Children>(children)...) {}
    };

    class TP : public TXMLElement {
    public:
        TP(std::string_view text) : TXMLElement("p", text) {}
    };

    class TDiv : public TXMLElement {
    public:
        explicit TDiv(std::string_view text) : TXMLElement("div", text) {}

        template <std::derived_from<TXMLElement>... Children>
        TDiv(std::string_view text, Children&&... children) : TXMLElement("div", text, std::forward<Children>(children)...) {}
    };

    class TSpan : public TXMLElement {
    public:
        explicit TSpan(std::string_view text) : TXMLElement("span", text) {}

        template <std::derived_from<TXMLElement>... Children>
        TSpan(std::string_view text, Children&&... children) : TXMLElement("span", text, std::forward<Children>(children)...) {}
    };

    class TA : public TXMLElement {
    public:
        explicit TA(std::string_view text) : TXMLElement("a", text) {}

        template <std::derived_from<TXMLElement>... Children>
        TA(std::string_view text, Children&&... children) : TXMLElement("a", text, std::forward<Children>(children)...) {}
    };
}
