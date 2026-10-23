#include "Tilc/Globals.h"

std::default_random_engine Tilc::RandomGenerator = std::default_random_engine{ std::random_device{}() };
const char* Tilc::GoogleClientId = "";
