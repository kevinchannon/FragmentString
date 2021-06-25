#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t FRAGMENT_COUNT = 1>
class BasicFragmentString
{
public:
	using Array_T = std::array<const Char_T*, FRAGMENT_COUNT>;

	BasicFragmentString( const Char_T* psz )
		: m_fragments{ psz }
	{
	}

	BasicFragmentString( const BasicFragmentString<Char_T, FRAGMENT_COUNT>& ) = default;
	BasicFragmentString( BasicFragmentString<Char_T, FRAGMENT_COUNT>&& ) = default;
	BasicFragmentString<Char_T, FRAGMENT_COUNT>& operator=( const BasicFragmentString<Char_T, FRAGMENT_COUNT>& ) = default;
	BasicFragmentString<Char_T, FRAGMENT_COUNT>& operator=( BasicFragmentString<Char_T, FRAGMENT_COUNT>&& ) = default;

	template<size_t STR_LEN>
	BasicFragmentString( const Char_T psz[STR_LEN] )
		: m_fragments( psz )
	{
	}

	template<size_t LEFT_FRAG_COUNT, size_t RIGHT_FRAG_COUNT>
	BasicFragmentString( const BasicFragmentString<Char_T, LEFT_FRAG_COUNT>& left, const BasicFragmentString<Char_T, RIGHT_FRAG_COUNT>& right )
	{
		std::copy( left.Fragments().cbegin(), left.Fragments().cend(), m_fragments.begin() );
		std::copy( right.Fragments().cbegin(), right.Fragments().cend(), std::next( m_fragments.begin(), left.Fragments().size() ) );
	}

	operator std::basic_string<Char_T>() const
	{
		std::stringstream ss;
		std::copy( m_fragments.cbegin(), m_fragments.cend(), std::ostream_iterator<const Char_T*, Char_T>( ss ) );
		return ss.str();
	}

	const std::array<const Char_T*, FRAGMENT_COUNT>& Fragments() const { return m_fragments; }

private:
	std::array<const Char_T*, FRAGMENT_COUNT > m_fragments;
};

///////////////////////////////////////////////////////////////////////////////

using FragmentString = BasicFragmentString<char>;
using FragmentWString = BasicFragmentString<wchar_t>;

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T, size_t LEFT_FRAG_COUNT>
BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1> operator+( const BasicFragmentString<Char_T, LEFT_FRAG_COUNT>& left, const Char_T* right )
{
	return BasicFragmentString<Char_T, LEFT_FRAG_COUNT + 1>( left, BasicFragmentString<Char_T>( right ) );
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
	for ( const Char_T* fragment : str.Fragments() )
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

	auto str5 = FragmentString( "What about this danger? " );
	auto s = std::string( "BOO!" );
	auto dangerStr = str5 + DangerString( s );

	std::cout << dangerStr << std::endl;

	auto wstr = FragmentWString( L"... and now a wiiiide string" );
	std::wcout << wstr << std::endl;

	auto wstr2 = wstr + L" with some more text";
	std::wcout << wstr2 << std::endl;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
