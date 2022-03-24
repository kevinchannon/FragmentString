#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <string_view>
#include <numeric>

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
size_t string_length(const Char_T* psz)
{
	if constexpr (std::is_same_v<Char_T, char>) {
		return std::strlen(psz);
	}
	else if constexpr (std::is_same_v<Char_T, wchar_t>) {
		return std::wcslen(psz);
	}
	else {
		static_assert(std::bool_constant<std::is_same_v<Char_T, char>>::value, "Invalid char type");
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t FRAGMENT_COUNT = 1>
class BasicFragmentString
{
public:
	using Char_t = Char_T;
	using Str_t = const Char_t*; //std::basic_string_view<Char_t>;
	using Array_t = std::array<Str_t, FRAGMENT_COUNT>;
	using This_t = BasicFragmentString<Char_t, FRAGMENT_COUNT>;

	BasicFragmentString( Str_t str )
		: m_fragments{ str }
	{
	}

	template<size_t STR_LEN>
	BasicFragmentString(const Char_t psz[STR_LEN])
		: m_fragments(psz)
	{
	}

	BasicFragmentString( const This_t& ) = default;
	BasicFragmentString( This_t&& ) = default;
	This_t& operator=( const This_t& ) = default;
	This_t& operator=( This_t&& ) = default;

	template<size_t LEFT_FRAG_COUNT, size_t RIGHT_FRAG_COUNT>
	BasicFragmentString( const BasicFragmentString<Char_t, LEFT_FRAG_COUNT>& left, const BasicFragmentString<Char_t, RIGHT_FRAG_COUNT>& right )
	{
		const auto it = std::copy( left.Fragments().cbegin(), left.Fragments().cend(), m_fragments.begin() );
		std::copy( right.Fragments().cbegin(), right.Fragments().cend(), it );
	}

	operator std::basic_string<Char_t>() const
	{
		auto out = std::basic_string<Char_t>{};
		out.reserve(length());
		out = std::accumulate(m_fragments.cbegin(), m_fragments.cend(), std::move(out), [](auto&& curr, auto&& next) {
			return curr += next;
			});
		return out;
	}

	const Array_t& Fragments() const { return m_fragments; }

	size_t length() const
	{
		return std::accumulate(m_fragments.begin(), m_fragments.end(), size_t{}, [](auto&& len, auto&& fragment) {
			return len += string_length(fragment);
			});
	}

private:
	Array_t m_fragments;
};

///////////////////////////////////////////////////////////////////////////////

using FragmentString = BasicFragmentString<char>;
using FragmentWString = BasicFragmentString<wchar_t>;

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t LEFT_FRAG_COUNT, size_t STR_LEN>
BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1> operator+(const BasicFragmentString<Char_T, LEFT_FRAG_COUNT>& left, const Char_T right[STR_LEN])
{
	return BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1>(left, BasicFragmentString<Char_T>(right));
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t LEFT_FRAG_COUNT>
BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1> operator+(const BasicFragmentString<Char_T, LEFT_FRAG_COUNT>& left, const Char_T* right)
{
	return BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1>(left, BasicFragmentString<Char_T>(right));
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t LEFT_FRAG_COUNT, size_t RIGHT_FRAG_COUNT>
BasicFragmentString<Char_T, LEFT_FRAG_COUNT + RIGHT_FRAG_COUNT> operator+( const BasicFragmentString<Char_T, LEFT_FRAG_COUNT>& left, const BasicFragmentString<Char_T, RIGHT_FRAG_COUNT>& right )
{
	return BasicFragmentString<Char_T, LEFT_FRAG_COUNT + RIGHT_FRAG_COUNT>( left, right );
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t FRAGMENT_COUNT>
std::basic_ostream<Char_T>& operator<<( std::basic_ostream<Char_T>& os, const BasicFragmentString<Char_T, FRAGMENT_COUNT>& str )
{
	for ( const auto& fragment : str.Fragments() )
		os << fragment;

	return os;
}

///////////////////////////////////////////////////////////////////////////////

const char* g_str = " and now a global part too!";

///////////////////////////////////////////////////////////////////////////////

const char* GetString() { return "FOO!"; }

///////////////////////////////////////////////////////////////////////////////

const char* DangerString( const std::string& str )
{
	static auto local_str = std::string( str );
	return local_str.c_str();
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
	auto str = FragmentString( "This is a test string" );
	std::cout << str << std::endl;

	auto str2 = str + " with an added part";
	std::cout << str2 << std::endl;

	auto str3 = str2 + g_str;
	std::cout << str3 << std::endl;

	auto str4 = FragmentString( "The string function returns: " ) + GetString();
	std::cout << str4 << std::endl;

	const auto s = std::string(str4);
	std::cout << "As a std::string? " << s << std::endl;

	auto str5 = FragmentString( "What about this danger? " );
	auto s1 = std::string( "BOO!" );
	auto dangerStr = str5 + DangerString( s1 );

	std::cout << dangerStr << std::endl;

	auto wstr = FragmentWString( L"... and now a wiiiide string" );
	std::wcout << wstr << std::endl;

	auto wstr2 = wstr + L" with some more text";
	std::wcout << wstr2 << std::endl;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
