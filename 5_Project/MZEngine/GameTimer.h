#pragma once

class GameTimer
{
public:
	GameTimer();

	float DeltaTime() const;

	void Reset();
	void Tick();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_prevTime;
	__int64 m_currTime;
};

