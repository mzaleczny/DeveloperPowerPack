#include "Tilc/Utils/StdObject.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Utils/FileUtils.h"
#include <iostream>

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TExtString& name, const Tilc::TExtString& value)
{
    this->type = PROPERTY_VALUE;
    this->name = name;
    this->value = value;
}

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TExtString& name, int64_t value)
{
    this->type = PROPERTY_VALUE_INT;
    this->name = name;
    this->value = "";
	this->iValue = value;
}

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TExtString& name, double value)
{
    this->type = PROPERTY_VALUE_DOUBLE;
    this->name = name;
    this->value = "";
	this->fValue = value;
}

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TExtString& name, TPointersVector* value_array)
{
    this->type = PROPERTY_ARRAY;
    this->name = name;
    this->value = "";
    aValue = value_array;
}

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TExtString& name, TStdObject *value_object)
{
    this->type = PROPERTY_OBJECT;
    this->name = name;
    this->value = "";
    this->oValue = value_object;
}

Tilc::TStdObjectProperty::TStdObjectProperty(const Tilc::TStdObjectProperty& copyObj)
{
    this->type = copyObj.type;
    this->name = copyObj.name;
    this->value = copyObj.value;
    size_t size = copyObj.aValue->size();
    if (size > 0)
    {
        FreeAValue();
        aValue->resize(size);
        Tilc::TStdObjectProperty* copyItem;
        for (size_t i = 0; i < size; ++i)
        {
            copyItem = reinterpret_cast<TStdObjectProperty*>((*copyObj.aValue)[i]);
            aValue->push_back(copyItem->clone());
        }
    }
    else
    {
        FreeAValue();
    }

    if (this->oValue)
    {
        delete this->oValue;
    }
    if (copyObj.oValue)
    {
        this->oValue = copyObj.oValue->clone();
    }
    else
    {
        this->oValue = nullptr;
    }

    if (this->extraData)
    {
        delete this->extraData;
    }
    if (copyObj.extraData)
    {
        this->extraData = copyObj.extraData->clone();
    }
    else
    {
        this->extraData = nullptr;
    }
}

Tilc::TStdObjectProperty::~TStdObjectProperty()
{
    this->clearValue();
}

void Tilc::TStdObjectProperty::clearValue()
{
    FreeAValue();

    if (this->oValue)
    {
        delete this->oValue;
        this->oValue = nullptr;
    }
	if (this->extraData)
    {
        delete this->extraData;
        this->extraData = nullptr;
	}
    this->value = "";
    this->type = PROPERTY_NONE;
}

void Tilc::TStdObjectProperty::FreeAValue()
{
    if (aValue)
    {
        for (size_t i = 0; i < aValue->size(); ++i)
        {
            delete reinterpret_cast<TStdObjectProperty*>((*aValue)[i]);
        }
        this->aValue->clear();
        delete aValue;
        aValue = nullptr;
    }
}

Tilc::TStdObjectProperty* Tilc::TStdObjectProperty::clone()
{
    Tilc::TStdObjectProperty* newValue = new Tilc::TStdObjectProperty(*this);
    return newValue;
}

Tilc::TExtString Tilc::TStdObjectProperty::getAsString()
{
    switch (this->type)
    {
        case PROPERTY_VALUE:
            return this->value;
        case PROPERTY_VALUE_INT:
            return Tilc::TExtString(std::to_string(iValue));
        case PROPERTY_VALUE_DOUBLE:
            return Tilc::TExtString(std::to_string(fValue));
    }
    return "";
}

bool Tilc::TStdObjectProperty::hasOnlyScalars()
{
    if (this->type == PROPERTY_VALUE || this->type == PROPERTY_VALUE_INT || this->type == PROPERTY_VALUE_DOUBLE)
    {
        return true;
    }

    if (this->type == PROPERTY_ARRAY)
    {
        return this->_arrayHasOnlyScalars();
    }

    if (this->type == PROPERTY_OBJECT)
    {
        return this->_objectHasOnlyScalars();
    }

    return false;
}

bool Tilc::TStdObjectProperty::_arrayHasOnlyScalars()
{
    bool onlyScalars = true;
    if (aValue->size() < 1)
    {
        return false;
    }

    Tilc::TStdObjectProperty *item;
    for (size_t i = 0; i < aValue->size(); ++i)
    {
        item = reinterpret_cast<Tilc::TStdObjectProperty*>((*aValue)[i]);
        if (item->type == PROPERTY_ARRAY || item->type == PROPERTY_OBJECT)
        {
            onlyScalars = false;
            break;
        }
    }

    return onlyScalars;
}

bool Tilc::TStdObjectProperty::_objectHasOnlyScalars()
{
    return false;
}








Tilc::TStdObject::TStdObject()
{
}

Tilc::TStdObject::TStdObject(const Tilc::TStdObject& copyObj)
{
    size_t size = copyObj.itemsCount();
    FreeProperties();
    this->_properties.resize(size);

    Tilc::TStdObjectProperty* copyItem;
    for (size_t i = 0; i < size; ++i)
    {
        copyItem = (TStdObjectProperty*)_properties[i];
        this->_properties.push_back(copyItem->clone());
    }
}

Tilc::TStdObject::~TStdObject()
{
    FreeProperties();
}

Tilc::TStdObject* Tilc::TStdObject::clone()
{
    Tilc::TStdObject* newValue = new Tilc::TStdObject(*this);
    return newValue;
}

void Tilc::TStdObject::set(const Tilc::TExtString& name, const Tilc::TExtString& value)
{
    Tilc::TStdObjectProperty *p;

    p = this->get(name);
    if (p)
    {
        p->clearValue();
        p->type = PROPERTY_VALUE;
        p->value = value;
    }
    else
    {
        p = new Tilc::TStdObjectProperty(name, value);
        this->_properties.push_back(p);
    }
}

void Tilc::TStdObject::set(const Tilc::TExtString& name, int64_t value)
{
    Tilc::TStdObjectProperty *p;

    p = this->get(name);
    if (p)
    {
        p->clearValue();
        p->type = PROPERTY_VALUE_INT;
        p->iValue = value;
    }
    else
    {
        p = new Tilc::TStdObjectProperty(name, value);
        this->_properties.push_back(p);
    }
}

void Tilc::TStdObject::set(const Tilc::TExtString& name, double value)
{
    Tilc::TStdObjectProperty *p;

    p = this->get(name);
    if (p)
    {
        p->clearValue();
        p->type = PROPERTY_VALUE_DOUBLE;
        p->fValue = value;
    }
    else
    {
        p = new Tilc::TStdObjectProperty(name, value);
        this->_properties.push_back(p);
    }
}

void Tilc::TStdObject::set(const Tilc::TExtString& name, TPointersVector* aValue)
{
    Tilc::TStdObjectProperty *p;

    p = this->get(name);
    if (p)
    {
        p->clearValue();
        p->type = PROPERTY_ARRAY;
        p->aValue = aValue;
    }
    else
    {
        p = new Tilc::TStdObjectProperty(name, aValue);
        this->_properties.push_back(p);
    }
}

void Tilc::TStdObject::set(const Tilc::TExtString& name, Tilc::TStdObject *oValue)
{
    Tilc::TStdObjectProperty *p;

    p = this->get(name);
    if (p)
    {
        p->clearValue();
        p->type = PROPERTY_OBJECT;
        p->oValue = oValue;
    }
    else
    {
        p = new Tilc::TStdObjectProperty(name, oValue);
        this->_properties.push_back(p);
    }
}

Tilc::TStdObjectProperty* Tilc::TStdObject::get(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty *p;
    
    for (size_t i = 0; i < this->_properties.size(); ++i)
    {
        p = reinterpret_cast<Tilc::TStdObjectProperty*>(this->_properties[i]);
        if (p->name == name)
        {
            return p;
        }
    }

    return nullptr;
}

Tilc::TExtString Tilc::TStdObject::getAsString(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty* property = this->get(name);
    if (property)
    {
        switch (property->type)
        {
            case PROPERTY_VALUE:
                return property->value;
            case PROPERTY_VALUE_INT:
                return Tilc::TExtString(std::to_string(property->iValue));
            case PROPERTY_VALUE_DOUBLE:
                return Tilc::TExtString(std::to_string(property->fValue));
        }
    }
    return "";
}

int64_t Tilc::TStdObject::getAsInt(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty* property = this->get(name);
    if (property)
    {
        switch (property->type)
        {
            case PROPERTY_VALUE:
                return property->value.toInt();
            case PROPERTY_VALUE_INT:
                return property->iValue;
            case PROPERTY_VALUE_DOUBLE:
                return (int64_t)property->fValue;
        }
    }
    return 0;
}

double Tilc::TStdObject::getAsDouble(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty* property = this->get(name);
    if (property)
    {
        switch (property->type)
        {
            case PROPERTY_VALUE:
                return property->value.toDouble();
            case PROPERTY_VALUE_DOUBLE:
                return property->fValue;
            case PROPERTY_VALUE_INT:
                return property->iValue;
        }
    }
    return 0.0;
}

Tilc::TPointersVector* Tilc::TStdObject::getAsArray(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty* property = this->get(name);
    if (property && property->type == PROPERTY_ARRAY)
    {
        return property->aValue;
    }
    return nullptr;
}

Tilc::TStdObject* Tilc::TStdObject::getAsObject(const Tilc::TExtString& name)
{
    Tilc::TStdObjectProperty* property = this->get(name);
    if (property && property->type == PROPERTY_OBJECT)
    {
        return property->oValue;
    }
    return nullptr;
}

Tilc::TStdObject* Tilc::TStdObject::getSubObject(const Tilc::TExtString& Path)
{
    size_t SeparatorPos = Path.find("/");
    if (SeparatorPos != std::string::npos)
    {
        Tilc::TExtString Name = Path.substr(0, SeparatorPos);
        Tilc::TStdObjectProperty* Property = this->get(Name);
        if (Property && Property->type == PROPERTY_OBJECT)
        {
            return Property->oValue->getSubObject(Path.substr(SeparatorPos + 1));
        }
    }
    else if (!Path.empty())
    {
        Tilc::TStdObjectProperty* Property = this->get(Path);
        if (Property && Property->type == PROPERTY_OBJECT)
        {
            return Property->oValue;
        }
    }
    return nullptr;
}

Tilc::TStdObjectProperty* Tilc::TStdObject::operator[](char* name)
{
    return this->get(name);
}

Tilc::TExtString Tilc::TStdObject::toJson(const Tilc::TExtString& quote)
{
    Tilc::TExtString json = "";
    Tilc::TStdObjectProperty *root;

    if (!this)
    {
        return json;
    }

    this->_quote = quote;
    if (this->_properties.size() > 0)
    {
        root = this->get("root");
        if (root)
        {
            json += this->_propertyToJson(root, "");
        }
    }

    return json;
}

Tilc::TExtString Tilc::TStdObject::_propertyToJson(Tilc::TStdObjectProperty *p, const Tilc::TExtString& indent)
{
    Tilc::TExtString json = "";

    if (p->type == PROPERTY_VALUE || p->type == PROPERTY_VALUE_INT || p->type == PROPERTY_VALUE_DOUBLE)
    {
        return this->_valueToJson(p, "");
    }

    if (p->type == PROPERTY_ARRAY)
    {
        return this->_arrayToJson(p, "");
    }

    if (p->type == PROPERTY_OBJECT)
    {
        return this->_objectToJson(p, "");
    }

    return "INTERNAL ERROR. This value should not be returned!";
}

Tilc::TExtString Tilc::TStdObject::_valueToJson(Tilc::TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName)
{
    Tilc::TExtString Value;
    switch (p->type)
    {
    case PROPERTY_VALUE:
        Value = p->value;
        break;
    case PROPERTY_VALUE_INT:
        Value = std::to_string(p->iValue);
        break;
    case PROPERTY_VALUE_DOUBLE:
        Value = std::to_string(p->fValue);
        break;
    }

    if (withName)
    {
        return indent + p->name + ": " + this->_quote + Value + this->_quote;
    }

    return indent + this->_quote + Value + this->_quote;
}

Tilc::TExtString Tilc::TStdObject::_arrayToJson(Tilc::TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName)
{
    Tilc::TStdObjectProperty *item;
    bool onlyScalars;
    Tilc::TExtString json;

    if (p->aValue->size() > 0)
    {
        // najpierw sprawdzamy czy ta tablica zawiera same wartości
        // skalarne
        onlyScalars = p->hasOnlyScalars();

        if (withName)
        {
            json = indent + p->name + ": [";
        }
        else
        {
            json = indent + "[";
        }
        // jeśli mamy same skalary, to je drukujemy w rządku
        if (onlyScalars)
        {
            size_t len = p->aValue->size();
            for (size_t i = 0; i < len; ++i)
            {
                item = reinterpret_cast<Tilc::TStdObjectProperty*>((*p->aValue)[i]);
                json += this->_valueToJson(item, "");
                if (i < len - 1)
                {
                    json += ", ";
                }
            }
            json += "]";
        }
        else
        {
            size_t len = p->aValue->size();
            for (size_t i = 0; i < len; ++i)
            {
                item = reinterpret_cast<Tilc::TStdObjectProperty*>((*p->aValue)[i]);
                if (item->type == PROPERTY_VALUE || item->type == PROPERTY_VALUE_INT || item->type == PROPERTY_VALUE_DOUBLE)
                {
                    json += Tilc::TExtString("\n") + indent + DEFAULT_INDENT + this->_valueToJson(item, "");
                }
                else if (item->type == PROPERTY_ARRAY)
                {
                    json += Tilc::TExtString("\n") + this->_arrayToJson(item, indent + DEFAULT_INDENT);
                }
                else if (item->type == PROPERTY_OBJECT)
                {
                    json += Tilc::TExtString("\n") + this->_objectToJson(item, indent + DEFAULT_INDENT);
                }
                if (i + 1 < len)
                {
                    json += ",";
                }
            }
            json += "\n";
            json += indent + "]";
        }
        return json;
    }
    else
    {
        if (withName)
        {
            return indent + p->name + ": []";
        }
        return indent + "[]";
    }
}

Tilc::TExtString Tilc::TStdObject::_objectToJson(Tilc::TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName)
{
    Tilc::TExtString json = "";

    if (withName)
    {
        json = indent + p->name + ": {";
    }
    else
    {
        json = indent + "{";
    }
    TPointersVector& a = p->oValue->_properties;
    size_t len = a.size();
    if (len == 0)
    {
        json += "}";
        return json;
    }
    Tilc::TStdObjectProperty *item;
    for (size_t i = 0; i < len; ++i)
    {
        item = reinterpret_cast<Tilc::TStdObjectProperty*>(a[i]);
        if (item->type == PROPERTY_VALUE || item->type == PROPERTY_VALUE_INT || item->type == PROPERTY_VALUE_DOUBLE)
        {
            json += Tilc::TExtString("\n") + indent + DEFAULT_INDENT + this->_valueToJson(item, "", true);
        }
        else if (item->type == PROPERTY_ARRAY)
        {
            json += Tilc::TExtString("\n") + this->_arrayToJson(item, indent + DEFAULT_INDENT, true);
        }
        else if (item->type == PROPERTY_OBJECT)
        {
            json += Tilc::TExtString("\n") + this->_objectToJson(item, indent + DEFAULT_INDENT, true);
        }
        if (i + 1 < len)
        {
            json += ",";
        }
    }
    json += "\n";
    json += indent + "}";

    return json;
}

bool Tilc::TStdObject::loadFromFile(const Tilc::TExtString& fname)
{
    if (FileExists(fname))
    {
        Tilc::TJsonParser parser;
        this->clean();
        Tilc::TStdObject* data = parser.parseFile(fname, this);
        if (data)
        {
            return true;
        }
        else
        {
            if (!parser.isError())
            {
                //AlertBox(Tilc::TExtString("No json objects found in file [") + fname + "]");
                std::cout << "No json objects found in file [" << fname << "]" << std::endl;
            }
            else
            {
                //ErrorBox(parser.getErrorMessage());
                std::cerr << parser.getErrorMessage() << std::endl;
            }
        }
    }
    return false;
}

void Tilc::TStdObject::clean()
{
    FreeProperties();
}

void Tilc::TStdObject::FreeProperties()
{
    for (size_t i = 0; i < _properties.size(); ++i)
    {
        delete reinterpret_cast<TStdObjectProperty*>(_properties[i]);
    }
    _properties.clear();
}
