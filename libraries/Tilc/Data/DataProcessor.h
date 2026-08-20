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
            // insert objects when Id in Json input is empty otherwise update item with specified Id
            /*
                {
                    "Action": "InsertUpdate",
                    "Items": [
                        {
                            "id": "",
                            "name": "Kategoria #1"
                        },
                        {
                            "id": "",
                            "name": "Kategoria #2"
                        },
                        {
                            "id": "",
                            "name": "Kategoria #3"
                        },
                        {
                            "id": "",
                            "name": "Kategoria #4"
                        },
                        {
                            "id": "",
                            "name": "Kategoria #5"
                        },
                        {
                            "id": "1",
                            "name": "Kategoria #1",
                            "short_description": "Krótki opis kategorii #1."
                        }
                    ]
                }
            */
            static std::ostream& SaveItems(std::ostream& os, Tilc::Data::TDB& DB, const char* TableName, const Tilc::TExtString& JsonCommand);
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
