#include "Tilc/Utils/Html.h"

namespace Tilc
{
    DECLSPEC std::ostream& operator<<(std::ostream& out, const Tilc::THTML& element)
    {
        out << "HTML";
        out << static_cast<const Tilc::TXMLElement&>(element);
        return out;
    }
}
