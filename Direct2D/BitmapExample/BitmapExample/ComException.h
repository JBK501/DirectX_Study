#pragma once
#include <exception>


// COM ���� ����Ŭ����
class ComException : public std::exception
{
	HRESULT mResult;

public:
	ComException(HRESULT hr) : mResult(hr) {}

	virtual const char* what() const override
	{
		static char buf[512]{};

		sprintf_s(buf, 512, "Failure with HRESULT : %X", mResult);
		return buf;
	}
};


// COM ���� ���� ��ƿ��Ƽ
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw ComException(hr);
	}
}

