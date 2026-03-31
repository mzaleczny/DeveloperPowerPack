#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/ArrayUtils.h"
#include <codecvt>
#include <algorithm>
#include <locale>
#include <ranges>

// --------------------          ą       ć       ę       ł       ń       ó       ś       ź       ż       Ą       Ć       Ę       Ł       Ń       Ó       Ś       Ź       Ż
unsigned char allowed_codes_ansi[] = { 0xb9,   0xe6,   0xea,   0xb3,   0xf1,   0xf3,   0x9c,   0x9f,   0xbf,   0xa5,   0xc6,   0xca,   0xa3,   0xd1,   0xd3,   0x8c,   0x8f,   0xaf };
unsigned char allowed_codes_iso2[] = { 0xb1,   0xe6,   0xea,   0xb3,   0xf1,   0xf3,   0xb6,   0xbc,   0xbf,   0xa1,   0xc6,   0xca,   0xa3,   0xd1,   0xd3,   0xa6,   0xac,   0xaf };
unsigned char allowed_codes_oem_852[] = { 0xa5,   0x86,   0xa9,   0x88,   0xe4,   0xa2,   0x98,   0xab,   0xbe,   0xa4,   0x8f,   0xa8,   0x9d,   0xe3,   0xe0,   0x97,   0x8d,   0xbd };
unsigned short allowed_codes_utf8[] = { 0x85c4, 0x87c4, 0x99c4, 0x82c5, 0x84c5, 0xb3c3, 0x9bc5, 0xbac5, 0xbcc5, 0x84c4, 0x86c4, 0x98c4, 0x81c5, 0x83c5, 0x93c3, 0x9ac5, 0xb9c5, 0xbbc5 };
unsigned short allowed_codes_unicode[] = { 0x0105, 0x0107, 0x0119, 0x0142, 0x0144, 0x00f3, 0x015b, 0x017a, 0x017c, 0x0104, 0x0106, 0x0118, 0x0141, 0x0143, 0x00d3, 0x015a, 0x0179, 0x017b };

const constexpr int FORMAT_PARSING_STATE_NONE = 0;
const constexpr int FORMAT_PARSING_STATE_READFLAGS = 1;
const constexpr int FORMAT_PARSING_STATE_READWIDTH = 2;
const constexpr int FORMAT_PARSING_STATE_READPRECISION = 3;
const constexpr int FORMAT_PARSING_STATE_READTYPE = 4;

const constexpr int FORMAT_DEFAULT_PRECISION = 4;

const constexpr wchar_t* WORD_BOUNDARY_CHARS = L" \t.,;?!";


int Tilc::TExtString::TExtString::LettersArray[] = {
	' ', 'A', L'Ą', 'B', 'C', L'Ć', 'D', 'E', L'Ę', 'F', 'G', 'H', 'I', 'J', 'K', 'L', L'Ł', 'M', 'N', L'Ń', 'O', L'Ó', 'P', 'Q', 'R', 'S', L'Ś', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', L'Ź', L'Ż'
};

Tilc::TExtString Tilc::TExtString::UnshiftLeft(char ToChar)
{
	Tilc::TExtString Result;
	size_t Pos = find(ToChar);
	if (Pos == std::string::npos)
	{
		Result = *this;
		*this = "";
		return Result;
	}
	Result = substr(0UL, Pos);
	*this = substr(Pos+1);
	return Result;
}

Tilc::TExtString Tilc::TExtString::UnshiftRight(char ToChar)
{
	Tilc::TExtString Result;
	size_t Pos = rfind(ToChar);
	if (Pos == std::string::npos)
	{
		Result = *this;
		*this = "";
		return Result;
	}
	Result = substr(Pos+1);
	*this = substr(0UL, Pos);
	return Result;
}

size_t Tilc::TExtString::Explode(char Explodechar, std::vector<Tilc::TExtString>& Items)
{
	char ch;
	Tilc::TExtString CurrentString = "";
	Items.clear();
	const char* String = c_str();
	for (size_t i = 0; i < length(); i++)
	{
		ch = String[i];
		if (ch == Explodechar)
		{
			Items.push_back(CurrentString);
			CurrentString = "";
		}
		else
		{
			CurrentString += ch;
		}
	}
	if (CurrentString.length() > 0)
	{
		Items.push_back(CurrentString);
	}

	return Items.size();
}

size_t Tilc::TExtString::Explode(const Tilc::TExtString& ExplodeString, std::vector<Tilc::TExtString>& Items)
{
	Tilc::TExtString CurrentString = "";
	Items.clear();
	size_t Pos = 0;
	size_t CurrentPos = 0;
	while ((Pos = find(ExplodeString.c_str(), CurrentPos)) != Tilc::TExtString::npos)
	{
		CurrentString = substr(CurrentPos, Pos - CurrentPos);
		Items.push_back(CurrentString);
		CurrentPos = Pos + ExplodeString.length();
	}
	if (CurrentPos < length())
	{
		Items.push_back(substr(CurrentPos));
	}
	return Items.size();
}

std::wstring Tilc::TExtString::ToWideString(unsigned int CodePage)
{
    /*
	// deal with trivial case of empty string
	if (empty())
	{
		return std::wstring();
	}

	// determine required length of new string
	size_t RequiredLength = ::MultiByteToWideChar(CodePage, 0, c_str(), static_cast<int>(length()), 0, 0);

	// construct new string of required length
	std::wstring Destination(RequiredLength, L'\0');

	// convert old string to new string
	::MultiByteToWideChar(CodePage, 0, c_str(), static_cast<int>(length()), &Destination[0], static_cast<int>(Destination.length()));

	// return new string ( compiler should optimize this away )
	return Destination;
    */
    return std::wstring();
}

const char* Tilc::TExtString::FromWideString(const std::wstring& Source, unsigned int CodePage)
{
    /*
	// deal with trivial case of empty string
	if (Source.empty())
	{
		assign("");
		return c_str();
	}

	// determine required length of new string
	size_t RequiredLength = ::WideCharToMultiByte(CodePage, 0, Source.c_str(), static_cast<int>(Source.length()), 0, 0, NULL, NULL);

	// construct new string of required length
	resize(RequiredLength, '\0');

	// convert old string to new string
	::WideCharToMultiByte(CodePage, 0, Source.c_str(), static_cast<int>(Source.length()), &(*this)[0], static_cast<int>(RequiredLength), NULL, NULL);
    */
	return c_str();
}

const char* Tilc::TExtString::FromWideString(const wchar_t* Source, size_t SourceLength, unsigned int CodePage)
{
    /*
	// deal with trivial case of empty string
	if (!Source || SourceLength < 1)
	{
		assign("");
		return c_str();
	}

	// determine required length of new string
	size_t RequiredLength = ::WideCharToMultiByte(CodePage, 0, Source, static_cast<int>(SourceLength), 0, 0, NULL, NULL);

	// construct new string of required length
	resize(RequiredLength, '\0');

	// convert old string to new string
	::WideCharToMultiByte(CodePage, 0, Source, static_cast<int>(SourceLength), &(*this)[0], static_cast<int>(RequiredLength), NULL, NULL);
    */
	return c_str();
}

Tilc::TExtString Tilc::TExtString::SplitIntoMultiline(int MaxcharCount)
{
	Tilc::TExtString Result;
	if (length() <= MaxcharCount)
	{
		return *this;
	}

	int LastStartLinePos = 0;
	int LastWordBreakPos = 0;
	const char* Source = c_str();
	size_t Pos = 0;
	while (Pos < length())
	{
		if (Pos - LastStartLinePos < MaxcharCount)
		{
			if (Pos > 0 && (Source[Pos] == ' ' || Source[Pos] == '\n' || Source[Pos] == '\t'))
			{
				LastWordBreakPos = static_cast<int>(Pos);
			}
			Pos++;
		}
		else {
			Result += substr(LastStartLinePos, LastWordBreakPos - LastStartLinePos) + "\n";
			Pos = LastWordBreakPos + 1;
			LastStartLinePos = static_cast<int>(Pos);
		}
	}
	Result += substr(LastStartLinePos);
	return Result;
}

const char* Tilc::TExtString::FromUtf8(const char* utf8str, const std::locale& loc)
{
	/*
	// UTF-8 to wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	std::wstring wstr = wconv.from_unsigned chars(utf8str);
	// wstring to string
	std::vector<char> buf(wstr.size());
	std::use_facet<std::ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
	assign(buf.data(), buf.size());
	*/
	return c_str();
}

const char* Tilc::TExtString::AnsiToUtf8(const char* AnsiStr)
{
	/*
	std::locale loc(".1250");
	size_t len = strlen(AnsiStr);
	if (len < 1)
	{
		return "";
	}

	std::vector<wchar_t> buf(len);
	use_facet<std::ctype<wchar_t>>(loc).widen(AnsiStr, AnsiStr + len, buf.data());

	std::wstring ws(buf.data(), buf.size());
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	assign(wconv.to_unsigned chars(ws));
	*/
	return c_str();
}

const char* Tilc::TExtString::AnsiPlcharToUtf8PlString(char plchar)
{
	switch (plchar)
	{
	case '\xa5': return UTF8_PL_A;
	case '\xc6': return UTF8_PL_C;
	case '\xca': return UTF8_PL_E;
	case '\xa3': return UTF8_PL_L;
	case '\xd1': return UTF8_PL_N;
	case '\xd3': return UTF8_PL_O;
	case '\x8c': return UTF8_PL_S;
	case '\x8f': return UTF8_PL_Z1;
	case '\xaf': return UTF8_PL_Z2;
	}
	return "";
}

const char* Tilc::TExtString::ToUppercase()
{
	std::transform(begin(), end(), begin(), ::toupper);
	return c_str();
}

const char* Tilc::TExtString::ToLowercase()
{
	std::transform(begin(), end(), begin(), ::tolower);
	return c_str();
}

const char* Tilc::TExtString::StrReplace(Tilc::TExtString What, Tilc::TExtString ReplaceWith)
{
	size_t pos1, pos2;

	pos1 = 0UL;
	while ((pos1 = find(What, pos1)) != Tilc::TExtString::npos)
	{
		pos2 = pos1 + What.length();
		if (pos2 > length())
		{
			pos2 = length();
		}
		*this = substr(0, pos1) + ReplaceWith + substr(pos2);
		pos1 += ReplaceWith.length();
	}
	return c_str();
}

bool Tilc::TExtString::IsUtf8PL()
{
	// Jeśli w tekscie nie ma zadnej litery UTF-8 PL, to zakladamy ze jest kodowany w ANSI i konwertujemy go na UTF-8
	if (find(UTF8_PL_A_SM) == Tilc::TExtString::npos && find(UTF8_PL_C_SM) == Tilc::TExtString::npos &&
		find(UTF8_PL_E_SM) == Tilc::TExtString::npos && find(UTF8_PL_L_SM) == Tilc::TExtString::npos &&
		find(UTF8_PL_N_SM) == Tilc::TExtString::npos && find(UTF8_PL_O_SM) == Tilc::TExtString::npos &&
		find(UTF8_PL_S_SM) == Tilc::TExtString::npos && find(UTF8_PL_Z1_SM) == Tilc::TExtString::npos &&
		find(UTF8_PL_Z2_SM) == Tilc::TExtString::npos &&
		find(UTF8_PL_A) == Tilc::TExtString::npos && find(UTF8_PL_C) == Tilc::TExtString::npos &&
		find(UTF8_PL_E) == Tilc::TExtString::npos && find(UTF8_PL_L) == Tilc::TExtString::npos &&
		find(UTF8_PL_N) == Tilc::TExtString::npos && find(UTF8_PL_O) == Tilc::TExtString::npos &&
		find(UTF8_PL_S) == Tilc::TExtString::npos && find(UTF8_PL_Z1) == Tilc::TExtString::npos &&
		find(UTF8_PL_Z2) == Tilc::TExtString::npos)
	{
		return false;
	}
	return true;
}


bool Tilc::TExtString::isInt() const
{
    if (empty())
    {
        return false;
    }

    bool retval = true;
    size_t i = 0;
    size_t len = this->length();
    if (i < len && (this->c_str()[i] == '-' || this->c_str()[i] == '+'))
    {
        i++;
    }
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len)
    {
        return false;
    }

    for (; i < len; i++)
    {
        if (!iswdigit(this->c_str()[i]))
        {
            retval = false;
            break;
        }
    }

    return retval;
}

bool Tilc::TExtString::isDouble() const
{
    bool retval = true;
    bool decsep_found = false;
    size_t i = 0;
    size_t len = this->length();

    if (i < len && (this->c_str()[i] == '-' || this->c_str()[i] == '+'))
    {
        ++i;
    }
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len)
    {
        return false;
    }
    if (i < len && (this->c_str()[i] == ',' || this->c_str()[i] == '.'))
    {
        i++;
        decsep_found = true;
    }

    for (; i < len; i++)
    {
        if (iswdigit(this->c_str()[i]))
        {
            continue;
        }
        if ((this->c_str()[i] == ',' || this->c_str()[i] == '.') && !decsep_found)
        {
            decsep_found = true;
            continue;
        }
        retval = false;
        break;
    }

    return retval;
}


bool Tilc::TExtString::isNumeric(bool* isInt, bool* isDouble) const
{
    bool _isInt = true;
    bool _isDouble = true;
    //BOOL retval = true;
    bool decsep_found = false;
    size_t i = 0;
    size_t len = std::strlen(c_str());
    //CLocale* locale = new CLocale();

    if (i < len && (this->c_str()[i] == '-' || this->c_str()[i] == '+'))
    {
        ++i;
    }
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len)
    {
        *isInt = false;
        *isDouble = false;
        return false;
    }
    if (i < len && (this->c_str()[i] == ',' || this->c_str()[i] == '.'))
    {
        i++;
        decsep_found = true;
        _isInt = false;
    }

    for (; i < len; i++)
    {
        if (iswdigit(this->c_str()[i]))
        {
            continue;
        }
        if ((this->c_str()[i] == ',' || this->c_str()[i] == '.') && !decsep_found)
        {
            decsep_found = true;
            _isInt = false;
            continue;
        }
        _isDouble = false;
        _isInt = false;
        break;
    }

    if (isInt)
    {
        *isInt = _isInt;
    }
    if (isDouble)
    {
        *isDouble = _isDouble;
    }

    return _isInt || _isDouble;
}


const char* Tilc::TExtString::ImplodeInts(char Implodechar, std::vector<int>& Items)
{
	*this = "";
	for (int i = 0; i < Items.size(); i++)
	{
		*this += std::to_string(Items[i]);
		if (i < Items.size() - 1)
		{
			*this += Implodechar;
		}
	}
	return c_str();
}

inline Tilc::TExtString Tilc::TExtString::WidecharToString(wchar_t Widechar, unsigned int CodePage)
{
	Tilc::TExtString Result;
	Result.FromWideString(std::wstring(1, Widechar), CodePage);
	return Result;
}

void Tilc::TExtString::RTrim(char ch)
{
	size_t len = this->length();
	if (len > 0)
	{
		size_t i = len - 1;
		while (i >= 0 && (*this)[i] == ch)
		{
			(*this)[i] = '\0';
			i--;
		}
	}
}

void Tilc::TExtString::RTrimAllChars(const char* StringOfChars)
{
	size_t len = this->length();
	if (len > 0)
	{
		size_t i = len - 1;
		while (i >= 0 && strchr(StringOfChars, (*this)[i]) != nullptr)
		{
			(*this)[i] = '\0';
			i--;
		}
	}
}

void Tilc::TExtString::LTrim(char ch)
{
	size_t start_pos = 0;
	size_t len = this->length();
	while (start_pos < len && (*this)[start_pos] == ch)
	{
		start_pos++;
	}

	if (start_pos == this->length())
	{
		(*this)[0] = '\0';
		return;
	}

	strcpy(&((*this)[0]), &((*this)[start_pos]));
}

void Tilc::TExtString::LTrimAllChars(const char* StringOfChars)
{
	size_t start_pos = 0;
	size_t len = this->length();
	while (start_pos < len && strchr(StringOfChars, (*this)[start_pos]) != nullptr)
	{
		start_pos++;
	}

	if (start_pos == this->length())
	{
		(*this)[0] = '\0';
		return;
	}

	strcpy(&((*this)[0]), &((*this)[start_pos]));
}


void Tilc::TExtString::CutAtLast(char ch)
{
	size_t pos = length() - 1;
	while (pos >= 0 && c_str()[pos] != ch)
	{
		pos--;
	}

	if (pos >= 0)
	{
		(*this)[pos] = '\0';
	}
}

void Tilc::TExtString::CutAtLastchar(const char* StringOfChars)
{
	if (!StringOfChars || StringOfChars[0] == '\0')
	{
		return;
	}

	size_t len = length();
	if (len > 0)
	{
		size_t pos = len - 1;
		char ch;
		size_t len = strlen(StringOfChars);
		bool breakLoop = false;

		while (pos >= 0)
		{
			for (size_t i = 0; i < len; ++i)
			{
				ch = StringOfChars[i];
				if (c_str()[pos] == ch)
				{
					breakLoop = true;
					break;
				}
			}

			if (breakLoop)
			{
				break;
			}

			pos--;
		}

		if (pos >= 0)
		{
			(*this)[pos] = '\0';
		}
	}
}

Tilc::TExtString Tilc::TExtString::StrAfterLast(char ch)
{
	size_t len = length();
	if (len > 0)
	{
		size_t pos = len - 1;
		while (pos >= 0 && c_str()[pos] != ch)
		{
			pos--;
		}

		if (pos == this->length() - 1)
		{
			return "";
		}

		if (pos >= 0)
		{
			return substr(pos + 1);
		}
	}

	return "";
}

Tilc::TExtString Tilc::TExtString::StrAfterLastchar(const char* StringOfChars)
{
	if (!StringOfChars || StringOfChars[0] == '\0')
	{
		return "";
	}

	size_t thislen = length();
	if (thislen > 0)
	{
		size_t pos = thislen - 1;
		size_t len = strlen(StringOfChars);
		char ch;
		bool breakLoop = false;

		while (pos >= 0)
		{
			for (size_t i = 0; i < len; i++)
			{
				ch = StringOfChars[i];
				if (c_str()[pos] == ch)
				{
					breakLoop = true;
					break;
				}
			}
			if (breakLoop)
			{
				break;
			}
			pos--;
		}

		if (pos == thislen - 1)
		{
			return "";
		}

		if (pos >= 0)
		{
			return substr(pos + 1);
		}
	}

	return "";
}


Tilc::TExtString& Tilc::TExtString::Prepend(TExtString& s)
{
	return Prepend(s.c_str());
}

Tilc::TExtString& Tilc::TExtString::Prepend(const char* s)
{
	if (s)
	{
		*this = s + *this;
	}

	return *this;
}

Tilc::TExtString& Tilc::TExtString::Prepend(char ch)
{
	*this = ch + *this;
	return *this;
}

Tilc::TExtString& Tilc::TExtString::Prepend(int num)
{
	*this = std::to_string(num) + *this;
	return *this;
}

Tilc::TExtString& Tilc::TExtString::Prepend(unsigned int num)
{
	*this = std::to_string(num) + *this;
	return *this;
}

Tilc::TExtString& Tilc::TExtString::Prepend(float num)
{
	*this = std::to_string(num) + *this;
	return *this;
}


size_t Tilc::TExtString::LinesCount()
{
	size_t count = 0;
	if (length() > 0)
	{
		count = 1;
	}

	size_t len = length();
	for (size_t i = 0; i < len; i++)
	{
		if (c_str()[i] == L'\n')
		{
			count++;
		}
	}

	return count;
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(Tilc::TExtString& s)
{
	return PrependEachLine(s.c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(const char* s)
{
	if (s)
	{
		size_t s_len = strlen(s);
		size_t this_len = length();
		if (s_len > 0)
		{
			size_t linesCount = LinesCount();
			if (linesCount == 0)
			{
				linesCount = 1;
			}

			size_t extrachars = linesCount * s_len;
			size_t new_buf_size = this_len + extrachars;
			TExtString new_buf;
			new_buf.reserve(new_buf_size);

			size_t src_i;
			size_t dst_i;

			new_buf = s;
			dst_i = s_len;
			for (src_i = 0; src_i < this_len; ++src_i)
			{
				new_buf[dst_i] = c_str()[src_i];
				dst_i++;
				if (c_str()[src_i] == '\n' && src_i + 1 < this_len)
				{
					new_buf += s;
					dst_i += s_len;
				}
			}

			*this = new_buf;
		}
	}

	return *this;
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(char ch)
{
	return PrependEachLine(std::to_string(ch).c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(int num)
{
	return PrependEachLine(std::to_string(num).c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(unsigned int num)
{
	return PrependEachLine(std::to_string(num).c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(long long num)
{
	return PrependEachLine(std::to_string(num).c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(unsigned long long num)
{
	return PrependEachLine(std::to_string(num).c_str());
}

Tilc::TExtString& Tilc::TExtString::PrependEachLine(float num)
{
	return PrependEachLine(std::to_string(num).c_str());
}

bool Tilc::TExtString::StartsWith(const Tilc::TExtString& s)
{
	return find(s) == 0;
}

bool Tilc::TExtString::StartsWithchar(char ch)
{
	return length() > 0 && c_str()[0] == ch;
}

bool Tilc::TExtString::StartsWithOneOfChars(const char* StringOfChars)
{
	if (length() == 0)
	{
		return false;
	}
	return strchr(StringOfChars, c_str()[0]) != nullptr;
}

bool Tilc::TExtString::EndsWith(const Tilc::TExtString& s)
{
	size_t slen = s.length();
	size_t len = length();
	if (len < slen)
	{
		return false;
	}
	const char* buf = c_str() + len - slen;
	return strstr(buf, s.c_str()) == buf;
}

bool Tilc::TExtString::EndsWithchar(char ch)
{
	size_t len = length();
	if (len > 0)
	{
		return c_str()[len - 1] == ch;
	}
	return false;
}

bool Tilc::TExtString::EndsWithOneOfChars(const char* StringOfChars)
{
	size_t Len = length();
	if (Len == 0)
	{
		return false;
	}
	wchar_t ch = c_str()[Len - 1];
	return strchr(StringOfChars, ch) != nullptr;
}

bool Tilc::TExtString::ContainsOneOfChars(const char* StringOfChars)
{
	size_t Len = length();
	for (size_t i = 0; i < Len; ++i)
	{
		if (strchr(StringOfChars, c_str()[i]) != nullptr)
		{
			return true;
		}
	}
	return false;
}

bool Tilc::TExtString::ContainsWhiteSpace()
{
	size_t Len = length();
	for (size_t i = 0; i < Len; ++i)
	{
		if (std::isspace(c_str()[i]))
		{
			return true;
		}
	}
	return false;
}

size_t Tilc::TExtString::RemoveCharsWithCodeLessThan(int CharCode)
{
    size_t count = 0;
    size_t len = length();
    char* buf = data();
    for (size_t i = 0; i < len; ++i)
    {
        if (buf[i] < CharCode)
        {
            for (size_t j = i + 1; j < len; ++j)
            {
                buf[j - 1] = buf[j];
            }
            --len;
            ++count;
            --i;
        }
    }
    buf[len] = '\0';

    return count;
}

long Tilc::DetectDataEncoding(unsigned char* buf, size_t buflen)
{
	if (buflen >= 3 && buf[0] == (unsigned char)0xef && buf[1] == (unsigned char)0xbb && buf[2] == (unsigned char)0xbf) {
		return ENC_UTF8_WITH_BOM;
	}
	unsigned long i;
	bool is_utf8 = true;
	bool is_unicode = true;
	bool is_ansi = true;
	bool is_iso2 = true;
	bool is_oem_852 = true;
	unsigned char b;
	unsigned short w;


	// najpierw sprawdzamy czy UTF8
	for (i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII lub znak końca linii, to pasuje do każdego kodowania
		if (b == (unsigned char)0x0a || b == (unsigned char)0x0d || (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f)) {
			continue;
		}

		// sprawdzaymy czy dopuszczalny znak utf8
		// pobieramy słowo
		w = 0;
		if (i + 1 < buflen) {
			w = buf[i + 1];
			w <<= 8;
			w |= b;
		}

		if (!InArray(allowed_codes_utf8, sizeof(allowed_codes_utf8), w))
		{
			is_utf8 = false;
			break;
		}

		i++;
	}

	if (is_utf8) {
		return ENC_UTF8;
	}


	// sprawdzamy czy UNICODE
	for (i = 0; i < buflen; i += 2) {
		b = buf[i];
		// pobieramy słowo
		w = 0;
		if (i + 1 < buflen) {
			w = buf[i + 1];
			w <<= 8;
			w |= b;
		}

		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (w >= (unsigned short)0x20 && w <= (unsigned short)0x7f) {
			continue;
		}


		if (!InArray(allowed_codes_unicode, sizeof(allowed_codes_unicode), w))
		{
			is_unicode = false;
			break;
		}
	}

	if (is_unicode) {
		return ENC_UNICODE;
	}

	// now we check if have one-unsigned char encoding such as ANSI/ISO2/OEM852
	for (i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f) {
			continue;
		}

		// sprawdzaymy czy dopuszczalny znak ansi
		if (is_ansi) {
			if (!InArray(allowed_codes_ansi, sizeof(allowed_codes_ansi), b))
			{
				is_ansi = false;
			}
		}

		// sprawdzaymy czy dopuszczalny znak ansi
		if (is_iso2) {
			if (!InArray(allowed_codes_iso2, sizeof(allowed_codes_iso2), b))
			{
				is_iso2 = false;
			}
		}

		// sprawdzaymy czy dopuszczalny znak oem_852
		if (is_oem_852) {
			if (!InArray(allowed_codes_oem_852, sizeof(allowed_codes_oem_852), b))
			{
				is_oem_852 = false;
			}
		}

		// jeśli wszystkie kodowania jednobajtowe już odpadły, to nie sprwdzamy dalej
		if (!is_ansi && !is_iso2 && !is_oem_852) {
			break;
		}
	}

	// We accept one-unsigned char encoding in order: ANSI, ISO2, OEM_852
	if (is_ansi) {
		return ENC_ANSI;
	}

	if (is_iso2) {
		return ENC_ISO2;
	}

	if (is_oem_852) {
		return ENC_OEM_852;
	}

	return 0;
}

void Tilc::Iso2ToAnsi(unsigned char* buf, size_t buflen)
{
	unsigned char b;
	for (unsigned long i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f) {
			continue;
		}

		for (unsigned long k = 0; k < sizeof(allowed_codes_iso2); k++) {
			if (b == allowed_codes_iso2[k]) {
				buf[i] = allowed_codes_ansi[k];
				break;
			}
		}
	}
}

void Tilc::AnsiToIso2(unsigned char* buf, size_t buflen)
{
	unsigned char b;
	for (unsigned long i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f) {
			continue;
		}

		for (unsigned long k = 0; k < sizeof(allowed_codes_ansi); k++) {
			if (b == allowed_codes_ansi[k]) {
				buf[i] = allowed_codes_iso2[k];
				break;
			}
		}
	}
}

void Tilc::Oem852ToAnsi(unsigned char* buf, size_t buflen)
{
	unsigned char b;
	for (unsigned long i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f) {
			continue;
		}

		for (unsigned long k = 0; k < sizeof(allowed_codes_oem_852); k++) {
			if (b == allowed_codes_oem_852[k]) {
				buf[i] = allowed_codes_ansi[k];
				break;
			}
		}
	}
}

void Tilc::AnsiToOem852(unsigned char* buf, size_t buflen)
{
	unsigned char b;
	for (unsigned long i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (unsigned char)0x20 && b <= (unsigned char)0x7f) {
			continue;
		}

		for (unsigned long k = 0; k < sizeof(allowed_codes_ansi); k++) {
			if (b == allowed_codes_ansi[k]) {
				buf[i] = allowed_codes_oem_852[k];
				break;
			}
		}
	}
}

bool Tilc::CharIsSentenceBoundary(wchar_t ch) {
	if (wcschr(WORD_BOUNDARY_CHARS, ch) != nullptr) {
		return true;
	}

	return false;
}

bool Tilc::IsDecimal(const wchar_t* str) {
	if (!str) return false;

	size_t len = wcslen(str);
	if (len < 1) return false;

	for (size_t i = 0; i < len; i++) {
		if (i == 0 && str[i] == L'-') {
			continue;
		}

		if (!iswdigit(str[i])) {
			return false;
		}
	}

	return true;
}

bool Tilc::IsDouble(const wchar_t* str) {
	if (!str) return false;

	size_t len = wcslen(str);
	bool hasDot = false;
	wchar_t ch;

	if (len < 1) return false;

	for (size_t i = 0; i < len; i++) {
		ch = str[i];

		if (i == 0 && ch == L'-') {
			continue;
		}

		if (!hasDot && (ch == L'.' || ch == L',')) {
			hasDot = true;
			continue;
		}

		if (!iswdigit(ch)) {
			return false;
		}
	}

	return true;
}

Tilc::TExtString Tilc::GetRandomString(int DesiredLength, const char* Chars)
{
    /*
    auto d = std::uniform_int_distribution<>{ 0, static_cast<int>(strlen(Chars)) - 1 };
    auto random_char = [&] { return Chars[d(RandomGenerator)]; };
    auto s = std::string(DesiredLength, '\0');
    std::ranges::generate(s, random_char); // Requires C++20
    return s;
    */
	if (strlen(Chars) == 0)
	{
    	Chars = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM!@#$%^&*()-=_+[]{};:\'\"\\|,<.>/?";
	}
    size_t Len = strlen(Chars);
    Tilc::TExtString Result;
    for (int i = 0; i < DesiredLength; ++i)
    {
        Result.push_back(Chars[rand() % Len]);
    }
    return Result;
}
