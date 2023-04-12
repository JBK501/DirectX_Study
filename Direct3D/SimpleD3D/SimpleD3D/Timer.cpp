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

		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);	// os에서 사용하는 성능 카운터 주파수를 구해온다.
		mdSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);	// 1사이클 당 경과 시간을 구한다.(앞으로 구할 주파수에 이 값을 곱하면 원하는 시간이 나온다.)
	}
	void Timer::Start()
	{
		long long currTime;

		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);	// 성능 카운터의 현재 값을 가져온다.

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
		if (mbStopped)	// 타이머가 멈춰있으면(시간변화가 없음)
		{
			mdDeltaTime = 0.0;	// 델타 타임을 0으로 지정한다.
			return;	// 종료한다.
		}

		// 델타 타임을 초 단위로 변경해 저장한다.
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mllCurrTime = currTime;

		mdDeltaTime = (mllCurrTime - mllPrevTime) * mdSecondsPerCount;

		// 이전 시간을 저장한다.
		mllPrevTime = mllCurrTime;

		// 델타 타임이 비정상적일 경우(ex>오버플로우 발생) 0으로 맞춰춘다.
		if (mdDeltaTime < 0.0)
		{
			mdDeltaTime = 0.0;
		}
	}
	float Timer::TotalTime() const
	{
		if (mbStopped)	// 멈춰 있다면
		{
			// 멈췄던 시간과 앱이 구동된 시간의 차이출력
			return static_cast<float>(
				(mllStopTime - mllBaseTime - mllPausedTime) * mdSecondsPerCount
			);
		}
		else 
		{
			// 현재 시간과 시작 시간의 차이출력
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