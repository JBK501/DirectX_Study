#pragma once

namespace MyUtil
{
	class Timer
	{
	public:
		Timer();

	public:
		void Start();	// 프로그램 시작할 때 호출
		void Stop();	// 시간 계산을 멈출 때 호출
		void Resume();	// 시간 계산을 계속할 때 호출
		void Update();	// 매 프레임마다 호출

		float TotalTime() const;	// 총 경과 시간 
		float DeltaTime() const;	// 에전 프레임가 현재 프레임 차이
		void SetScale(float scale);	// 델타 시간의 비율 지정

	private:
		double mdSecondsPerCount;	// 시간 점검 기능이 작동하는 시간 간격
		double mdDeltaTime;	// 현재 호출 시간과 이전 호출 시간의 차이
		float mfScale;	// 시간 비율

		long long mllBaseTime;	// 게임이 처음 구동 되었을 때 시간
		long long mllPausedTime;	// 멈춰 있었던 시간	
		long long mllStopTime;	// 멈춘 시간
		long long mllPrevTime;	// 이전 시간	
		long long mllCurrTime;	// 현재 시간

		bool mbStopped;	// 타이머가 중지되었는지에 대한 플래그
	};
}


