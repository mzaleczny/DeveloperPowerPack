#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc {

    #define     JSONPARSER_GROW_CHUNK                                                  8192

    #define     JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY                                1
    #define     JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY              2
    #define     JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE                    3
    #define     JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE                    4
    #define     JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE                     5
    #define     JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE                    6
    #define     JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME                           7
    #define     JSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON                               8
    #define     JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE                          9
    #define     JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE                   10
    #define     JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE                   11
    #define     JSONPARSER_STATE_STOP_PARSING                                          100

    #define     JSONPARSER_CURRENT_ITEM_UNDEFINED                                      0
    #define     JSONPARSER_CURRENT_ITEM_ARRAY                                          1
    #define     JSONPARSER_CURRENT_ITEM_OBJECT                                         2

    class TStdObject;

    class DECLSPEC TJsonParserEntity {
    public:
        TJsonParserEntity(void *entity, int entity_type)
        {
            this->entity = entity;
            this->entity_type = entity_type;
        };
        // wskaźnik na obiekt
        void *entity;
        // typ obiektu:
        //  0 - nieokreślony
        //  1 - tablica (CmzPointerList)
        //  2 - obiekt  (TStdObject)
        int entity_type;
    private:
        TJsonParserEntity() {};
    };

    class DECLSPEC TJsonParser {
    public:
        TJsonParser();
        virtual ~TJsonParser();

        TStdObject* parseFile(const TExtString& fname, TStdObject* destination = nullptr);
        TStdObject* parse(const TExtString& content, TStdObject* destination = nullptr);
        inline bool isError() {return this->_error != 0; };
        inline TExtString getErrorMessage() {return this->_error_msg; };

    private:
        // poniższy atrybut przechowuje obiekt który zostanie zwrócony po
        // poprawnym przeparsowaniu pliku json. Ma on jeden atrybut: 'root'
        // który zawiera wskaźnik na pierwszy obiekt lub tablicę w pliku
        // json.
        TStdObject *_retval;
        // Flag marking if retval object was created in CParser object or if it was supplied from parser caller.
        // If it was created inside parser, then if parser error this object is deleted. Otherwise not.
        bool _retvalInternalCreated;
    
        // tablica zawierająca wskaźniki na aktualnie przetwarzane encje.
        // są to wskaźniki na obiekty klasy CJsonParserEntity.
        TPointersVector _current_object;
        // nazwa atrybutu pod jaką zostanie dodana nowa wartość do
        // aktualnie przetwarzanego obiektu
        TExtString _current_object_attribute_name;

        // keeps track of global char index within buffer, allows to decrement index variable that is locally used to traverse chars in buffer
        char* _current_token;
        int _current_token_buf_size;
        // ilość znaczących znaków w buforze tokena
        int _current_token_length;
        bool m_QuotedValueStarted;
        bool m_QuotedNameStarted;

        // Tablica kolejnych stanów parsera. Zawiera liczby całkowite.
        std::vector<int> _current_state;
        // poniższa zmienna informuje czy nazwę atrybutu obiektu otoczono
        // cudzysłowem czy nie a jeśli tak, to jakim. Możliwe wartości:
        //  - - nazwa atrybutu podana bez cudzysłowu
        //  ' - nazwa atrybutu podana w pojedyńczym cudzysłowiu
        //  " - nazwa atrybutu podana w podwójnym cudzysłowiu
        char _object_property_name_quote;

        int _error;
        TExtString _error_msg;
        int _line_number;
        int _char_number_in_line;
        int _char_number_in_file;

        void _processBuffer(const TExtString& buffer);
        inline bool _isWhiteSpace(char ch) { return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t'; };
        void _deleteCurrentObjectArray();
        void _deleteCurrentStateArray();
        void _cleanRetvalObject();
        TExtString _getStringFromCurrentToken();
        int _onExpectArrayValueInQuote(char quoteType, char ch);
        // returns number of characters to move backward the cursor in caller function. This is required to handle reading value that is not quoted.
        int _onExpectObjectValueInQuote(char quoteType, char ch);
        void _onBeginArray();
        void _onBeginObject();
        void _onCompleteReadObjectAttributeName();
        void _onCompleteReadObjectAttributeSimpleValue();
        void _appendCharToCurrentToken(char ch);
        void _onCloseSquare();
        void _initParsingProcess(TStdObject* destination = nullptr);
    };

}
