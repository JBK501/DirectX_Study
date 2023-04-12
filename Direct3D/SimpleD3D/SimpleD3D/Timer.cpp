#include <Windows.h>
#include "Timer.h"

namespace MyUtil
{
	Timer::Timer() :
		mdSecondsPerCount{ 0.0f },
		mdDeltaTime{ -1.0f },
		mfScale{ 1.0f },
		mllBaseTime{},
		mllPausedTime{},
		mllStopTime{},
		mllPrevTime{},
		mllCurrTime{},
		mbStopped{ false }
	{
		long long countsPerSec;

		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);	// os���� ����ϴ� ���� ī���� ���ļ��� ���ؿ´�.
		mdSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);	// 1����Ŭ �� ��� �ð��� ���Ѵ�.(������ ���� ���ļ��� �� ���� ���ϸ� ���ϴ� �ð��� ���´�.)
	}
	void Timer::Start()
	{
		long long currTime;

		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);	// ���� ī������ ���� ���� �����´�.

		mllBaseTime = currTime;
		mllPrevTime = currTime;
		mllStopTime = 0;
		mbStopped = false;
	}
	void Timer::Stop()
	{
		if (!mbStopped)
		{
			long long currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			mllStopTime = currTime;
			mbStopped = true;
		}
	}
	void Timer::Resume()
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		if (mbStopped)
		{
			mllPausedTime += (currTime - mllStopTime);

			mllPrevTime = currTime;
			mllStopTime = 0;
			mbStopped = false;
		}
	}
	void Timer::Update()
	{
		if (mbStopped)	// Ÿ�̸Ӱ� ����������(�ð���ȭ�� ����)
		{
			mdDeltaTime = 0.0;	// ��Ÿ Ÿ���� 0���� �����Ѵ�.
			return;	// �����Ѵ�.
		}

		// ��Ÿ Ÿ���� �� ������ ������ �����Ѵ�.
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mllCurrTime = currTime;

		mdDeltaTime = (mllCurrTime - mllPrevTime) * mdSecondsPerCount;

		// ���� �ð��� �����Ѵ�.
		mllPrevTime = mllCurrTime;

		// ��Ÿ Ÿ���� ���������� ���(ex>�����÷ο� �߻�) 0���� �������.
		if (mdDeltaTime < 0.0)
		{
			mdDeltaTime = 0.0;
		}
	}
	float Timer::TotalTime() const
	{
		if (mbStopped)	// ���� �ִٸ�
		{
			// ����� �ð��� ���� ������ �ð��� �������
			return static_cast<float>(
				(mllStopTime - mllBaseTime - mllPausedTime) * mdSecondsPerCount
			);
		}
		else 
		{
			// ���� �ð��� ���� �ð��� �������
			return static_cast<float>(
				(mllCurrTime - mllBaseTime - mllPausedTime) * mdSecondsPerCount
			);
		}
	}
	float Timer::DeltaTime() const
	{
		return static_cast<float>(mdDeltaTime * mfScale);
	}
	void Timer::SetScale(float scale)
	{
		mfScale = scale;
	}
}