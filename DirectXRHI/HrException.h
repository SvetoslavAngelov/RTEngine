#pragma once

/*
    From Microsoft's DXSampleHelper.h
    Source: https://github.com/Microsoft/DirectX-Graphics-Samples
*/

#include <Windows.h>
#include <stdexcept>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

class HrException : public std::runtime_error
{
    inline std::string HrToString(HRESULT hr)
    {
        char s_str[64] = {};
        sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
        return std::string(s_str);
    }
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
{
    if (FAILED(hr))
    {
        OutputDebugString(msg);
        throw HrException(hr);
    }
}

inline void ThrowIfFalse(bool value)
{
    ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* msg)
{
    ThrowIfFailed(value ? S_OK : E_FAIL, msg);
}