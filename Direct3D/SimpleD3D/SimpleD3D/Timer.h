#pragma once

namespace MyUtil
{
	class Timer
	{
	public:
		Timer();

	public:
		void Start();	// ���α׷� ������ �� ȣ��
		void Stop();	// �ð� ����� ���� �� ȣ��
		void Resume();	// �ð� ����� ����� �� ȣ��
		void Update();	// �� �����Ӹ��� ȣ��

		float TotalTime() const;	// �� ��� �ð� 
		float DeltaTime() const;	// ���� �����Ӱ� ���� ������ ����
		void SetScale(float scale);	// ��Ÿ �ð��� ���� ����

	private:
		double mdSecondsPerCount;	// �ð� ���� ����� �۵��ϴ� �ð� ����
		double mdDeltaTime;	// ���� ȣ�� �ð��� ���� ȣ�� �ð��� ����
		float mfScale;	// �ð� ����

		long long mllBaseTime;	// ������ ó�� ���� �Ǿ��� �� �ð�
		long long mllPausedTime;	// ���� �־��� �ð�	
		long long mllStopTime;	// ���� �ð�
		long long mllPrevTime;	// ���� �ð�	
		long long mllCurrTime;	// ���� �ð�

		bool mbStopped;	// Ÿ�̸Ӱ� �����Ǿ������� ���� �÷���
	};
}


