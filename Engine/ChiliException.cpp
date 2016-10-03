#include "ChiliException.h"
#include "DXErr.h"
#include <array>


//////////////////////////////////////////////////
//            Base Exception
ChiliException::ChiliException( const wchar_t* file,
	unsigned int line,const std::wstring& note )
	:
	note( note ),
	file( file ),
	line( line )
{}

std::wstring ChiliException::GetLocation() const
{
	return std::wstring( L"Line [" ) + std::to_wstring( line ) + L"] in " + file;
}


//////////////////////////////////////////////////
//           Graphics Exception
GraphicsException::GraphicsException( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring GraphicsException::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    ( !errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty )
		+ ( !errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
		: empty )
		+ ( !note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
		: empty )
		+ ( !location.empty() ? std::wstring( L"Location: " ) + location
		: empty );
}

std::wstring GraphicsException::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring GraphicsException::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring GraphicsException::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}