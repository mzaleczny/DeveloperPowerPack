#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/StdObject.h"
#include <vector>

namespace Tilc {

    class TStdObject;
    class TStdObjectProperty;

    const constexpr char* DEFAULT_INDENT		= "    ";
    const constexpr int PROPERTY_NONE			= 0;
    const constexpr int PROPERTY_VALUE			= 1;
    const constexpr int PROPERTY_VALUE_INT		= 2;
    const constexpr int PROPERTY_VALUE_DOUBLE	= 3;
    const constexpr int PROPERTY_ARRAY			= 4;
    const constexpr int PROPERTY_OBJECT			= 5;


    class DECLSPEC TStdObjectProperty {
    public:
        // dostępne typy:
        //   'scalar' - pole value przechowuje wartość (typu Tilc::TExtString) dla tej właściwości
        //   'scalar_int' - pole value przechowuje wartość (typu LONG) dla tej właściwości
        //   'scalar_double' - pole value przechowuje wartość (typu double) dla tej właściwości
        //   'array'  - pole aValue zawiera tablicę obiektów CmzStdObjectProperty
        //   'object' - pole oValue zawiera wskaźnik na obiekt CmzStdObject
        int type{ PROPERTY_VALUE_INT };
        Tilc::TExtString name;
        Tilc::TExtString value;
	    int64_t iValue{};
	    double fValue{};
        TPointersVector* aValue{};
        TStdObject* oValue{};
	    TStdObject* extraData{};

        TStdObjectProperty(const Tilc::TExtString& name, const Tilc::TExtString& value);
        TStdObjectProperty(const Tilc::TExtString& name, int64_t value);
        TStdObjectProperty(const Tilc::TExtString& name, double value);
        TStdObjectProperty(const Tilc::TExtString& name, TPointersVector* value_array);
        TStdObjectProperty(const Tilc::TExtString& name, TStdObject *value_object);
        TStdObjectProperty(const TStdObjectProperty& copyObj);
        ~TStdObjectProperty();

        void clearValue();
        void FreeAValue();
        bool hasOnlyScalars();
        TStdObjectProperty* clone();
        Tilc::TExtString getAsString();

    protected:
        TStdObjectProperty() {};
        bool _arrayHasOnlyScalars();
        bool _objectHasOnlyScalars();
    };
    typedef TStdObjectProperty *PStdObjectProperty;




    class DECLSPEC TStdObject {
    public:
        TStdObject();
        TStdObject(const TStdObject& copyObj);
        virtual ~TStdObject();

        void set(const Tilc::TExtString& name, const Tilc::TExtString& value);
        void set(const Tilc::TExtString& name, int64_t value);
        void set(const Tilc::TExtString& name, double value);
        void set(const Tilc::TExtString& name, TPointersVector* aValue);
        void set(const Tilc::TExtString& name, TStdObject* oValue);
        TStdObjectProperty* get(const Tilc::TExtString& name);
        Tilc::TExtString getAsString(const Tilc::TExtString& name);
        int64_t getAsInt(const Tilc::TExtString& name);
        double getAsDouble(const Tilc::TExtString& name);
        TPointersVector* getAsArray(const Tilc::TExtString& name);
        TStdObject* getAsObject(const Tilc::TExtString& name);
        // Below Path is of format ex. "Person/Data/Age" relative to current/calling StdObject
        TStdObject* getSubObject(const Tilc::TExtString& Path);
        Tilc::TExtString toJson(const Tilc::TExtString& quote = "\"");
        bool loadFromFile(const Tilc::TExtString& fname);
        inline size_t itemsCount() const { return this->_properties.size(); }
        TStdObject* clone();
        void clean();
    
        // przeciążone operatory
        TStdObjectProperty* operator[](char* name);
        void FreeProperties();

    protected:
        TPointersVector _properties;
        Tilc::TExtString _quote;

        Tilc::TExtString _propertyToJson(TStdObjectProperty *p, const Tilc::TExtString& indent);
        Tilc::TExtString _valueToJson(TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName = false);
        Tilc::TExtString _arrayToJson(TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName = false);
        Tilc::TExtString _objectToJson(TStdObjectProperty *p, const Tilc::TExtString& indent, bool withName = false);
    };

}
