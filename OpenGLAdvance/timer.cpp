#include "timer.h"

Timer::Timer()
{
	mCurrentThread = GetCurrentThread();
	DWORD_PTR systemAffitinityMask;
	GetProcessAffinityMask(GetCurrentProcess(), &mProcessAffinityMask, &systemAffitinityMask);
	SetThreadAffinityMask(mCurrentThread, 1);
	QueryPerformanceFrequency(&mCPUFrequency);
	SetThreadAffinityMask(mCurrentThread, mProcessAffinityMask);
}

void Timer::Start()
{
	SetThreadAffinityMask(mCurrentThread, 1);
	QueryPerformanceCounter(&mStart);
	SetThreadAffinityMask(mCurrentThread, mProcessAffinityMask);
}

int Timer::GetPassedTicks()
{
	SetThreadAffinityMask(mCurrentThread, 1);
	QueryPerformanceCounter(&mEnd);
	SetThreadAffinityMask(mCurrentThread, mProcessAffinityMask);
	return mEnd.QuadPart - mStart.QuadPart;
}

float Timer::GetPassedTime() //ms
{
	return static_cast<float>(GetPassedTicks()) / static_cast<float>(mCPUFrequency.QuadPart) * 1000;
}