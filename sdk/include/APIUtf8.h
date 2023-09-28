#ifndef __UTF8_H__
#define __UTF8_H__

#pragma once

#include <windows.h>
#include <atlconv.h>
#include <iostream>
/*
* s   = source string
* d   = conversion destination
* len = (optional) source string len, does NOT include \0
*/
static inline bool WCharToUTF8(wchar_t const *s, std::string& d, size_t len = -1)
{
    d.clear();
    if (s == NULL)
        return false;

    int required = ::WideCharToMultiByte(CP_UTF8, 0, s, (int)len, NULL, 0, NULL, NULL);

    if (required == 0)
    {
        if (len != 0)
            return false;

        return true;
    }

    char* b = new char[required+1];
    if (b == NULL)
        return false;

    if (::WideCharToMultiByte(CP_UTF8, 0, s, (int)len, b, (int)required, NULL, NULL) == 0)
    {
        delete [] b;
        return false;
    }

    b[required]=0;
    d.assign(b);
    delete [] b;

    return true;
}

static inline bool WCharToUTF8(wchar_t const *s, char* d, size_t dlen)
{
    if(s==NULL || d==NULL || dlen<=0)
    {
        if(d)
            *d = '\0';
        return false;
    }
    bool ret = (0 != ::WideCharToMultiByte(CP_UTF8, 0, s, -1, d, (int)dlen, NULL, NULL));
    d[dlen-1]='\0';
    return ret; 
}
/*
* s   = source string
* d   = conversion destination
* len = (optional) source string len, does NOT include \0
*/
static inline bool UTF8ToWChar(const char* s, std::wstring& d, size_t len = -1)
{
    d.clear();

    if (s == NULL)
        return false;

    int required = ::MultiByteToWideChar(CP_UTF8, 0, s, (int)len, NULL, 0);

    if (required == 0)
    {
        if (len != 0)
            return false;

        d.clear();
        return true;
    }

    wchar_t* b = new wchar_t[required+1];
    if (b == NULL)
        return false;

    if (::MultiByteToWideChar(CP_UTF8, 0, s, (int)len, b, required) == 0)
    {
        delete [] b;
        return false;
    }

    b[required]=0;
    d.assign(b);
    delete [] b;

    return true;
}

static inline bool UTF8ToWChar(const char* s, wchar_t* d, size_t dlen)
{
    if(s==NULL || d==NULL || dlen<=0)
    {
        if(d)
            *d = L'\0';
        return false;
    }
    bool ret = (0!=::MultiByteToWideChar(CP_UTF8, 0, s, -1, d, (int)dlen));
    d[dlen-1]=L'\0';
    return ret; 
}



static inline std::wstring UTF8ToWChar(const std::string &s)
{
    std::wstring res;
    UTF8ToWChar(s.c_str(), res);
    return res;
}


static inline std::string WCharToUTF8(const std::wstring &s)
{
    std::string res;
    WCharToUTF8(s.c_str(), res);
    return res;
}

class CWCharToUTF8
{
    friend static std::ostream& operator << (std::ostream &strm, CWCharToUTF8 &Converter);
    friend static std::wostream& operator << (std::wostream &strm, CWCharToUTF8 &Converter);

public:
    explicit CWCharToUTF8(const WCHAR *pszWideCharString)
    {
        if(pszWideCharString)
            m_tsrtTcharString = pszWideCharString;
    }
    explicit CWCharToUTF8(const std::wstring &wcsWideCharString)
    {
        m_tsrtTcharString = wcsWideCharString;
    }
    explicit CWCharToUTF8(std::wstring &wcsWideCharString)
    {
        m_tsrtTcharString = wcsWideCharString;
    }
    operator const char*()
    {
        Initialize();
        return m_strUTF8String.c_str();
    }
    operator std::string()
    {
        Initialize();
        return m_strUTF8String;
    }
    const char* c_str()
    {
        Initialize();
        return m_strUTF8String.c_str();
    }
private:
    void Initialize()
    {
        WCharToUTF8(m_tsrtTcharString.c_str(), m_strUTF8String);        	
    }

private:
    std::wstring m_tsrtTcharString;
    std::string  m_strUTF8String;
};
static inline std::ostream& operator << (std::ostream &strm, CWCharToUTF8 &Converter)
{
    Converter.Initialize();
    return (strm << Converter.m_strUTF8String.c_str());
}

static inline std::wostream& operator << (std::wostream &strm, CWCharToUTF8 &Converter)
{
    Converter.Initialize();
    return (strm << Converter.m_strUTF8String.c_str());
}

class CUTF8ToWChar
{
public:
    explicit CUTF8ToWChar(const char* pszUTF8String)
    {
        if(pszUTF8String)
            m_strUTF8String= pszUTF8String;
    }    
    explicit CUTF8ToWChar(char * pszUTF8String)
    {
        if(pszUTF8String)
            m_strUTF8String= pszUTF8String;
    }
    explicit CUTF8ToWChar(const std::string & strUTF8String)
    {
        m_strUTF8String= strUTF8String;
    }
    explicit CUTF8ToWChar(std::string & strUTF8String)
    {
        m_strUTF8String= strUTF8String;
    }
    operator const WCHAR*()
    {
        Initialize();
        return m_tstrWCharString.c_str();
    }
    operator WCHAR*()
    {
        Initialize();
        return (WCHAR*)m_tstrWCharString.c_str();
    }
    const WCHAR* c_str()
    {
        Initialize();
        return m_tstrWCharString.c_str();
    }

private:
    void Initialize()
    {
        UTF8ToWChar(m_strUTF8String.c_str(), m_tstrWCharString);
    }

private:
    std::string m_strUTF8String;
    std::wstring m_tstrWCharString;
};
#endif
