#include "stdafx.h"
#include "Timer.h"
#include "MessageDispatcher.h"

CGameTimer::CGameTimer()
{
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastPerformanceCounter);
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequencyPerSec);
	m_fTimeScale = 1.0f / (double)m_nPerformanceFrequencyPerSec;

	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
	m_nPausedPerformanceCounter = 0;
	m_nStopPerformanceCounter = 0;

	m_fDynamicTimeScale = 1.0f;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
	m_fTotalTimeElasped = 0.0f;

	std::unique_ptr<UpdateDynamicTimeScaleListener> pUpdateDynamicTimeScaleListener = std::make_unique<UpdateDynamicTimeScaleListener>();
	pUpdateDynamicTimeScaleListener->SetTimer(this);
	m_pListeners.push_back(std::move(pUpdateDynamicTimeScaleListener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::SET_DYNAMIC_TIMER_SCALE, m_pListeners.back().get());
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float fLockFPS)
{
	if (m_bStopped) {
		m_fTimeElapsed = 0.0f;
		return;
	}
	float fTimeElapsed;

	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);
	fTimeElapsed = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale); // 현재 카운터에서 마지막 카운터를 뺴고 스케일 값을 곱하여 프레임간 시간 차를 구함.

	if (fLockFPS > 0.0f) {
		while (fTimeElapsed < (1.0f) / fLockFPS) // 프레임 레이트가 fLockFPS 값만큼 될 수 있도록 루프를 돌며 대기한다.
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);
			fTimeElapsed = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);
		}
	}

	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f) { // 유효한 정지시간에 대하여 정지 시간들의 샘플을 업데이트
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) ++m_nSampleCount;
	}

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f) { // 현재 프레임 레이트를 계산
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; ++i)
		m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount; // 정시 시간 샘플들의 평균으로 최종적인 정지 시간값을 결정

	m_fTimeElapsed *= m_fDynamicTimeScale;
	m_fTotalTimeElasped += m_fTimeElapsed;
}

void CGameTimer::Start()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);
	if (m_bStopped) {
		m_nPausedPerformanceCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		m_bStopped = false;
	}
}

void CGameTimer::Stop()
{
	if (!m_bStopped) {
		::QueryPerformanceCounter((LARGE_INTEGER*)m_nStopPerformanceCounter);
		m_bStopped = true;
	}
}

void CGameTimer::Reset()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nBasePerformanceCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;
	m_bStopped = false;
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	if (lpszString) {
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return m_nCurrentFrameRate;
}

float CGameTimer::GetFrameTimeElapsed()
{
	return m_fTimeElapsed;
}

float CGameTimer::GetTotalTime()
{
	return m_fTotalTimeElasped;
}

long long CGameTimer::GetNowTimeAfter(float second)
{
	auto now = GetNowTime();
	return now + std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(second)).count();
}

long long CGameTimer::GetNowTime()
{
	auto tp = std::chrono::system_clock::now().time_since_epoch();
	auto now = std::chrono::duration_cast<std::chrono::microseconds>(tp);

	return now.count();
}

void CGameTimer::SetDynamicTimeScale(float fDynamicTimeScale, float fDuration, float fMinTimeScale)
{
	if (m_fDynamicTimeScale <= fMinTimeScale)
		return;

	m_fDynamicTimeScale *= fDynamicTimeScale;

	std::wstring timeScale{ std::to_wstring(m_fDynamicTimeScale) };
	OutputDebugString(L"SetDynamicTimeScale: ");
	OutputDebugString(timeScale.c_str());
	OutputDebugString(L"\n");

	std::thread restorTimer_thread(&CGameTimer::RestoreDynamicTimeScale, this, fDynamicTimeScale, fDuration);
	restorTimer_thread.detach();
}

void CGameTimer::RestoreDynamicTimeScale(float fDynamicTimeScale, float fDuration)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(fDuration * 1000)));

	m_fDynamicTimeScale /= fDynamicTimeScale;

	std::wstring timeScale{ std::to_wstring(m_fDynamicTimeScale) };
	OutputDebugString(L"RestoreDynamicTimeScale: ");
	OutputDebugString(timeScale.c_str());
	OutputDebugString(L"\n");
}
