#pragma once

#include <ostream>
#include <initializer_list>

#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"
#include "Tilc/Data/Db.h"

namespace Tilc {
    namespace Data {
        class DECLSPEC TDataProcessor
        {
        public:
            static std::ostream& PrintList(std::ostream& os, Tilc::Data::TDB& DB, const char* Sql, std::initializer_list<const char*> Fields);

        private:
            TDataProcessor() {}
            virtual ~TDataProcessor() {}
        };
    }
}
