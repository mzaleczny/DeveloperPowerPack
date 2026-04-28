#pragma once

#include <string>
#include <vector>
#include "Tilc/DllGlobals.h"

#define UTF8_PL_A		"\xc4\x84" // "Ą"
#define UTF8_PL_C		"\xc4\x86" // "Ć"
#define UTF8_PL_E		"\xc4\x98" // "Ę"
#define UTF8_PL_L		"\xc5\x81" // "Ł"
#define UTF8_PL_N		"\xc5\x83" // "Ń"
#define UTF8_PL_O		"\xc3\x93" // "Ó"
#define UTF8_PL_S		"\xc5\x9a" // "Ś"
#define UTF8_PL_Z1		"\xc5\xb9" // "Ź"
#define UTF8_PL_Z2		"\xc5\xbb" // "Ż"
#define UTF8_PL_A_SM	"\xc4\x85" // "ą"
#define UTF8_PL_C_SM	"\xc4\x87" // "ć"
#define UTF8_PL_E_SM	"\xc4\x99" // "ę"
#define UTF8_PL_L_SM	"\xc5\x82" // "ł"
#define UTF8_PL_N_SM	"\xc5\x84" // "ń"
#define UTF8_PL_O_SM	"\xc3\xb3" // "ó"
#define UTF8_PL_S_SM	"\xc5\x9b" // "ś"
#define UTF8_PL_Z1_SM	"\xc5\xba" // "ź"
#define UTF8_PL_Z2_SM	"\xc5\xbc" // "ż"


namespace Tilc
{

	const constexpr int FORMAT_ALIGN_LEFT	= 0;
	const constexpr int FORMAT_ALIGN_RIGHT	= 1;


	const constexpr int ENC_UTF8_WITH_BOM	= 1;
	const constexpr int ENC_UTF8			= 2;
	const constexpr int	ENC_UNICODE			= 3;
	const constexpr int ENC_ANSI			= 10;
	const constexpr int ENC_ISO2			= 11;
	const constexpr int ENC_OEM_852			= 12;


	class DECLSPEC TExtString : public std::string
	{
	public:
		using std::string::string;

		static int LettersArray[];

		TExtString(const std::string& s) { assign(s.c_str()); }
		TExtString(const std::wstring& Source, unsigned int CodePage) { FromWideString(Source, CodePage); }
		TExtString(const wchar_t* Source, size_t SourceLength, unsigned int CodePage) { FromWideString(Source, SourceLength, CodePage); }

        bool operator==(const char* Value)
        {
            return this->compare(Value) == 0;
        }
        bool operator!=(const char* Value)
        {
            return this->compare(Value) != 0;
        }
        TExtString operator+(int64_t Value)
		{
			return TExtString(c_str() + std::to_string(Value));
		}
		TExtString operator+(int32_t Value)
		{
			return TExtString(c_str() + std::to_string(Value));
		}
		TExtString operator+(char Value)
		{
			return TExtString(c_str() + std::to_string(Value));
		}
		TExtString operator+(wchar_t Value)
		{
			wchar_t s[2]{ L"" };
			s[0] = Value;
			TExtString Result(*this);
			Result.append(FromWideString(s, 1));
			return Result;
		}
		TExtString operator+(const char* Value)
		{
			TExtString Result = *this;
			Result.append(Value);
			return Result;
		}
		TExtString operator+(const wchar_t* Value)
		{
			TExtString Result(*this);
			Result.append(FromWideString(Value, wcslen(Value)));
			return Result;
		}
		TExtString& operator+=(int64_t Value)
		{
			*this = *this + Value;
			return *this;
		}
		TExtString& operator+=(int32_t Value)
		{
			*this = *this + Value;
			return *this;
		}
		TExtString& operator+=(char Value)
		{
            push_back(Value);
			return *this;
		}
		TExtString& operator+=(wchar_t Value)
		{
			*this = *this + Value;
			return *this;
		}
		TExtString& operator+=(const char* Value)
		{
			*this = *this + Value;
			return *this;
		}
		TExtString& operator+=(const wchar_t* Value)
		{
			*this = *this + Value;
			return *this;
		}
		TExtString& operator+=(const TExtString& Value)
		{
			*this = *this + Value;
			return *this;
		}

        inline bool IsEmpty() { return size() == 0; }
		Tilc::TExtString UnshiftLeft(char ToChar);
		Tilc::TExtString UnshiftRight(char ToChar);
		size_t Explode(char Explodechar, std::vector<Tilc::TExtString>& Items);
		size_t Explode(const Tilc::TExtString& ExplodeString, std::vector<Tilc::TExtString>& Items);
		std::wstring ToWideString(unsigned int CodePage);
		const char* FromWideString(const std::wstring& Source, unsigned int CodePage);
		const char* FromWideString(const wchar_t* Source, size_t SourceLength, unsigned int CodePage);
		Tilc::TExtString SplitIntoMultiline(int MaxcharCount);
		const char* FromUtf8(const char* utf8str, const std::locale& loc);
		const char* AnsiToUtf8(const char* AnsiStr);
		static const char* AnsiPlcharToUtf8PlString(char plchar);
		const char* ToUppercase();
		const char* ToLowercase();
		const char* StrReplace(Tilc::TExtString What, Tilc::TExtString ReplaceWith);
		bool IsUtf8PL();

        bool isInt() const;
        bool isDouble() const;
        bool isNumeric(bool* isInt = nullptr, bool* isDouble = nullptr) const;

        int64_t inline toInt() { return std::stoll(c_str()); }
        float inline toFloat() { return std::stof(c_str()); }
        double inline toDouble() { return std::stod(c_str()); }

		const char* ImplodeInts(char Implodechar, std::vector<int>& Items);
		static inline Tilc::TExtString WidecharToString(wchar_t Widechar, unsigned int CodePage);

		void RTrim(char ch);
		void RTrimAllChars(const char* StringOfChars);
		inline void RTrim()
		{
			this->RTrimAllChars(" \t\n\r");
		}
		void LTrim(char ch);
		void LTrimAllChars(const char* StringOfChars);
		inline void LTrim()
		{
			this->LTrimAllChars(" \t\n\r");
		}
		inline void Trim(char ch)
		{
			this->RTrim(ch);
			this->LTrim(ch);
		}
		inline void TrimAllChars(const char* StringOfChars)
		{
			this->RTrimAllChars(StringOfChars);
			this->LTrimAllChars(StringOfChars);
		}
		inline void Trim()
		{
			this->RTrim();
			this->LTrim();
		}

		// Znajduje ostatnie wystąpienie podanego znaku i wstawia w jego miejsce NULL, czyli
		// obcina łańcuch na ostatnim wystąpieniu tego znaku.
		void CutAtLast(char ch);
		void CutAtLastchar(const char* StringOfChars);

		// Zwraca podłańcuch znajdujący się po ostatnim wystąpieniu znaku ch
		TExtString StrAfterLast(char ch);
		// Zwraca podłańcuch znajdujący się po ostatnim wystąpieniu któregokolwiek ze znaków podanych w ciągu
		// 'stringOfChars'
		TExtString StrAfterLastchar(const char* stringOfChars);

        void InsertAt(size_t pos, const TExtString& s);
        void DeleteCharAt(size_t pos);
        // Removes single Utf8 char just befor current pos and returns bytes removed
        int DeleteSingleUtf8CharBeforePos(size_t pos);
        // Removes single Utf8 char at current pos and returns bytes removed
        int DeleteSingleUtf8CharAtPos(size_t pos);
        // zwraca długość w bajtach znaku utf8 na pozycji pos
        int GetUtf8CharLength(size_t pos);
        // zwraca długość w bajtach NumChars kolejnych liter utf8 począwszy pozycji pos
        int GetUtf8CharsLength(size_t pos, int NumChars);
        // zwraca długość w bajtach NumChars kolejnych liter utf8 bezpośrednio poprzedzających pozycję pos
        int GetPrecedingUtf8CharsLength(size_t pos, int NumChars);
        int TruncateUtf8AtEnd(size_t NumChars);

		// dokleja na początku tego łańcucha odpowiedni element i zwraca referencję do tego łańcucha.
		TExtString& Prepend(TExtString& s);
		TExtString& Prepend(const char* s);
		TExtString& Prepend(char ch);
		TExtString& Prepend(int num);
		TExtString& Prepend(unsigned int num);
		TExtString& Prepend(float num);

		size_t LinesCount();
		TExtString& PrependEachLine(TExtString& s);
		TExtString& PrependEachLine(const char* s);
		TExtString& PrependEachLine(char ch);
		TExtString& PrependEachLine(int num);
		TExtString& PrependEachLine(unsigned int num);
		TExtString& PrependEachLine(long long num);
		TExtString& PrependEachLine(unsigned long long num);
		TExtString& PrependEachLine(float num);

        // odcina z końca łańcucha podaną liczbę znaków
        void TruncateAtEnd(size_t NumChars);

        bool StartsWith(const TExtString& s);
		bool StartsWithchar(char ch);
		bool StartsWithOneOfChars(const char* StringOfChars);
		bool EndsWith(const TExtString& s);
		bool EndsWithchar(char ch);
		bool EndsWithOneOfChars(const char* StringOfChars);
		bool Contains(char ch)
		{
			return find(ch) != std::string::npos;
		}
		bool Contains(const char* SubString)
		{
			return find(SubString) != std::string::npos;
		}
		bool ContainsOneOfChars(const char* StringOfChars);
		bool ContainsWhiteSpace();
		long long StrPos(const char* SubString, size_t StartFrom = 0);
		long long StrPosLast(const char* SubString, size_t StartFrom = 0);
        size_t RemoveCharsWithCodeLessThan(int CharCode);
	};


	DECLSPEC long DetectDataEncoding(unsigned char* buf, size_t buflen);
	DECLSPEC void Iso2ToAnsi(unsigned char* buf, size_t buflen);
	DECLSPEC void AnsiToIso2(unsigned char* buf, size_t buflen);
	DECLSPEC void Oem852ToAnsi(unsigned char* buf, size_t buflen);
	DECLSPEC void AnsiToOem852(unsigned char* buf, size_t buflen);
	//DECLSPEC_MZSTD VOID WINAPI UnicodeToAnsi(unsigned char *buf, unsigned long buflen);

    DECLSPEC inline bool IsCharWhiteSpace(unsigned char c)
    {
        return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
    }

    DECLSPEC inline bool IsUtf8StartByte(unsigned char c)
    {
        return (c < 0x80) || (c >= 0xc0);
    }
    DECLSPEC inline bool IsUtf8ContinuationByte(unsigned char c)
    {
        return (c >= 0x80) && (c < 0xc0);
    }

	DECLSPEC bool CharIsSentenceBoundary(wchar_t ch);

	DECLSPEC bool IsDecimal(const wchar_t* str);
	DECLSPEC bool IsDouble(const wchar_t* str);
	// if Chars is specified then it is used as character from which new string will be consisted of
	DECLSPEC TExtString GetRandomString(int DesiredLength, const char* Chars = "");


    // Below code and struct speeds up comparison a + b == c, wheere a, b, c are TExtString
    inline bool IsConcatEqual(const TExtString& a, const TExtString& b, const TExtString& c)
    {
        return a.size() + b.size() == c.size() &&
            std::equal(a.begin(), a.end(), c.begin()) &&
            std::equal(b.begin(), b.end(), c.begin() + a.size());
    }

    // Zwraca łańcuch z postacią heksadecymalną dla podanej liczby. Zwracany jest np ciąg 0x45.
    // Parametr minHexNumber określa minimalną liczbę znaków po ciągu '0x'. Jeśli jest on wiekszy
    // niż wynikowy łańcuch, to wtedy ten wynikowy łańcuch jest uzupełniany 0-ami po lewej stronie.
    // Jeśli useSmallAlpah jest równe TRUE, to dla cyfr 10..15 użyte są znaki 'a'..'f', w przeciwnym
    // razie użyte są znaki 'A'..'F'
    DECLSPEC TExtString IntToHex(uint64_t num, size_t minHexNumberLength = 2, bool useSmallAlpha = true, bool prependx0 = true);
    DECLSPEC int HexToInt(const TExtString& hex);

    /*
    struct TConcatProxy
    {
        const TExtString& a;
        const TExtString& b;
        operator TExtString() const&& { return TExtString{ a + b }; }
    };

    auto operator+(const TExtString& a, const TExtString& b)
    {
        return TConcatProxy{ a, b };
    }

    auto operator==(TConcatProxy&& concat, const TExtString& str) -> bool
    {
        return IsConcatEqual(concat.a, concat.b, str);
    }
    */
}

namespace std
{
	template<>
	struct hash<Tilc::TExtString>
	{
		std::size_t operator()(const Tilc::TExtString& s) const
		{
			return std::hash<std::string>()(s);
		}
	};
}
