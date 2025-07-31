#include "pch.h"
#include "comdef.h"

char const* Exception::what() const
{
    static string msg;
    wstring wMsg = what_utf8();
    msg.assign(wMsg.begin(), wMsg.end());
    return msg.c_str();
}

std::wstring Exception::what_utf8() const
{
    wstring msg;
    msg += _functionName;
    msg += L" failed in ";
    msg += _fileName;
    msg += L", line ";
    msg += std::to_wstring(_lineNumber);
        
    msg += L"\nUser message: ";
    msg += _userMessage;
    return msg;
}

wstring DXException::what_utf8() const
{
    wstring msg = Exception::what_utf8();
    _com_error err(_errorCode);
    
    msg += L"\nError: ";
    msg += err.ErrorMessage();
    return msg;
}
