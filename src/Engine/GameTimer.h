#ifndef GAMETIMER_H
#define GAMETIMER_H
#include <chrono>

class GameTimer
{
public:
    GameTimer();

    float GetDeltaTime() const;
    float GetFixedDeltaTime() const;
    void UpdateTime();
    float GetFPS();
    void SetTimeScale(float scale);

private:
    std::chrono::high_resolution_clock::time_point mLastFrameTime;
    float timeScale;
    unsigned long long mDeltaTime = 0ULL;
};

#endif // GAMETIMER_H