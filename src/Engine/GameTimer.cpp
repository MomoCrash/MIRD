#include "pch.h"
#include "GameTimer.h"

GameTimer::GameTimer(): timeScale(1.0f)
{
	mLastFrameTime = std::chrono::high_resolution_clock::now();
}

float GameTimer::GetDeltaTime() const
{
	return (mDeltaTime / 1000000000.0f) * timeScale;
}

float GameTimer::GetFixedDeltaTime() const
{
	return 1.0f/60.0f * timeScale;
}

void GameTimer::UpdateTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	mDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - mLastFrameTime).count();
	mLastFrameTime = currentTime;
}

float GameTimer::GetFPS()
{
	return 1.0f / GetDeltaTime();
}

void GameTimer::SetTimeScale(float scale)
{
	timeScale = scale;
}
