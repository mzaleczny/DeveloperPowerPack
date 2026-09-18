#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <cstddef>
#include <regex>

namespace Tilc
{
	using namespace std::string_literals;
	template <typename CharT>
	using tstring = std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

	// Use: std::cout << std::setw(30) << std::left	<< email << " : " << (IsValidEmailFormat(email) ? "valid" : "invalid") << '\n';
	template <typename CharT>
	bool IsValidEmailFormat(tstring<CharT> const & email)
	{
		// std::regex_constants::icase - extra argument that means that we want case insensitive match
		auto rx = std::regex{ R"(^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$)"s, std::regex_constants::icase };
		return std::regex_match(email, rx);
	}

	// Use: auto[valid, localpart, hostname, dnslabel] = IsValidEmailFormatWithResult(email);
	template <typename CharT>
	inline std::tuple<bool, tstring<CharT>, tstring<CharT>, tstring<CharT>> IsValidEmailFormatWithResult(tstring<CharT> const & email)
	{
		auto rx = std::regex{ R"(^([A-Z0-9._%+-]+)@([A-Z0-9.-]+)\.([A-Z]{2,})$)"s, std::regex_constants::icase };
		auto result = std::smatch{};
		auto success = std::regex_match(email, result, rx);

		return std::make_tuple(
		   success,
		   success ? result[1].str() : ""s,
		   success ? result[2].str() : ""s,
		   success ? result[3].str() : ""s);
	}
}
