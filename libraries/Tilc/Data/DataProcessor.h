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
            // remove objects with given ids, JsonCommand shold be for example
            /*
                {
                    "Action": "Delete",
                    "Ids": ["1", "2", "3", "4", "5"]
                }
            */
            static std::ostream& DeleteItems(std::ostream& os, Tilc::Data::TDB& DB, const char* Sql, const Tilc::TExtString& JsonCommand);

        private:
            TDataProcessor() {}
            virtual ~TDataProcessor() {}
        };
    }
}
