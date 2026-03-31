#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/StdObject.h"

using TPointerList = std::vector<void*>;

Tilc::TJsonParser::TJsonParser()
{
    this->_retval = nullptr;
    this->_current_token_buf_size = JSONPARSER_GROW_CHUNK;
    this->_current_token = new char[this->_current_token_buf_size];
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
    this->_retvalInternalCreated = false;
}

Tilc::TJsonParser::~TJsonParser()
{
    if (this->_current_token)
    {
        delete[] this->_current_token;
    }
    this->_deleteCurrentObjectArray();
    this->_deleteCurrentStateArray();
    this->_cleanRetvalObject();
}

void Tilc::TJsonParser::_deleteCurrentObjectArray()
{
    size_t i;
    TJsonParserEntity *entity;

    for (i = 0; i < this->_current_object.size(); ++i)
    {
        entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[i]);
        if (entity)
        {
            delete entity;
        }
    }
    this->_current_object.clear();
}

void Tilc::TJsonParser::_deleteCurrentStateArray()
{
    this->_current_state.clear();
}

void Tilc::TJsonParser::_initParsingProcess(TStdObject* destination)
{
    this->_cleanRetvalObject();
    this->_deleteCurrentObjectArray();
    this->_deleteCurrentStateArray();
    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY);
    if (!this->_current_token)
    {
        this->_current_token_buf_size = JSONPARSER_GROW_CHUNK;
        this->_current_token = new char[this->_current_token_buf_size];
    }
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
    
    if (destination)
    {
        this->_retval = destination;
        this->_retvalInternalCreated = false;
    }
    else
    {
        this->_retval = new Tilc::TStdObject();
        this->_retvalInternalCreated = true;
    }
    
    TJsonParserEntity* entity = new Tilc::TJsonParserEntity(this->_retval, JSONPARSER_CURRENT_ITEM_OBJECT);
    this->_current_object.push_back(entity);
    this->_current_object_attribute_name = "root";
    this->_char_number_in_file = -1;
    this->_char_number_in_line = -1;
}

Tilc::TStdObject* Tilc::TJsonParser::parseFile(const Tilc::TExtString& fname, TStdObject* destination)
{
    this->_error = 0;
    this->_error_msg = "";
    this->_line_number = 1;

    this->_initParsingProcess(destination);

    Tilc::TFile File(fname.c_str());
    Tilc::TExtString Buffer;
    File.ReadContent(Buffer);
    this->_processBuffer(Buffer);

    if (!this->_error && this->_current_state.size() > 1 && this->_current_state[this->_current_state.size() - 1] != JSONPARSER_STATE_STOP_PARSING)
    {
        this->_error = 1;
        this->_error_msg = Tilc::TExtString("Error at line ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Unexpected end of data.");
    }

    TStdObject *retval = this->_retval;
    if (this->_error)
    {
        this->_cleanRetvalObject();
        return nullptr;
    }
    this->_retval = nullptr;
    this->_retvalInternalCreated = false;
	return retval;
}

Tilc::TStdObject* Tilc::TJsonParser::parse(const Tilc::TExtString& content, Tilc::TStdObject* destination)
{
    this->_error = 0;
    this->_error_msg = "";
    this->_line_number = 1;

    this->_initParsingProcess(destination);
    this->_processBuffer(content);

    if (!this->_error && this->_current_state.size() > 1 && this->_current_state[this->_current_state.size()-1] != JSONPARSER_STATE_STOP_PARSING)
    {
        this->_error = 1;
        this->_error_msg = Tilc::TExtString("Error at line ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Unexpected end of data.");
    }

    TStdObject *retval = this->_retval;
    if (this->_error)
    {
        this->_cleanRetvalObject();
        return nullptr;
    }
    this->_retval = nullptr;
    this->_retvalInternalCreated = false;
	return retval;
}

void Tilc::TJsonParser::_processBuffer(const TExtString& buffer)
{
    int i = -1;
    char ch;
    int current_state;

    while (i + 1 < buffer.length())
    {
        this->_char_number_in_line++;
        this->_char_number_in_file++;
        i++;
        ch = buffer[i];

        if (ch == '\n')
        {
            this->_char_number_in_line = -1;
            this->_line_number++;
        }

        current_state = this->_current_state[this->_current_state.size()-1];
        switch (current_state)
        {
        case JSONPARSER_STATE_STOP_PARSING:
            // sprawdzamy, czy w aktualnym buforze są jakieś znaki
            // różne od białych znaków, jeśli tak to błąd
            if (!this->_isWhiteSpace(ch))
            {
                this->_error = 1;
                this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Unexpected data at end of file.");
                return;
            }
            break;

        case JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY:
            if (ch != '{' && ch != '[')
            {
                if (this->_isWhiteSpace(ch))
                {
                    continue;
                }
                else
                {
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Object or array expected.");
                    return;
                }
            }
            else
            {
                if (ch == '[')
                {
                    this->_onBeginArray();
                }
                else if (ch == '{')
                {
                    this->_onBeginObject();
                }
            }
            break;




        case JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY:
            if (ch != '{' && ch != '[' && ch != '\'' && ch != '"' && ch != ']')
            {
                if (this->_isWhiteSpace(ch))
                {
                    continue;
                }
                else
                {
                    // We allow values not quoted so if one is specified then we enter reading value in double quote state
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE);
                    this->_current_token[0] = ch;
                    this->_current_token[1] = 0;
                    this->_current_token_length = 1;
                    this->m_QuotedValueStarted = false;
                    /*
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Object, array or scalar value expected.");
                    return;
                    */
                }
            }
            else
            {
                TJsonParserEntity *entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size()-1]);
                if (entity->entity_type != JSONPARSER_CURRENT_ITEM_ARRAY)
                {
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". INTERNAL ERROR. Array expected.");
                    return;
                }

                switch (ch)
                {
                case '[':
                    this->_onBeginArray();
                    break;
                case '{':
                    this->_onBeginObject();
                    break;
                case ']':
                    // if we closing empty array, then push fake state that is set when at least one array element
                    // is read
                    if (this->_current_state[this->_current_state.size() - 1] == JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY)
                    {
                        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
                        --i;
                        break;
                    }
                    this->_current_state.pop_back();
                    this->_current_state.pop_back();
                    this->_onCloseSquare();
                    break;
                case '"':
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE);
                    this->_current_token[0] = 0;
                    this->_current_token_length = 0;
                    this->m_QuotedValueStarted = true;
                    break;
                case '\'':
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE);
                    this->_current_token[0] = 0;
                    this->_current_token_length = 0;
                    this->m_QuotedValueStarted = true;
                    break;
                }
            }
            break;



        case JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE:
            i = i - this->_onExpectArrayValueInQuote('"', ch);
            if (this->_error)
            {
                return;
            }
            break;



        case JSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE:
            this->_onExpectArrayValueInQuote('\'', ch);
            if (this->_error)
            {
                return;
            }
            break;



        case JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE:
            if (this->_isWhiteSpace(ch))
            {
                continue;
            }
            if (ch == ',')
            {
                this->_current_state.pop_back();
                break;
            }
            if (ch == ']') {
                TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                this->_current_state.pop_back();
                if (this->_current_state[this->_current_state.size()-1] == JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY)
                {
                    this->_current_state.pop_back();
                }
                delete reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                this->_current_object.pop_back();
                if (this->_current_state.size() == 1)
                {
                    this->_current_state.push_back(JSONPARSER_STATE_STOP_PARSING);
                    break;
                }
                else
                {
                    TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                    if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
                    {
                        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
                    }
                    else if (entity->entity_type == JSONPARSER_CURRENT_ITEM_OBJECT)
                    {
                        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                    }
                }
                break;
            }
            // jeśli tu jesteśmy to błąd
            this->_error = 1;
            this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Comma or bracket ] expected.");
            return;




        case JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE:
            if (this->_isWhiteSpace(ch))
            {
                continue;
            }
            if (ch == ',')
            {
                this->_current_state.pop_back();
                break;
            }
            if (ch == '}')
            {
                TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                this->_current_state.pop_back();
                if (this->_current_state[this->_current_state.size()-1] == JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME)
                {
                    this->_current_state.pop_back();
                }
                delete reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                this->_current_object.pop_back();
                if (this->_current_state.size() == 1)
                {
                    this->_current_state.push_back(JSONPARSER_STATE_STOP_PARSING);
                    break;
                }
                else
                {
                    TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                    if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
                    {
                        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
                    }
                    else if (entity->entity_type == JSONPARSER_CURRENT_ITEM_OBJECT)
                    {
                        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                    }
                }
                break;
            }

            // jeśli tu jesteśmy to błąd
            this->_error = 1;
            this->_error_msg = Tilc::TExtString("Error at line ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Parenthesis or comma expected }.");
            return;



        case JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME:
            if (this->_current_token_length == 0 && this->_isWhiteSpace(ch))
            {
                continue;
            }
            if (ch == '\'' && this->_current_token_length == 0 && this->_object_property_name_quote == '~')
            {
                this->_object_property_name_quote = '\'';
                this->m_QuotedNameStarted = true;
                break;
            }
            if (ch == '"' && this->_current_token_length == 0 && this->_object_property_name_quote == '~')
            {
                this->_object_property_name_quote = '"';
                this->m_QuotedNameStarted = true;
                break;
            }
            if (
                (ch == '\'' && this->_object_property_name_quote == '\'')
                ||
                (ch == '"' && this->_object_property_name_quote == '"')
            ) {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1)
                {
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON);
                break;
            }

            if (ch != ':' && this->_current_token_length == 0 && this->_object_property_name_quote == '~')
            {
                // if we have here closing bracket then it is empty object, so add fake state as in case reading of at least one object
                // property
                if (ch == '}')
                {
                    this->_current_state.pop_back();
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                    delete reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                    this->_current_object.pop_back();
                    break;
                }
                this->_object_property_name_quote = '"';
                this->m_QuotedNameStarted = true;
                this->_appendCharToCurrentToken(ch);
                break;
            }

            if (ch == ':')
            {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1)
                {
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE);
                this->m_QuotedNameStarted = false;
                break;
            }

            /* Lines below are comented as we allow spaces within quotated attributes names
            // jeśli mamy biały znak i otwarty jakiś cudzysłów to błąd
            if (this->_object_property_name_quote != '~' && this->_isWhiteSpace(ch))
            {
                this->_error = 1;
                this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Unallowed whitespace in object attribute name.");
                return;
            }
            */

            if (this->_object_property_name_quote == '~' && this->_isWhiteSpace(ch))
            {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1)
                {
                    this->_error = 1;
                    this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON);
                break;
            }

            // jeśli mamy zamykający nawias klamrowy, to
            // koniec pobierania obiektu
            if (ch == '}')
            {
                delete reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
                this->_current_object.pop_back();
                this->_current_state.pop_back();
                if (this->_current_state[this->_current_state.size()-1] == JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE)
                {
                    this->_current_state.pop_back();
                }
                this->_onCloseSquare();
                break;
            }

            // tutaj dodajemy znak do tokena
            this->_appendCharToCurrentToken(ch);

            break;




        case JSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON:
            if (this->_isWhiteSpace(ch))
            {
                continue;
            }
            if (ch == ':')
            {
                this->_current_state.pop_back();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE);
                break;
            }
            // jeśli tu jesteśmy, to błąd
            this->_error = 1;
            this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Colon expected ':'.");
            return;



        case JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE:
            if (this->_current_token_length == 0)
            {
                if (this->_isWhiteSpace(ch))
                {
                    continue;
                }
                if (ch == '{')
                {
                    this->_current_state.pop_back();
                    this->_onBeginObject();
                    break;
                }
                if (ch == '[')
                {
                    this->_current_state.pop_back();
                    this->_onBeginArray();
                    break;
                }
                if (ch == '\'')
                {
                    this->_current_state.pop_back();
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE);
                    this->m_QuotedValueStarted = true;
                    break;
                }
                if (ch == '"')
                {
                    this->_current_state.pop_back();
                    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE);
                    this->m_QuotedValueStarted = true;
                    break;
                }

                // We allow not quoted object attribute's values
                this->_current_state.pop_back();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE);
                this->m_QuotedValueStarted = false;
                // tutaj dodajemy znak do tokena
                this->_appendCharToCurrentToken(ch);
                break;
                /*
                // jeśli mamy inny znak, to błąd - wartość powinna być
                // w cudzysłowach
                this->_error = 1;
                this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Scalar value of object attribute [") + this->_current_object_attribute_name + "] should be quoted.";
                return;
                */
            }

            // jeśli tu jesteśmy, to błąd
            this->_error = 1;
            this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Scalar value of object attribute should be quoted.");
            return;



        case JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE:
            i = i - this->_onExpectObjectValueInQuote('"', ch);
            if (this->_error)
            {
                return;
            }
            break;



        case JSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE:
            this->_onExpectObjectValueInQuote('\'', ch);
            if (this->_error)
            {
                return;
            }
            break;
        }

        if (this->_current_state.size() < 1)
        {
            break;
        }
    }
}


int Tilc::TJsonParser::_onExpectArrayValueInQuote(char quoteType, char ch)
{
    int NumBack = 0;
    // if we are reading not quoted value
    if (!this->m_QuotedValueStarted)
    {
        // then space means that reading value is over
        if (this->_isWhiteSpace(ch) || ch == ',' || ch == ']')
        {
            ch = quoteType;
            NumBack = 1;
        }
    }
    if (ch == quoteType)
    {
        TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
        if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
        {
            Tilc::TExtString v = this->_getStringFromCurrentToken();
            this->_current_token[0] = 0;
            this->_current_token_length = 0;
            TPointerList *a = (TPointerList*)entity->entity;
            TStdObjectProperty *p = nullptr;
			bool isInt, isDouble;
			if (v.isNumeric(&isInt, &isDouble))
            {
				if (isInt)
                {
					p = new TStdObjectProperty(Tilc::TExtString(std::to_string(a->size())), v.toInt());
				}
                else if (isDouble)
                {
					p = new TStdObjectProperty(Tilc::TExtString(std::to_string(a->size())), v.toDouble());
				} else
                {
					this->_error = 1;
					this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Improper numerical array value (not LONG and not DOUBLE).");
				}
			}
            else
            {
				p = new TStdObjectProperty(Tilc::TExtString(std::to_string(a->size())), v);
			}
            a->push_back(p);
            this->_current_state.pop_back();
            this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
            return NumBack;
        }
        else
        {
            this->_error = 1;
            this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". INTERNAL ERROR. Array expected.");
            return NumBack;
        }
    }
    else
    {
        this->_appendCharToCurrentToken(ch);
    }
    return NumBack;
}

int Tilc::TJsonParser::_onExpectObjectValueInQuote(char quoteType, char ch)
{
    int NumBack = 0;
    if (this->m_QuotedValueStarted && ch == quoteType)
    {
        this->_onCompleteReadObjectAttributeSimpleValue();
        this->_current_state.pop_back();
        this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
        this->m_QuotedValueStarted = false;
    }
    else
    {
        if (!this->m_QuotedValueStarted)
        {
            if (ch == ' ')
            {
                this->_onCompleteReadObjectAttributeSimpleValue();
                this->_current_state.pop_back();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                this->m_QuotedValueStarted = false;
            }
            else if (ch == ',' || ch == '}')
            {
                // in this case we need to move backward one character to properly parse json
                this->_onCompleteReadObjectAttributeSimpleValue();
                this->_current_state.pop_back();
                this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                this->m_QuotedValueStarted = false;
                NumBack = 1;
            }
            else
            {
                this->_appendCharToCurrentToken(ch);
            }
        }
        else
        {
            this->_appendCharToCurrentToken(ch);
        }
    }
    return NumBack;
}

void Tilc::TJsonParser::_onBeginArray()
{
    TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
    TPointerList* newA = new TPointerList();
    if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
    {
        TPointerList *a = (TPointerList*)entity->entity;
        TStdObjectProperty *p = new TStdObjectProperty(Tilc::TExtString(std::to_string(a->size())), newA);
        a->push_back(p);
    }
    else if (entity->entity_type == JSONPARSER_CURRENT_ITEM_OBJECT)
    {
        TStdObject *o = (TStdObject*)entity->entity;
        o->set(this->_current_object_attribute_name, newA);
    }
    TJsonParserEntity *new_entity = new TJsonParserEntity(newA, JSONPARSER_CURRENT_ITEM_ARRAY);
    this->_current_object.push_back(new_entity);
    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY);
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
}

void Tilc::TJsonParser::_onBeginObject()
{
    TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size()-1]);
    TStdObject *newO = new TStdObject();
    if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
    {
        TPointerList *a = (TPointerList*)entity->entity;
        TStdObjectProperty *p = new TStdObjectProperty(Tilc::TExtString(std::to_string(a->size())), newO);
        a->push_back(p);
    }
    else if (entity->entity_type == JSONPARSER_CURRENT_ITEM_OBJECT)
    {
        TStdObject *o = (TStdObject*)entity->entity;
        o->set(this->_current_object_attribute_name, newO);
    }
    TJsonParserEntity *new_entity = new TJsonParserEntity(newO, JSONPARSER_CURRENT_ITEM_OBJECT);
    this->_current_object.push_back(new_entity);
    this->_current_state.push_back(JSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME);
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

Tilc::TExtString Tilc::TJsonParser::_getStringFromCurrentToken()
{
    long outbuf_size = this->_current_token_length + 1;
    //wchar_t *outbuf = new wchar_t[outbuf_size];
    //MultiByteToWideChar(CP_UTF8, 0, &this->_current_token[0], -1, outbuf, outbuf_size);
    //Tilc::TExtString v(outbuf, outbuf_size);
    Tilc::TExtString v(this->_current_token);
    return v;
}

void Tilc::TJsonParser::_onCompleteReadObjectAttributeName() {
    this->_current_object_attribute_name = this->_getStringFromCurrentToken();
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

void Tilc::TJsonParser::_onCompleteReadObjectAttributeSimpleValue()
{
    TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
    if (entity->entity_type != JSONPARSER_CURRENT_ITEM_OBJECT)
    {
        this->_error = 1;
        this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". INTERNAL ERROR. Object expected.");
        return;
    }
    TStdObject *o = (TStdObject*)entity->entity;
    Tilc::TExtString v = this->_getStringFromCurrentToken();
	bool isInt, isDouble;
	if (v.isNumeric(&isInt, &isDouble))
    {
		if (isInt)
        {
			o->set(this->_current_object_attribute_name, v.toInt());
		}
        else if (isDouble)
        {
			o->set(this->_current_object_attribute_name, v.toDouble());
		}
        else
        {
			this->_error = 1;
			this->_error_msg = Tilc::TExtString("Error at line: ") + this->_line_number + " at position " + this->_char_number_in_line + Tilc::TExtString(". Improper numerical scalar value of object attribute (not LONG and not DOUBLE).");
		}
	}
    else
    {
		o->set(this->_current_object_attribute_name, v);
	}
    this->_current_object_attribute_name = "";
    this->_current_token[0] = 0;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

void Tilc::TJsonParser::_appendCharToCurrentToken(char ch)
{
    if (this->_current_token_length >= this->_current_token_buf_size - 1)
    {
        long new_size = this->_current_token_buf_size + JSONPARSER_GROW_CHUNK;
        char *tmp = new char[new_size];
        memcpy(tmp, this->_current_token, this->_current_token_length);
        delete[] this->_current_token;
        this->_current_token = tmp;
        this->_current_token_buf_size = new_size;
    }
    this->_current_token[this->_current_token_length] = ch;
    this->_current_token_length++;
    this->_current_token[this->_current_token_length] = 0;
}

void Tilc::TJsonParser::_onCloseSquare()
{
    if (this->_current_state.size() == 1)
    {
        this->_current_state.push_back(JSONPARSER_STATE_STOP_PARSING);
    }
    else
    {
        TJsonParserEntity* entity = reinterpret_cast<TJsonParserEntity*>(this->_current_object[this->_current_object.size() - 1]);
        if (entity->entity_type == JSONPARSER_CURRENT_ITEM_ARRAY)
        {
            this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
        }
        else if (entity->entity_type == JSONPARSER_CURRENT_ITEM_OBJECT)
        {
            this->_current_state.push_back(JSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
        }
    }
}

void Tilc::TJsonParser::_cleanRetvalObject()
{
    if (this->_retval)
    {
        if (this->_retvalInternalCreated)
        {
            delete this->_retval;
        }
        this->_retval = nullptr;
        this->_retvalInternalCreated = false;
    }
}

