#pragma once
class TimeManager
{
private:
	TimeManager() {};
	~TimeManager() {};

public:
	static TimeManager* GetInstance()
	{
		static TimeManager inst;
		return &inst;
	}

	void SetDeltaTime(float p_deltaTime);

	float GetDeltaTime() { return m_deltaTime; }
	float GetAccTIme() { return m_accTIme; }

private:
	float m_deltaTime = 0.f;
	float m_accTIme = 0.f;
};

