#include "Tilc/Globals.h"
#include <sstream>

std::default_random_engine Tilc::RandomGenerator = std::default_random_engine{ std::random_device{}() };
const char* Tilc::GoogleClientId = "";
Tilc::TDbConnectionParams Tilc::DbConnectionParams;

Tilc::TExtString Tilc::ImplodeId(std::vector<Tilc::TIdName> const& items, char ImplodeChar)
{
    std::ostringstream oss;
    bool pierwszy = true;

    for (const auto& item : items) {
        if (!pierwszy)
        {
            oss << ImplodeChar;
        }
        oss << item.id;
        pierwszy = false;
    }

    return oss.str();
}
