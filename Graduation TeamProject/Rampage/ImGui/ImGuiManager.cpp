#include "ImGuiManager.h"
#include "ImGuiConstants.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Scene\SimulatorScene.h"
#include "..\Object\Texture.h"
#include "..\Global\GameFramework.h"
#include "..\Global\Locator.h"
#include "..\Sound\SoundManager.h"
#include "..\Object\TextureManager.h"
#include "implot.h"
#include "implot_internal.h"

#define NUM_FRAMES_IN_FLIGHT 3
#define MAX_FILENAME_SIZE 100
#define U8STR(str) reinterpret_cast<const char*>(u8##str)

bool CreateDirectoryIfNotExists(const std::wstring& path) {
	DWORD attr = GetFileAttributesW(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES) {
		if (!CreateDirectoryW(path.c_str(), NULL)) {
			return false;
		}
	}
	else if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
		return false;
	}
	return true;
}

std::wstring ConvertU8ToW(const std::u8string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str.c_str()), static_cast<int>(str.size()), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str.c_str()), static_cast<int>(str.size()), &wstr[0], size_needed);
	return wstr;
}

//========================================================================
void DataLoader::SaveComponentSets(std::wstring wFolderName)
{
	FILE* pInFile;

	for (int i = 0; i < 3; ++i)
	{
		CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();

		// State 선택 로직 생각 필요
		switch (i) {
		case 0:
			pCurrentAnimation = Atk1_Player::GetInst();
			break;
		case 1:
			pCurrentAnimation = Atk2_Player::GetInst();
			break;
		case 2:
			pCurrentAnimation = Atk3_Player::GetInst();
			break;
		default:
			break;
		}

		std::wstring path;
		wFolderName.resize(wcslen(wFolderName.c_str()));

		path = file_path + wFolderName + L"\\Component" + std::to_wstring(i) + file_ext;

		std::wstring fp = file_path + wFolderName;
		if (!CreateDirectoryIfNotExists(fp)) {
			return;
		}

		::_wfopen_s(&pInFile, path.c_str(), L"wb");
		SaveComponentSet(pInFile, pCurrentAnimation);
		fclose(pInFile);
	}
}

void DataLoader::LoadComponentSets(std::wstring wFolderName)
{
	FILE* pInFile;

	for (int i = 0; i < 3; ++i)
	{
		CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();

		// State 선택 로직 생각 필요
		switch (i) {
		case 0:
			pCurrentAnimation = Atk1_Player::GetInst();
			break;
		case 1:
			pCurrentAnimation = Atk2_Player::GetInst();
			break;
		case 2:
			pCurrentAnimation = Atk3_Player::GetInst();
			break;
		default:
			break;
		}

		std::wstring path;
		wFolderName.resize(wcslen(wFolderName.c_str()));

		path = file_path + wFolderName + L"\\Component" + std::to_wstring(i) + file_ext;

		::_wfopen_s(&pInFile, path.c_str(), L"rb");
		LoadComponentSet(pInFile, pCurrentAnimation);
		fclose(pInFile);
	}
}

void DataLoader::SaveComponentSet(FILE* pInFile, CState<CPlayer>* pState)
{
	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pState->GetCameraMoveComponent());
	CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pState->GetCameraShakeComponent());
	CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pState->GetCameraZoomerComponent());

	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pState->GetDamageAnimationComponent());
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pState->GetShakeAnimationComponent());
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pState->GetStunAnimationComponent());

	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShockSoundComponent());
	SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShootSoundComponent());

	SoundPlayComponent* pGoblinMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetGoblinMoanComponent());
	SoundPlayComponent* pOrcMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetOrcMoanComponent());
	SoundPlayComponent* pSkeletonMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetSkeletonMoanComponent());

	ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(pState->GetParticleComponent());
	ImpactEffectComponent* pImpactComponent = dynamic_cast<ImpactEffectComponent*>(pState->GetImpactComponent());

	TrailComponent* pTrailComponent = dynamic_cast<TrailComponent*>(pState->GetTrailComponent());

	std::string str = "<Components>:";
	WriteStringFromFile(pInFile, str);

	WriteStringFromFile(pInFile, std::string("<CCameraMover>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraMoveComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetMovingTime());
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetRollBackTime());
	WriteStringFromFile(pInFile, std::string("</CCameraMover>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraShaker>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraShakerComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Duration>:"));
	WriteFloatFromFile(pInFile, pCameraShakerComponent->GetDuration());
	WriteStringFromFile(pInFile, std::string("<Magnitude>:"));
	WriteFloatFromFile(pInFile, pCameraShakerComponent->GetMagnitude());
	WriteStringFromFile(pInFile, std::string("<Frequency>:"));
	WriteFloatFromFile(pInFile, pCameraShakerComponent->GetFrequency());
	WriteStringFromFile(pInFile, std::string("</CCameraShaker>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraZoomer>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraZoomerComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetMovingTime());
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetRollBackTime());
	WriteStringFromFile(pInFile, std::string("<IsZoomIN>:"));
	WriteIntegerFromFile(pInFile, pCameraZoomerComponent->GetIsIn());
	WriteStringFromFile(pInFile, std::string("</CCameraZoomer>:"));

	WriteStringFromFile(pInFile, std::string("<DamageAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pDamageAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("</DamageAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<ShakeAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShakeAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Duration>:"));
	WriteFloatFromFile(pInFile, pShakeAnimationComponent->GetDuration());
	WriteStringFromFile(pInFile, std::string("<Distance>:"));
	WriteFloatFromFile(pInFile, pShakeAnimationComponent->GetDistance());
	WriteStringFromFile(pInFile, std::string("<Frequency>:"));
	WriteFloatFromFile(pInFile, pShakeAnimationComponent->GetFrequency());
	WriteStringFromFile(pInFile, std::string("</ShakeAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<StunAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pStunAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<StunTime>:"));
	WriteFloatFromFile(pInFile, pStunAnimationComponent->GetStunTime());
	WriteStringFromFile(pInFile, std::string("</StunAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CEffectSoundComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShockSoundComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pShockSoundComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pShockSoundComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pShockSoundComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</CEffectSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CShootSoundComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShootSoundComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pShootSoundComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pShootSoundComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pShootSoundComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</CShootSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<GoblinMoanComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pGoblinMoanComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pGoblinMoanComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pGoblinMoanComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pGoblinMoanComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</GoblinMoanComponent>:"));

	WriteStringFromFile(pInFile, std::string("<OrcMoanComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pOrcMoanComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pOrcMoanComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pOrcMoanComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pOrcMoanComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</OrcMoanComponent>:"));

	WriteStringFromFile(pInFile, std::string("<SkeletonMoanComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pSkeletonMoanComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pSkeletonMoanComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pSkeletonMoanComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pSkeletonMoanComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</SkeletonMoanComponent>:"));

	WriteStringFromFile(pInFile, std::string("<ParticleComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pParticleComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<ParticleEmitNumber>:"));
	WriteIntegerFromFile(pInFile, pParticleComponent->GetEmitParticleNumber());
	WriteStringFromFile(pInFile, std::string("<ParticleIndex>:"));
	WriteIntegerFromFile(pInFile, pParticleComponent->GetTextureIndex());
	WriteStringFromFile(pInFile, std::string("<Alpha>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetAlpha());
	WriteStringFromFile(pInFile, std::string("<LifeTime>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetLifeTime());
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("<Size_X>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetSize().x);
	WriteStringFromFile(pInFile, std::string("<Size_Y>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetSize().y);
	WriteStringFromFile(pInFile, std::string("<Color_R>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetColor().x);
	WriteStringFromFile(pInFile, std::string("<Color_G>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetColor().y);
	WriteStringFromFile(pInFile, std::string("<Color_B>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetColor().z);
	WriteStringFromFile(pInFile, std::string("</ParticleComponent>:"));

	WriteStringFromFile(pInFile, std::string("<ImpactComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pImpactComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<TextureIndex>:"));
	WriteIntegerFromFile(pInFile, pImpactComponent->GetTextureIndex());
	WriteStringFromFile(pInFile, std::string("<Size_X>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetSize().x);
	WriteStringFromFile(pInFile, std::string("<Size_Y>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetSize().y);
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("<Alpha>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetAlpha());
	WriteStringFromFile(pInFile, std::string("<Color_R>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetColor().x);
	WriteStringFromFile(pInFile, std::string("<Color_G>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetColor().y);
	WriteStringFromFile(pInFile, std::string("<Color_B>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetColor().z);
	WriteStringFromFile(pInFile, std::string("</ImpactComponent>:"));

	WriteStringFromFile(pInFile, std::string("<TrailComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pTrailComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<EmissiveFactor>:"));
	WriteFloatFromFile(pInFile, pTrailComponent->m_fEmissiveFactor);
	WriteStringFromFile(pInFile, std::string("<nCurves>:"));
	WriteIntegerFromFile(pInFile, pTrailComponent->m_nCurves);
	WriteStringFromFile(pInFile, std::string("<Rcurves>:"));
	for (int i = 0; i < MAX_COLORCURVES; ++i)
		WriteFloatFromFile(pInFile, pTrailComponent->m_fR_CurvePoints[i]);
	WriteStringFromFile(pInFile, std::string("<Gcurves>:"));
	for (int i = 0; i < MAX_COLORCURVES; ++i)
		WriteFloatFromFile(pInFile, pTrailComponent->m_fG_CurvePoints[i]);
	WriteStringFromFile(pInFile, std::string("<Bcurves>:"));
	for (int i = 0; i < MAX_COLORCURVES; ++i)
		WriteFloatFromFile(pInFile, pTrailComponent->m_fB_CurvePoints[i]);
	WriteStringFromFile(pInFile, std::string("<Timecurves>:"));
	for (int i = 0; i < MAX_COLORCURVES; ++i)
		WriteFloatFromFile(pInFile, pTrailComponent->m_fColorCurveTimes_R[i]);
	WriteStringFromFile(pInFile, std::string("</TrailComponent>:"));

	str = "</Components>:";
	WriteStringFromFile(pInFile, str);
}

void DataLoader::LoadComponentSet(FILE* pInFile, CState<CPlayer>* pState)
{
	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pState->GetCameraMoveComponent());
	CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pState->GetCameraShakeComponent());
	CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pState->GetCameraZoomerComponent());

	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pState->GetDamageAnimationComponent());
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pState->GetShakeAnimationComponent());
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pState->GetStunAnimationComponent());

	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShockSoundComponent());
	SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShootSoundComponent());

	SoundPlayComponent* pGoblinMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetGoblinMoanComponent());
	SoundPlayComponent* pOrcMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetOrcMoanComponent());
	SoundPlayComponent* pSkeletonMoanComponent = dynamic_cast<SoundPlayComponent*>(pState->GetSkeletonMoanComponent());

	ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(pState->GetParticleComponent());
	ImpactEffectComponent* pImpactComponent = dynamic_cast<ImpactEffectComponent*>(pState->GetImpactComponent());

	TrailComponent* pTrailComponent = dynamic_cast<TrailComponent*>(pState->GetTrailComponent());

	char buf[256];
	std::string str;
	str.resize(256);
	ReadStringFromFile(pInFile, buf);

	for (; ; )
	{
		ReadStringFromFile(pInFile, buf);

		if (!strcmp(buf, "<CCameraMover>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					pCameraMoveComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					pCameraMoveComponent->SetMovingTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					pCameraMoveComponent->SetRollBackTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraMoveComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraMover>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraShaker>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Duration>:"))
				{
					pCameraShakerComponent->SetDuration(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Magnitude>:"))
				{
					pCameraShakerComponent->SetMagnitude(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraShakerComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Frequency>:"))
				{
					pCameraShakerComponent->SetFrequency(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraShaker>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraZoomer>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					pCameraZoomerComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					pCameraZoomerComponent->SetMovingTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					pCameraZoomerComponent->SetRollBackTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<IsZoomIN>:"))
				{
					pCameraZoomerComponent->SetIsIn(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraZoomerComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraZoomer>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<DamageAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pDamageAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MaxDistance>:"))
				{
					pDamageAnimationComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Speed>:"))
				{
					pDamageAnimationComponent->SetSpeed(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</DamageAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<ShakeAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShakeAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Duration>:"))
				{
					pShakeAnimationComponent->SetDuration(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Distance>:"))
				{
					pShakeAnimationComponent->SetDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Frequency>:"))
				{
					pShakeAnimationComponent->SetFrequency(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</ShakeAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<StunAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pStunAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<StunTime>:"))
				{
					pStunAnimationComponent->SetStunTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</StunAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CEffectSoundComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShockSoundComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pShockSoundComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pShockSoundComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pShockSoundComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CEffectSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CShootSoundComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShootSoundComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pShootSoundComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pShootSoundComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pShootSoundComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CShootSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<GoblinMoanComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pGoblinMoanComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pGoblinMoanComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pGoblinMoanComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pGoblinMoanComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</GoblinMoanComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<OrcMoanComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pOrcMoanComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pOrcMoanComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pOrcMoanComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pOrcMoanComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</OrcMoanComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<SkeletonMoanComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pSkeletonMoanComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pSkeletonMoanComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pSkeletonMoanComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pSkeletonMoanComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</SkeletonMoanComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<ParticleComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pParticleComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<ParticleEmitNumber>:"))
				{
					pParticleComponent->SetEmitParticleNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<ParticleIndex>:"))
				{
					pParticleComponent->SetTextureIndex(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Alpha>:"))
				{
					pParticleComponent->SetAlpha(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<LifeTime>:"))
				{
					pParticleComponent->SetLifeTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Speed>:"))
				{
					pParticleComponent->SetSpeed(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Size_X>:"))
				{
					pParticleComponent->SetSizeX(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Size_Y>:"))
				{
					pParticleComponent->SetSizeY(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_R>:"))
				{
					pParticleComponent->SetColorR(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_G>:"))
				{
					pParticleComponent->SetColorG(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_B>:"))
				{
					pParticleComponent->SetColorB(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</ParticleComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<ImpactComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pImpactComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<TextureIndex>:"))
				{
					pImpactComponent->SetTextureIndex(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Size_X>:"))
				{
					pImpactComponent->SetSizeX(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Size_Y>:"))
				{
					pImpactComponent->SetSizeY(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Speed>:"))
				{
					pImpactComponent->SetSpeed(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Alpha>:"))
				{
					pImpactComponent->SetAlpha(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_R>:"))
				{
					pImpactComponent->SetColorR(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_G>:"))
				{
					pImpactComponent->SetColorG(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Color_B>:"))
				{
					pImpactComponent->SetColorB(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</ImpactComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<TrailComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pTrailComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<EmissiveFactor>:"))
				{
					pTrailComponent->m_fEmissiveFactor = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<nCurves>:"))
				{
					pTrailComponent->m_nCurves = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Rcurves>:"))
				{
					for(int i = 0;i<MAX_COLORCURVES;++i)
						pTrailComponent->m_fR_CurvePoints[i] = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Gcurves>:"))
				{
					for (int i = 0; i < MAX_COLORCURVES; ++i)
						pTrailComponent->m_fG_CurvePoints[i] = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Bcurves>:"))
				{
					for (int i = 0; i < MAX_COLORCURVES; ++i)
						pTrailComponent->m_fB_CurvePoints[i] = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Timecurves>:"))
				{
					for (int i = 0; i < MAX_COLORCURVES; ++i)
						pTrailComponent->m_fColorCurveTimes_R[i] = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "</TrailComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "</Components>:"))
		{
			break;
		}
	}
}
//========================================================================

wchar_t* ConverCtoWC(const char* str)

{

	//wchar_t형 변수 선언

	wchar_t* pStr;

	//멀티 바이트 크기 계산 길이 반환

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	//wchar_t 메모리 할당

	pStr = new WCHAR[strSize];

	//형 변환

	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;

}


CImGuiManager::CImGuiManager()
{
}
CImGuiManager::~CImGuiManager()
{
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void CImGuiManager::CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = 10;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dSrvDescHeap.GetAddressOf());

	m_d3dSrvCPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dSrvGPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();

	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
}
void CImGuiManager::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}
void CImGuiManager::SetPreviewTexture(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT textureIndex, UINT index, PREVIEW_TEXTURE_TYPE type)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle; d3dSrvCPUDescriptorHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * index);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle; d3dSrvGPUDescriptorHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * index);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();

	ID3D12Resource* pShaderResource = pTexture->GetResource(textureIndex);
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(textureIndex);
	pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
	d3dSrvCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	switch (type)
	{
	case PREVIEW_TEXTURE_TYPE::TYPE_IMPACT: // impact
		m_d3dSrvGPUDescriptorHandle_ImpactTexture = d3dSrvGPUDescriptorHandle;
		break;
	case PREVIEW_TEXTURE_TYPE::TYPE_PARTICLE: // particle
		m_d3dSrvGPUDescriptorHandle_ParticleTexture = d3dSrvGPUDescriptorHandle;
		break;
	case PREVIEW_TEXTURE_TYPE::TYPE_TRAILBASE: // trailbase
		m_d3dSrvGPUDescriptorHandle_TrailMainTexture = d3dSrvGPUDescriptorHandle;
		break;
	case PREVIEW_TEXTURE_TYPE::TYPE_TRAILNOISE: // trailnoise
		m_d3dSrvGPUDescriptorHandle_TrailNoiseTexture = d3dSrvGPUDescriptorHandle;
		break;
	default:
		break;
	}
	d3dSrvGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	
}
void CImGuiManager::SetShaderResourceViews(CTexture* pTexture, UINT index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle; d3dSrvCPUDescriptorHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * index);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle; d3dSrvGPUDescriptorHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * index);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		d3dSrvCPUDescriptorHandle.ptr = pTexture->m_pd3dSrvCpuDescriptorHandles[i].ptr;
		d3dSrvGPUDescriptorHandle.ptr = pTexture->m_pd3dSrvGpuDescriptorHandles[i].ptr;
	}
}

ID3D12Resource* CImGuiManager::GetRTTextureResource() { return m_pRTTexture->GetResource(0); }
void CImGuiManager::Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, const RECT& DeskTopCoordinatesRect)
{
	m_hWnd = hWnd;

	CreateSrvDescriptorHeaps(pd3dDevice);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescHeap.Get(),
		m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 1.0f, 1.0f, 1.0f, 1.0f } };
	m_pRTTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	m_pRTTexture->CreateTexture(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue, RESOURCE_TEXTURE2D, 0, 1);
	CreateShaderResourceViews(pd3dDevice, m_pRTTexture.get(), 1);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	ID3D12Resource* pd3dTextureResource = m_pRTTexture->GetResource(0);
	pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
	m_pd3dRtvCPUDescriptorHandles = d3dRtvCPUDescriptorHandle;

	m_pDataLoader = std::make_unique<DataLoader>();

	m_lDesktopWidth = DeskTopCoordinatesRect.right - DeskTopCoordinatesRect.left;
	m_lDesktopHeight = DeskTopCoordinatesRect.bottom - DeskTopCoordinatesRect.top;
}
void CImGuiManager::DemoRendering()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	//2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}
void CImGuiManager::SetUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();

	// State 선택 로직 생각 필요
	switch (Player_Animation_Number) {
	case 0:
		pCurrentAnimation = Atk1_Player::GetInst();
		break;
	case 1:
		pCurrentAnimation = Atk2_Player::GetInst();
		break;
	case 2:
		pCurrentAnimation = Atk3_Player::GetInst();
		break;
	default:
		break;
	}

	dearImGuiSize = ImGui::GetIO().DisplaySize;

	if (show_simulator_scene)
	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin(U8STR("시뮬레이터"), &show_simulator_scene, my_window_flags);

		int my_image_width = 0.55f * m_lDesktopWidth;
		int my_image_height = 0.55f * m_lDesktopHeight;
		ImGui::Image((ImTextureID)m_pRTTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr, ImVec2((float)my_image_width, (float)my_image_height));

		ImGui::End();
	}

	if (show_preset_menu)
	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin(U8STR("타격감 프리셋 메뉴"), &show_preset_menu, my_window_flags);

		std::string path = "Data";
		std::vector<std::u8string> v;
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			v.emplace_back(entry.path().u8string().substr(entry.path().u8string().find_last_of(u8"/\\") + 1));
		}

		static int idx = 0;
		if (ImGui::BeginListBox(U8STR("프리셋 리스트")))
		{
			for (int n = 0; n < v.size(); n++)
			{
				const bool is_selected = (idx == n);
				if (ImGui::Selectable(reinterpret_cast<const char*>(v[n].c_str()), is_selected))
					idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();

		if (v.size() > 0)
		{
			std::u8string first_string = u8"선택된 폴더: ";
			first_string.append(v[idx]);
			ImGui::Text(reinterpret_cast<const char*>(first_string.c_str()));
			ImGui::SameLine(m_lDesktopWidth * 0.14f + ImGui::CalcTextSize(U8STR("불러오기")).x);
			if (ImGui::Button(U8STR("불러오기")))
			{
				m_pDataLoader->LoadComponentSets(ConvertU8ToW(v[idx]));
				show_preset_menu = false;
			}
		}
		else
		{
			std::u8string first_string = u8"Data 폴더에 프리셋이 없습니다. ";
			ImGui::Text(reinterpret_cast<const char*>(first_string.c_str()));
			ImGui::SameLine(m_lDesktopWidth * 0.14f + ImGui::CalcTextSize(U8STR("불러오기")).x);
			if (ImGui::Button(U8STR("불러오기")))
			{
				m_pDataLoader->LoadComponentSets(ConvertU8ToW(v[idx]));
				show_preset_menu = false;
			}
		}
		ImGui::End();
	}

	if (show_save_menu)
	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin(U8STR("저장 메뉴"), &show_save_menu, my_window_flags);

		static std::u8string folder_name;
		folder_name.resize(MAX_PATH);

		ImGui::InputText(U8STR("저장 이름"), reinterpret_cast<char*>(folder_name.data()), 64);

		ImVec2 itemSize = ImGui::GetItemRectSize();

		if (ImGui::Button(U8STR("저장"), itemSize))
		{
			m_pDataLoader->SaveComponentSets(ConvertU8ToW(folder_name));
			show_save_menu = false;
		}

		ImGui::End();
	}

	// Show my window.
	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar;
		bool* p_open = NULL;

		ImGui::SetNextWindowSize(ImVec2(m_lDesktopWidth * 0.25f, 0.0f), ImGuiCond_Always);
		ImGui::Begin(U8STR("시뮬레이터 관리자"), p_open, my_window_flags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(U8STR("메뉴")))
			{
				if (ImGui::MenuItem(U8STR("저장"), NULL))
				{
					show_save_menu = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem(U8STR("타격감 프리셋 메뉴"), NULL))
				{
					show_preset_menu = true;
				}
				if (ImGui::MenuItem(U8STR("몬스터 리셋"), NULL))
				{
					CSimulatorScene::GetInst()->ResetMonster();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// 시뮬레이터 씬 보여주기 여부 설정 ImGui
		ImGui::Checkbox(U8STR("시뮬레이터"), &show_simulator_scene);

		ImGui::SameLine(m_lDesktopWidth * 0.17f);
		// 플레이어 애니메이션 출력 버튼
		if (ImGui::Button(U8STR("공격1")))
		{
			if (show_simulator_scene)
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(0);
			Player_Animation_Number = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button(U8STR("공격2")))
		{
			if (show_simulator_scene)
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(1);
			Player_Animation_Number = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button(U8STR("공격3")))
		{
			if (show_simulator_scene)
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(2);
			Player_Animation_Number = 2;
		}

		if (ImGui::CollapsingHeader(U8STR("특수 효과")))
		{
			if (ImGui::TreeNode(U8STR("충격 이펙트")))
			{
				ShowImpactManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("파티클 이펙트")))
			{
				ShowParticleManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("잔상 이펙트")))
			{
				ShowTrailManager(pCurrentAnimation);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader(U8STR("애니메이션")))
		{
			if (ImGui::TreeNode(U8STR("대미지 애니메이션")))
			{
				ShowDamageAnimationManager(pCurrentAnimation);

				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("흔들림 애니메이션")))
			{
				ShowShakeAnimationManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("경직 애니메이션")))
			{
				ShowStunAnimationManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("역경직 애니메이션")))
			{
				ShowHitLagManager(pCurrentAnimation);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader(U8STR("카메라 테크닉")))
		{
			if (ImGui::TreeNode(U8STR("카메라 이동")))
			{
				ShowCameraMoveManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("카메라 흔들림")))
			{
				ShowCameraShakeManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("카메라 줌인")))
			{
				ShowCameraZoomManager(pCurrentAnimation);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader(U8STR("음향")))
		{
			if (ImGui::TreeNode(U8STR("충격 효과음")))
			{
				ShowShockSoundManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("발사 효과음")))
			{
				ShowShootSoundManager(pCurrentAnimation);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(U8STR("대미지 음성")))
			{
				ShowDamageMoanSoundManager(pCurrentAnimation);
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar;
		bool* p_open = NULL;

		ImGui::SetNextWindowSize(ImVec2(m_lDesktopWidth * 0.6f, 0.0f), ImGuiCond_Always);
		ImGui::Begin(U8STR("창작 마당"), p_open, my_window_flags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(U8STR("메뉴")))
			{
				if (ImGui::MenuItem(U8STR("새로고침"), NULL))
				{
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		{
			ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.66f, 0.15f * m_lDesktopWidth), false);
		
			ImGui::Text(U8STR("정렬기준: ")); ImGui::SameLine();

			const char* allignStyle[] = { U8STR("ID"), U8STR("이름"), U8STR("좋아요"), U8STR("싫어요") };
			static int selectedAllignStyle = 0;
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo(U8STR("##allignStyle"), &selectedAllignStyle, allignStyle, IM_ARRAYSIZE(allignStyle));

			// Sample 1
			ImGui::Columns(7, "Creation Column", false);
			ImGui::Separator();
			ImGui::Text(U8STR("이미지")); ImGui::NextColumn();
			ImGui::Text(U8STR("ID")); ImGui::NextColumn();
			ImGui::Text(U8STR("이름")); ImGui::NextColumn();
			ImGui::Text(U8STR("좋아요")); ImGui::NextColumn();
			ImGui::Text(U8STR("싫어요")); ImGui::NextColumn();
			ImGui::Text(U8STR("신고")); ImGui::NextColumn();
			ImGui::Text(U8STR("불러오기")); ImGui::NextColumn();
			ImGui::Separator();

			struct CreationItem {
				CTexture* preview;
				std::u8string id;
				std::u8string name;
				std::u8string nGoodSign;
				std::u8string nBadSign;
			};

			std::vector<CreationItem> vCreationItems;
			vCreationItems.push_back(CreationItem{ m_pRTTexture.get(), u8"창작자1", u8"프리셋1", u8"123", u8"321" });
			vCreationItems.push_back(CreationItem{ m_pRTTexture.get(), u8"창작자2", u8"프리셋2", u8"456", u8"654" });
			vCreationItems.push_back(CreationItem{ m_pRTTexture.get(), u8"창작자3", u8"프리셋3", u8"789", u8"987" });

			for (int i = 0; i < 3; i++)
			{
				int my_image_width = 0.05f * m_lDesktopWidth;
				int my_image_height = 0.05f * m_lDesktopHeight;
				ImGui::Image((ImTextureID)m_pRTTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr, ImVec2((float)my_image_width, (float)my_image_height));
				ImGui::NextColumn();
				ImGui::Text(reinterpret_cast<const char*>(vCreationItems[i].id.c_str())); ImGui::NextColumn();
				ImGui::Text(reinterpret_cast<const char*>(vCreationItems[i].name.c_str())); ImGui::NextColumn();
				ImGui::Text(reinterpret_cast<const char*>(vCreationItems[i].nGoodSign.c_str())); ImGui::NextColumn();
				ImGui::Text(reinterpret_cast<const char*>(vCreationItems[i].nBadSign.c_str())); ImGui::NextColumn();
				ImGui::Button(U8STR("신고하기"), ImVec2(-FLT_MIN, 0.0f)); ImGui::NextColumn();
				ImGui::Button(U8STR("불러오기"), ImVec2(-FLT_MIN, 0.0f)); ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 originalColor = style.Colors[ImGuiCol_Button];
			style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
			ImGui::Button(U8STR("1")); ImGui::SameLine();
			ImGui::Button(U8STR("2")); ImGui::SameLine();
			ImGui::Button(U8STR("3")); ImGui::SameLine();
			ImGui::Button(U8STR("4")); ImGui::SameLine();
			ImGui::Button(U8STR("5")); ImGui::SameLine();
			style.Colors[ImGuiCol_Button] = originalColor;

			ImGui::EndChild();
		}

		ImGui::SameLine(0.0f, 0.01f * m_lDesktopWidth);

		{
			static std::u8string searchText;
			searchText.resize(256);

			char keyword[256] = {};
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("ChildR", ImVec2(0, 0.15f * m_lDesktopWidth), true);

			const char* items[] = { U8STR("ID"), U8STR("이름") };

			static int selectedSearchStyle = 0;
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Combo(U8STR("##searchStyle"), &selectedSearchStyle, items, IM_ARRAYSIZE(items));
			
			ImGui::InputText(U8STR("##keywordText"), reinterpret_cast<char*>(searchText.data()), 256);
			ImGui::SameLine();
			if (ImGui::Button(U8STR("검색##keywordSearch"), ImVec2(-FLT_MIN, 0.0f)))
				OutputDebugString(ConvertU8ToW(searchText).c_str());

			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
		ImGui::End();
	}
}
void CImGuiManager::ShowImpactManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("충격 이펙트 관리자"), b_open, my_window_flags);
	ImpactEffectComponent* pImpactEffectComponent = dynamic_cast<ImpactEffectComponent*>(pCurrentAnimation->GetImpactComponent());
	ImGui::Checkbox(U8STR("켜기/끄기##ImpactEffect"), &pImpactEffectComponent->GetEnable());

	pImpactEffectComponent->SetTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::BillBoardTexture));
	UINT m_iBillboardTextureN = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureListIndex(TextureType::BillBoardTexture);
	
	std::shared_ptr<CTexture> pTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetTexture(TextureType::BillBoardTexture);
	std::vector<const char*> items;
	std::vector <std::string> str(100);
	for (int i = 0; i < m_iBillboardTextureN; i++)
	{
		std::wstring wstr = pTexture->GetTextureName(i);
		str[i].assign(wstr.begin(), wstr.end());
		items.emplace_back(str[i].c_str());
	}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::Combo(U8STR("텍스쳐##ImpactEffect"), (int*)(&pImpactEffectComponent->GetTextureIndex()), items.data(), items.size()));
	{
		int iTextureIndex = pImpactEffectComponent->GetTextureIndex();
		pImpactEffectComponent->SetTotalRowColumn(pTexture->GetRow(iTextureIndex), pTexture->GetColumn(iTextureIndex));

		int my_image_width = 0.2f * m_lDesktopHeight;
		int my_image_height = 0.2f * m_lDesktopHeight;
		SetPreviewTexture(Locator.GetDevice(), pTexture.get(), pImpactEffectComponent->GetTextureIndex(), 2, PREVIEW_TEXTURE_TYPE::TYPE_IMPACT);
		ImGui::Image((ImTextureID)(m_d3dSrvGPUDescriptorHandle_ImpactTexture.ptr), ImVec2((float)my_image_width, (float)my_image_height));
	}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("수명##ImpactEffect"), &pImpactEffectComponent->GetLifetime(), DRAG_FLOAT_UNIT, IMPACT_LIFETIME_MIN, IMPACT_LIFETIME_MAX, "%.2f", 0))
		pImpactEffectComponent->GetLifetime() = std::clamp(pImpactEffectComponent->GetLifetime(), IMPACT_LIFETIME_MIN, IMPACT_LIFETIME_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("투명도##ImpactEffect"), &pImpactEffectComponent->GetAlpha(), DRAG_FLOAT_UNIT, IMPACT_ALPHA_MIN, IMPACT_ALPHA_MAX, "%.2f", 0))
		pImpactEffectComponent->GetAlpha() = std::clamp(pImpactEffectComponent->GetAlpha(), IMPACT_ALPHA_MIN, IMPACT_ALPHA_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("X 크기##ImpactEffect"), &pImpactEffectComponent->GetXSize(), DRAG_FLOAT_UNIT, IMPACT_SIZE_MIN, IMPACT_SIZE_MAX, "%.2f", 0))
		pImpactEffectComponent->GetXSize() = std::clamp(pImpactEffectComponent->GetXSize(), IMPACT_SIZE_MIN, IMPACT_SIZE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("Y 크기##ImpactEffect"), &pImpactEffectComponent->GetYSize(), DRAG_FLOAT_UNIT, IMPACT_SIZE_MIN, IMPACT_SIZE_MAX, "%.2f", 0))
		pImpactEffectComponent->GetYSize() = std::clamp(pImpactEffectComponent->GetYSize(), IMPACT_SIZE_MIN, IMPACT_SIZE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::ColorEdit3(U8STR("색상##ImpactEffect"), (float*)&pImpactEffectComponent->GetColor()); // Edit 3 floats representing a color
	ImGui::End();
}
void CImGuiManager::ShowParticleManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("파티클 이펙트 관리자"), b_open, my_window_flags);

	ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(pCurrentAnimation->GetParticleComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##ParticleEffect"), &pParticleComponent->GetEnable());

	UINT m_iParticleTextureN = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureListIndex(TextureType::ParticleTexture);
	pParticleComponent->SetTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
	std::shared_ptr<CTexture> pTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetTexture(TextureType::ParticleTexture);
	std::vector<const char*> items;
	std::vector <std::string> str(100);
	for (int i = 0; i < m_iParticleTextureN; i++)
	{
		std::wstring wstr = pTexture->GetTextureName(i);
		str[i].assign(wstr.begin(), wstr.end());
		items.emplace_back(str[i].c_str());
	}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if(ImGui::Combo(U8STR("텍스쳐##ParticleEffect"), (int*)(&pParticleComponent->GetTextureIndex()), items.data(), items.size()))

	int my_image_width = 0.2f * m_lDesktopHeight;
	int my_image_height = 0.2f * m_lDesktopHeight;


	SetPreviewTexture(Locator.GetDevice(), pTexture.get(), pParticleComponent->GetTextureIndex(), 3, PREVIEW_TEXTURE_TYPE::TYPE_PARTICLE);
	ImGui::Image((ImTextureID)(m_d3dSrvGPUDescriptorHandle_ParticleTexture.ptr), ImVec2((float)my_image_height, (float)my_image_height));

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("XSize##ParticleEffect", &pParticleComponent->GetXSize(), DRAG_FLOAT_UNIT, PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX, "%.2f", 0))
		pParticleComponent->GetXSize() = std::clamp(pParticleComponent->GetXSize(), PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX);

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("YSize##ParticleEffect", &pParticleComponent->GetYSize(), DRAG_FLOAT_UNIT, PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX, "%.2f", 0))
		pParticleComponent->GetYSize() = std::clamp(pParticleComponent->GetYSize(), PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("투명도##ParticleEffect"), &pParticleComponent->GetAlpha(), DRAG_FLOAT_UNIT, PARTICLE_ALPHA_MIN, PARTICLE_ALPHA_MAX, "%.2f", 0))
		pParticleComponent->GetAlpha() = std::clamp(pParticleComponent->GetAlpha(), PARTICLE_ALPHA_MIN, PARTICLE_ALPHA_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("수명##ParticleEffect"), &pParticleComponent->GetLifeTime(), DRAG_FLOAT_UNIT, PARTICLE_LIFETIME_MIN, PARTICLE_LIFETIME_MAX, "%.1f", 0))
		pParticleComponent->GetLifeTime() = std::clamp(pParticleComponent->GetLifeTime(), PARTICLE_LIFETIME_MIN, PARTICLE_LIFETIME_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragInt(U8STR("파티클 방출 개수##ParticleEffect"), &pParticleComponent->GetEmitParticleNumber(), DRAG_FLOAT_UNIT, PARTICLE_COUNT_MIN, PARTICLE_COUNT_MAX, "%d", 0))
		pParticleComponent->GetEmitParticleNumber() = std::clamp(pParticleComponent->GetEmitParticleNumber(), PARTICLE_COUNT_MIN, PARTICLE_COUNT_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("속도##ParticleEffect"), &pParticleComponent->GetSpeed(), DRAG_FLOAT_UNIT, PARTICLE_SPEED_MIN, PARTICLE_SPEED_MAX, "%.1f", 0))
		pParticleComponent->GetSpeed() = std::clamp(pParticleComponent->GetSpeed(), PARTICLE_SPEED_MIN, PARTICLE_SPEED_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::ColorEdit3(U8STR("색상"), (float*)&pParticleComponent->GetColor()); // Edit 3 floats representing a color

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("FieldSpeed##ParticleEffect", &pParticleComponent->GetFieldSpeed(), DRAG_FLOAT_UNIT, 0.0f, FLT_MAX, "%.2f", 0))
		pParticleComponent->GetFieldSpeed() = std::clamp(pParticleComponent->GetFieldSpeed(), FLT_MIN, FLT_MAX);

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("NoiseStrength##ParticleEffect", &pParticleComponent->GetNoiseStrength(), DRAG_FLOAT_UNIT, 0.0f, FLT_MAX, "%.2f", 0))
		pParticleComponent->GetNoiseStrength() = std::clamp(pParticleComponent->GetNoiseStrength(), FLT_MIN, FLT_MAX);

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("ProgressionRate##ParticleEffect", &pParticleComponent->GetProgressionRate(), DRAG_FLOAT_UNIT, 0.0f, FLT_MAX, "%.2f", 0))
		pParticleComponent->GetProgressionRate() = std::clamp(pParticleComponent->GetProgressionRate(), FLT_MIN, FLT_MAX);

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("LengthScale##ParticleEffect", &pParticleComponent->GetLengthScale(), DRAG_FLOAT_UNIT, 0.0f, FLT_MAX, "%.2f", 0))
		pParticleComponent->GetLengthScale() = std::clamp(pParticleComponent->GetLengthScale(), FLT_MIN, FLT_MAX);

	ImGui::SetNextItemWidth(190.f);
	float* DirectionData = (float*)(&pParticleComponent->GetFieldMainDirection());
	ImGui::InputFloat3("FieldMainDirection##ParticleEffect", DirectionData, "%.2f", 0);

	ImGui::End();
}
void CImGuiManager::ShowTrailManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("잔상 이펙트 관리자"), b_open, my_window_flags);

	TrailComponent* pTrailComponent = dynamic_cast<TrailComponent*>(pCurrentAnimation->GetTrailComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##TrailEffect"), &pTrailComponent->GetEnable());


	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

	UINT m_iParticleTextureN = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureListIndex(TextureType::TrailBaseTexture);
	pTrailComponent->SetMainTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::TrailBaseTexture));
	std::shared_ptr<CTexture> pTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetTexture(TextureType::TrailBaseTexture);
	std::vector<const char*> items;
	std::vector <std::string> str(100);
	for (int i = 0; i < m_iParticleTextureN; i++)
	{
		std::wstring wstr = pTexture->GetTextureName(i);
		str[i].assign(wstr.begin(), wstr.end());
		items.emplace_back(str[i].c_str());
	}

	int mainTextureIndexCache = pTrailComponent->GetMainTextureIndex();
	if (ImGui::Combo(U8STR("메인 텍스쳐##TrailEffect"), (int*)(&pTrailComponent->GetMainTextureIndex()), items.data(), items.size()))
	{
		/*std::shared_ptr pTexture = vTexture[pParticleComponent->GetParticleIndex()];
		pParticleComponent->SetParticleTexture(pTexture);*/
		// 메인 텍스쳐 로드
		/*TrailTextureUpdateParams param;
		param.pShader = */
	}

	int my_image_width = 0.2f * m_lDesktopHeight;
	int my_image_height = 0.2f * m_lDesktopHeight;


	SetPreviewTexture(Locator.GetDevice(), pTexture.get(), pTrailComponent->GetMainTextureIndex(), 4, PREVIEW_TEXTURE_TYPE::TYPE_TRAILBASE);
	ImGui::Image((ImTextureID)(m_d3dSrvGPUDescriptorHandle_TrailMainTexture.ptr), ImVec2((float)my_image_height, (float)my_image_height));

	ImGui::DragFloat("Emissve##TrailEffect", &pTrailComponent->m_fEmissiveFactor);

	ImGui::BulletText("Click and drag each point.");
	static ImPlotDragToolFlags R_flags;
	static ImPlotDragToolFlags G_flags;
	static ImPlotDragToolFlags B_flags;
	ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
	static bool VerticalInputFlag = false;

	static int colorSelectFlag = 3;
	ImGui::RadioButton("R##TrailEffect", &colorSelectFlag, 0); ImGui::SameLine();
	ImGui::RadioButton("G##TrailEffect", &colorSelectFlag, 1); ImGui::SameLine();
	ImGui::RadioButton("B##TrailEffect", &colorSelectFlag, 2); ImGui::SameLine();
	ImGui::RadioButton("ALL##TrailEffect", &colorSelectFlag, 3);
	switch (colorSelectFlag)
	{
	case 0:
		R_flags = ImPlotDragToolFlags_None;
		G_flags = ImPlotDragToolFlags_NoInputs;
		B_flags = ImPlotDragToolFlags_NoInputs;
		break;
	case 1:
		R_flags = ImPlotDragToolFlags_NoInputs;
		G_flags = ImPlotDragToolFlags_None;
		B_flags = ImPlotDragToolFlags_NoInputs;
		break;
	case 2:
		R_flags = ImPlotDragToolFlags_NoInputs;
		G_flags = ImPlotDragToolFlags_NoInputs;
		B_flags = ImPlotDragToolFlags_None;
		break;
	case 3:
		R_flags = ImPlotDragToolFlags_None;
		G_flags = ImPlotDragToolFlags_None;
		B_flags = ImPlotDragToolFlags_None;
		break;
	default:
		break;
	}


	int nCurveCache = pTrailComponent->m_nCurves;
	if (ImGui::InputInt(U8STR("색상 커브 개수##TrailEffect"), &pTrailComponent->m_nCurves, 1)) {
		if (pTrailComponent->m_nCurves <= 8 && pTrailComponent->m_nCurves >= 2) {
			if (pTrailComponent->m_nCurves > nCurveCache) {
				float size = 1.0f / nCurveCache;
				float delta = 1.0f + (size * (pTrailComponent->m_nCurves - nCurveCache));
				float correctionValue = 1.0f / delta;
				for (int i = 0; i < nCurveCache; ++i) {
					pTrailComponent->m_fColorCurveTimes_R[i] *= correctionValue;
				}
				for (int i = nCurveCache; i < pTrailComponent->m_nCurves; ++i) {
					pTrailComponent->m_fColorCurveTimes_R[i] = (1.0f + (size * ((i - nCurveCache) + 1))) * correctionValue;
				}
			}
			else if (pTrailComponent->m_nCurves < nCurveCache) {
				float delta = 1.0f - 1.0f / nCurveCache;
				float correctionValue = 1.0f / delta;
				for (int i = 0; i < pTrailComponent->m_nCurves; ++i) {
					pTrailComponent->m_fColorCurveTimes_R[i] *= correctionValue;
				}
			}
		}
	}
	pTrailComponent->m_nCurves = std::clamp(pTrailComponent->m_nCurves, 2, MAX_COLORCURVES);

	if (ImPlot::BeginPlot("##Bezier", ImVec2(-1, 0), ImPlotFlags_CanvasOnly)) {
		int nCurveIndexs = pTrailComponent->m_nCurves;

		//ImPlot::SetupAxesLimits(0, 1, 0, 1);

		static ImPlotPoint P[] = { ImPlotPoint(.05f,.05f), ImPlotPoint(0.2,0.4),  ImPlotPoint(0.8,0.6),  ImPlotPoint(.95f,.95f) };

		for (int i = 0; i < nCurveIndexs; ++i) {
			int id = i * 4;
			double yPointCache[3];
			double TimeControllPoint = -0.2f;

			yPointCache[0] = pTrailComponent->m_fColorCurveTimes_R[i]; yPointCache[1] = pTrailComponent->m_fColorCurveTimes_R[i]; yPointCache[2] = pTrailComponent->m_fColorCurveTimes_R[i];
			ImPlot::DragPoint(id, &yPointCache[0], &pTrailComponent->m_fR_CurvePoints[i], ImVec4(1, 0, 0, 1), 4, R_flags);
			ImPlot::DragPoint(id + 1, &yPointCache[0], &pTrailComponent->m_fG_CurvePoints[i], ImVec4(0, 1, 0, 1), 4, G_flags);
			ImPlot::DragPoint(id + 2, &yPointCache[0], &pTrailComponent->m_fB_CurvePoints[i], ImVec4(0, 0, 1, 1), 4, B_flags);
			if (VerticalInputFlag) {
				yPointCache[0] = std::clamp(yPointCache[0], (double)0.0f, (double)1.0f);
				if (i == 0 || i == nCurveIndexs - 1)
					yPointCache[0] = i == 0 ? 0.0f : 1.0f;
				pTrailComponent->m_fColorCurveTimes_R[i] = yPointCache[0];
			}
			ImPlot::DragPoint(id + 3, &pTrailComponent->m_fColorCurveTimes_R[i], &TimeControllPoint, ImVec4(1, 1, 1, 1), 4, B_flags);
			TimeControllPoint = -0.2f;
			{
				pTrailComponent->m_fColorCurveTimes_R[i] = std::clamp(pTrailComponent->m_fColorCurveTimes_R[i], (double)0.0f, (double)1.0f);
				if (i == 0 || i == nCurveIndexs - 1)
					pTrailComponent->m_fColorCurveTimes_R[i] = i == 0 ? 0.0f : 1.0f;
			}
		}

		ImPlotPoint Lines[3][100 * MAX_COLORCURVES];
		for (int i = 0; i < 3; ++i) {

			double* colorCurve = nullptr;
			double* timeCurve = nullptr;
			if (i == 0) {
				colorCurve = &pTrailComponent->m_fR_CurvePoints[0];
				timeCurve = &pTrailComponent->m_fColorCurveTimes_R[0];
			}
			else if (i == 1) {
				colorCurve = &pTrailComponent->m_fG_CurvePoints[0];
				timeCurve = &pTrailComponent->m_fColorCurveTimes_R[0];
			}
			else if (i == 2) {
				colorCurve = &pTrailComponent->m_fB_CurvePoints[0];
				timeCurve = &pTrailComponent->m_fColorCurveTimes_R[0];
			}
			if (colorCurve != nullptr && timeCurve != nullptr) {
				for (int j = 0; j < nCurveIndexs - 1; ++j) {
					ImPlotPoint distanceVector = { timeCurve[j + 1] - timeCurve[j],colorCurve[j + 1] - colorCurve[j] };
					float distance = sqrt(pow(distanceVector.x, 2) + pow(distanceVector.y, 2));
					ImPlotPoint Direct = { distanceVector.x / distance, distanceVector.y / distance };
					float dt = distance / 100.0f;

					float dt_x = Direct.x / 100;
					float dt_y = Direct.y / 100;
					for (int n = 0; n < 100; ++n) {
						Lines[i][n + 100 * j] = { timeCurve[j] + Direct.x * dt * n, colorCurve[j] + Direct.y * dt * n };
						//B[i] = { P[1].x + dt_x * i, P[1].y + dt_y * i};
					}
				}
			}

			ImVec4 LineColor;
			if (i == 0) {
				LineColor = ImVec4(0.5, 0, 0, 1);
			}
			else if (i == 1) {
				LineColor = ImVec4(0, 0.5, 0, 1);
			}
			else if (i == 2) {
				LineColor = ImVec4(0, 0, 0.5, 1);
			}
			ImPlotPoint* line = Lines[i];
			ImPlot::SetNextLineStyle(LineColor, 2);
			ImPlot::PlotLine("##bez", &line[0].x, &line[0].y, 100 * (nCurveIndexs - 1), 0, 0, sizeof(ImPlotPoint));
		}

		float testColors = { 0.0 };
		static int cmap = ImPlotColormap_RdBu;
		static ImVec4 abc = { 1.0,0.0,0.0,1.0f };
		static int interpolationCount = 30;
		std::vector<ImU32> colors; colors.resize((MAX_COLORCURVES-1)* interpolationCount);

		auto uncharted2_tonemap_partial = [](XMFLOAT3 x)
		{
			float A = 0.15f;
			float B = 0.50f;
			float C = 0.10f;
			float D = 0.20f;
			float E = 0.02f;
			float F = 0.30f;
			XMVECTOR a1 = (XMVectorMultiply(XMLoadFloat3(&Vector3::Add(Vector3::ScalarProduct(x, A, false), XMFLOAT3(B * C, B * C, B * C))), XMLoadFloat3(&x)) + XMLoadFloat3(&XMFLOAT3(D * E, D * E, D * E)));
			XMVECTOR a2 = (XMVectorMultiply(XMLoadFloat3(&(Vector3::Add(Vector3::ScalarProduct(x, A, false), XMFLOAT3(B, B, B)))), XMLoadFloat3(&x)) + XMVectorMultiply(XMLoadFloat3(&XMFLOAT3(D, D, D)), XMLoadFloat3(&XMFLOAT3(F, F, F))));
			XMVECTOR a3 = -XMVECTOR({ E,E,E }) / XMVECTOR({ F,F,F });
			XMVECTOR b1 = a1 / a2;
			XMVECTOR c1 = b1 + a3;

			return c1;
		};

		auto uncharted2_filmic = [&](XMFLOAT3 v)
		{
			float exposure_bias = 2.0f;
			XMVECTOR curr = uncharted2_tonemap_partial(Vector3::ScalarProduct(v, exposure_bias, false));

			XMFLOAT3 W = XMFLOAT3(11.2f, 11.2f, 11.2f);
			XMVECTOR white_scale = XMVECTOR({ 1.0f,1.0f,1.0f }) / uncharted2_tonemap_partial(W);
			XMFLOAT3 result;
			XMStoreFloat3(&result, XMVectorMultiply(curr, white_scale));
			return result;
		};

		XMFLOAT3 basecolors[MAX_COLORCURVES];
		for (int i = 0; i < pTrailComponent->m_nCurves; ++i) {
			XMFLOAT3 RGB = uncharted2_filmic(XMFLOAT3(pTrailComponent->m_fR_CurvePoints[i], pTrailComponent->m_fG_CurvePoints[i], pTrailComponent->m_fB_CurvePoints[i]));
			basecolors[i] = RGB;
		}

		int colorIndex = 0;

		for (int t = 0; t < colors.size(); ++t) {
			float newT = (float)t / colors.size();
			int index = 0;
			for (int i = 0; i < pTrailComponent->m_nCurves; ++i) {
				if (pTrailComponent->m_fColorCurveTimes_R[i] >= newT) {
					index = max(0, i - 1);
					break;
				}
			}

			float delta = newT - pTrailComponent->m_fColorCurveTimes_R[index];
			delta *= 1.0f / ((float)pTrailComponent->m_fColorCurveTimes_R[index + 1] - (float)pTrailComponent->m_fColorCurveTimes_R[index]);
			
			XMFLOAT3 newColor; XMStoreFloat3(&newColor, XMVectorLerp(XMLoadFloat3(&basecolors[index]), XMLoadFloat3(&basecolors[index + 1]), delta));

			BYTE byte_r = newColor.x * 255;
			BYTE byte_g = newColor.y * 255;
			BYTE byte_b = newColor.z * 255;

			BYTE abgr[4] = { (BYTE)(255), byte_b, byte_g, byte_r };
			BYTE rgba[4] = { byte_r, byte_g, byte_b, (BYTE)(255) };
			memcpy(&colors[colorIndex++], rgba, sizeof(ImU32));
		}
		


		/*for (int i = 0; i < pTrailComponent->m_nCurves - 1; ++i) {
			for (int j = 0; j < interpolationCount; ++j) {
				XMFLOAT3 newColor; XMStoreFloat3(&newColor, XMVectorLerp(XMLoadFloat3(&basecolors[i]), XMLoadFloat3(&basecolors[i + 1]), (float)j / interpolationCount));

				BYTE byte_r = newColor.x * 255;
				BYTE byte_g = newColor.y * 255;
				BYTE byte_b = newColor.z * 255;

				BYTE abgr[4] = { (BYTE)(255), byte_b, byte_g, byte_r };
				BYTE rgba[4] = { byte_r, byte_g, byte_b, (BYTE)(255) };
				memcpy(&colors[colorIndex++], rgba, sizeof(ImU32));
			}
		}*/

		//ImGui::ColorConvertFloat4ToU32

		static ImPlotColormap sampleColorMap = -1;
		if(sampleColorMap == -1) 
			sampleColorMap = ImPlot::AddColormap("testColorMap", colors.data(), colors.size(), false);
		else {
			ImPlotContext& gp = *GImPlot;
			ImU32* sampleColorTable = const_cast<ImU32*>(gp.ColormapData.GetKeys(sampleColorMap));
			ImU32 test[240]; memcpy(test, sampleColorTable, sizeof(ImU32) * 240);
			int TableSize = gp.ColormapData.GetTableSize(sampleColorMap);
			memcpy(sampleColorTable, colors.data(), colors.size() * sizeof(ImU32));
		}
		//ImVec2 cursorPos = ImGui::GetCurrentWindow()->DC.CursorPos;
		//ImPlot::RenderColorBar(colors.data(), colors.size(), *ImGui::GetWindowDrawList(), ImRect(cursorPos.x, cursorPos.y, cursorPos.x + 200, cursorPos.y + 10), false, false, false);
		ImPlot::ColormapSlider("testColorSlider", &testColors, &abc, "", sampleColorMap);
		//ImPlot::PopColormap();

		/*ImPlot::SetNextLineStyle(ImVec4(1, 0.5f, 1, 1));
		ImPlot::PlotLine("##t1", &P[1].x, &P[1].y, 1, 0, 0, sizeof(ImPlotPoint));
		ImPlot::SetNextLineStyle(ImVec4(0, 0.5f, 1, 1));
		ImPlot::PlotLine("##t2", &P[3].x, &P[3].y, 1, 0, 0, sizeof(ImPlotPoint));
		ImPlot::SetNextLineStyle(ImVec4(0, 0.9f, 0, 1), 2);*/
		//ImPlot::PlotLine("##bez", &B[0].x, &B[0].y, 100, 0, 0, sizeof(ImPlotPoint));

		ImPlot::EndPlot();
	}
	//ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	//ImVec2 testPos = ImGui::GetCursorScreenPos();
	//for (int i = 0; i < pTrailComponent->m_nCurves - 1; ++i) {
	//	ImU32 color1; ImU32 color2;
	//	//HIWORD()
	//	char rgba1[4] = { (char)(max(pTrailComponent->m_fR_CurvePoints[i], 1.0f)*255), (char)pTrailComponent->m_fG_CurvePoints[i],  (char)pTrailComponent->m_fB_CurvePoints[i], (char)1.0f * 255 };
	//	char rgba2[4] = { (char)pTrailComponent->m_fR_CurvePoints[i],   (char)pTrailComponent->m_fG_CurvePoints[i],  (char)pTrailComponent->m_fB_CurvePoints[i], (char)1.0f };
	//	ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(testPos.x + (200 * pTrailComponent->m_fColorCurveTimes_R[i]), testPos.y),
	//		ImVec2(testPos.x + (200 * pTrailComponent->m_fColorCurveTimes_R[i + 1]), testPos.y + 10), color1, color2, color2, color1);
	//}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

	UINT m_iNoiseTextureN = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureListIndex(TextureType::TrailNoiseTexture);
	pTrailComponent->SetNoiseTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::TrailNoiseTexture));
	std::shared_ptr<CTexture> pNoiseTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetTexture(TextureType::TrailNoiseTexture);
	std::vector<const char*> Noiseitems;
	std::vector <std::string> Noisestr(100);
	for (int i = 0; i < m_iNoiseTextureN; i++)
	{
		std::wstring wstr = pNoiseTexture->GetTextureName(i);
		Noisestr[i].assign(wstr.begin(), wstr.end());
		Noiseitems.emplace_back(str[i].c_str());
	}

	int NoiseTextureIndexCache = pTrailComponent->GetNoiseTextureIndex();
	if (ImGui::Combo(U8STR("노이즈 텍스쳐##TrailEffect"), (int*)(&pTrailComponent->GetNoiseTextureIndex()), Noiseitems.data(), Noiseitems.size()))
	{
	}

	my_image_width = 0.2f * m_lDesktopHeight;
	my_image_height = 0.2f * m_lDesktopHeight;

	SetPreviewTexture(Locator.GetDevice(), pNoiseTexture.get(), pTrailComponent->GetNoiseTextureIndex(), 5, PREVIEW_TEXTURE_TYPE::TYPE_TRAILNOISE);
	ImGui::Image((ImTextureID)(m_d3dSrvGPUDescriptorHandle_TrailNoiseTexture.ptr), ImVec2((float)my_image_height, (float)my_image_height));

	/*ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("XSize##TrailEffect", &pParticleComponent->GetXSize(), DRAG_FLOAT_UNIT, PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX, "%.2f", 0))
		pParticleComponent->GetXSize() = std::clamp(pParticleComponent->GetXSize(), PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX);

	ImGui::SetNextItemWidth(190.f);
	if (ImGui::DragFloat("YSize##TrailEffect", &pParticleComponent->GetYSize(), DRAG_FLOAT_UNIT, PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX, "%.2f", 0))
		pParticleComponent->GetYSize() = std::clamp(pParticleComponent->GetYSize(), PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX);*/

		//ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
		//if (ImGui::DragFloat(U8STR("투명도##TrailEffect"), &pParticleComponent->GetAlpha(), DRAG_FLOAT_UNIT, PARTICLE_ALPHA_MIN, PARTICLE_ALPHA_MAX, "%.2f", 0))
		//	pParticleComponent->GetAlpha() = std::clamp(pParticleComponent->GetAlpha(), PARTICLE_ALPHA_MIN, PARTICLE_ALPHA_MAX);

		//ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
		//ImGui::ColorEdit3(U8STR("색상"), (float*)&pParticleComponent->GetColor()); // Edit 3 floats representing a color

	ImGui::End();
}


void CImGuiManager::ShowDamageAnimationManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("대미지 애니메이션 관리자"), b_open, my_window_flags);
	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pCurrentAnimation->GetDamageAnimationComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##DamageAnimation"), &pDamageAnimationComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("최대거리##DamageAnimation"), &pDamageAnimationComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, DAMAGE_ANIMATION_DISTANCE_MIN, DAMAGE_ANIMATION_DISTANCE_MAX, "%.2f", 0))
		pDamageAnimationComponent->GetMaxDistance() = std::clamp(pDamageAnimationComponent->GetMaxDistance(), DAMAGE_ANIMATION_DISTANCE_MIN, DAMAGE_ANIMATION_DISTANCE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("속도##DamageAnimation"), &pDamageAnimationComponent->GetSpeed(), DRAG_FLOAT_UNIT, DAMAGE_ANIMATION_SPEED_MIN, DAMAGE_ANIMATION_SPEED_MAX, "%.2f", 0))
		pDamageAnimationComponent->GetSpeed() = std::clamp(pDamageAnimationComponent->GetSpeed(), DAMAGE_ANIMATION_SPEED_MIN, DAMAGE_ANIMATION_SPEED_MAX);

	ImGui::End();
}
void CImGuiManager::ShowShakeAnimationManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("흔들림 애니메이션 관리자"), b_open, my_window_flags);
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pCurrentAnimation->GetShakeAnimationComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##ShakeAnimation"), &pShakeAnimationComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지속시간##ShakeAnimation"), &pShakeAnimationComponent->GetDuration(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_DURATION_MIN, SHAKE_ANIMATION_DURATION_MAX, "%.2f", 0))
		pShakeAnimationComponent->GetDuration() = std::clamp(pShakeAnimationComponent->GetDuration(), SHAKE_ANIMATION_DURATION_MIN, SHAKE_ANIMATION_DURATION_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("거리##ShakeAnimation"), &pShakeAnimationComponent->GetDistance(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_DISTANCE_MIN, SHAKE_ANIMATION_DISTANCE_MAX, "%.2f", 0))
		pShakeAnimationComponent->GetDistance() = std::clamp(pShakeAnimationComponent->GetDistance(), SHAKE_ANIMATION_DISTANCE_MIN, SHAKE_ANIMATION_DISTANCE_MAX);


	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("빈도##ShakeAnimation"), &pShakeAnimationComponent->GetFrequency(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_FREQUENCY_MIN, SHAKE_ANIMATION_FREQUENCY_MAX, "%.3f", 0))
		pShakeAnimationComponent->GetFrequency() = std::clamp(pShakeAnimationComponent->GetFrequency(), SHAKE_ANIMATION_FREQUENCY_MIN, SHAKE_ANIMATION_FREQUENCY_MAX);

	ImGui::End();
}
void CImGuiManager::ShowStunAnimationManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("경직 애니메이션 관리자"), b_open, my_window_flags);
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pCurrentAnimation->GetStunAnimationComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##StunAnimation"), &pStunAnimationComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("경직시간##StunAnimation"), &pStunAnimationComponent->GetStunTime(), DRAG_FLOAT_UNIT, STUN_ANIMATION_STUNTIME_MIN, STUN_ANIMATION_STUNTIME_MAX, "%.2f", 0))
		pStunAnimationComponent->GetStunTime() = std::clamp(pStunAnimationComponent->GetStunTime(), STUN_ANIMATION_STUNTIME_MIN, STUN_ANIMATION_STUNTIME_MAX);

	ImGui::End();
}
void CImGuiManager::ShowHitLagManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;
	ImGui::Begin(U8STR("역경직 관리자"), b_open, my_window_flags);
	HitLagComponent* pHitLagComponent = dynamic_cast<HitLagComponent*>(pCurrentAnimation->GetHitLagComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##HitLag"), &pHitLagComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지속시간##Move"), &pHitLagComponent->GetMaxLagTime(), DRAG_FLOAT_UNIT, HIT_LAG_MAXTIME_MIN, HIT_LAG_MAXTIME_MAX, "%.2f", 0))
		pHitLagComponent->GetMaxLagTime() = std::clamp(pHitLagComponent->GetMaxLagTime(), HIT_LAG_MAXTIME_MIN, HIT_LAG_MAXTIME_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("애니메이션 배율##Move"), &pHitLagComponent->GetLagScale(), DRAG_FLOAT_UNIT, HIT_LAG_SCALEWEIGHT_MIN, HIT_LAG_SCALEWEIGHT_MAX, "%.2f", 0))
		pHitLagComponent->GetLagScale() = std::clamp(pHitLagComponent->GetLagScale(), HIT_LAG_SCALEWEIGHT_MIN, HIT_LAG_SCALEWEIGHT_MAX);
	ImGui::End();
}
void CImGuiManager::ShowCameraMoveManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("카메라 이동 관리자"), b_open, my_window_flags);
	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pCurrentAnimation->GetCameraMoveComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##Move"), &pCameraMoveComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("거리##Move"), &pCameraMoveComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, CAMERA_MOVE_DISTANCE_MIN, CAMERA_MOVE_DISTANCE_MAX, "%.2f", 0))
		pCameraMoveComponent->GetMaxDistance() = std::clamp(pCameraMoveComponent->GetMaxDistance(), CAMERA_MOVE_DISTANCE_MIN, CAMERA_MOVE_DISTANCE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("시간##Move"), &pCameraMoveComponent->GetMovingTime(), DRAG_FLOAT_UNIT, CAMERA_MOVE_TIME_MIN, CAMERA_MOVE_TIME_MAX, "%.2f", 0))
		pCameraMoveComponent->GetMovingTime() = std::clamp(pCameraMoveComponent->GetMovingTime(), CAMERA_MOVE_TIME_MIN, CAMERA_MOVE_TIME_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("복귀시간##Move"), &pCameraMoveComponent->GetRollBackTime(), DRAG_FLOAT_UNIT, CAMERA_MOVE_ROLLBACKTIME_MIN, CAMERA_MOVE_ROLLBACKTIME_MAX, "%.2f", 0))
		pCameraMoveComponent->GetRollBackTime() = std::clamp(pCameraMoveComponent->GetRollBackTime(), CAMERA_MOVE_ROLLBACKTIME_MIN, CAMERA_MOVE_ROLLBACKTIME_MAX);

	ImGui::End();
}
void CImGuiManager::ShowCameraShakeManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("카메라 흔들림 관리자"), b_open, my_window_flags);
	CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pCurrentAnimation->GetCameraShakeComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##Shake"), &pCameraShakerComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("흔들림 규모##Shake"), &pCameraShakerComponent->GetMagnitude(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_MAGNITUDE_MIN, CAMERA_SHAKE_MAGNITUDE_MAX, "%.2f", 0))
		pCameraShakerComponent->GetMagnitude() = std::clamp(pCameraShakerComponent->GetMagnitude(), CAMERA_SHAKE_MAGNITUDE_MIN, CAMERA_SHAKE_MAGNITUDE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지속 시간##Shake"), &pCameraShakerComponent->GetDuration(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_DURATION_MIN, CAMERA_SHAKE_DURATION_MAX, "%.2f", 0))
		pCameraShakerComponent->GetDuration() = std::clamp(pCameraShakerComponent->GetDuration(), CAMERA_SHAKE_DURATION_MIN, CAMERA_SHAKE_DURATION_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("빈도##Shake"), &pCameraShakerComponent->GetFrequency(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_FREQUENCY_MIN, CAMERA_SHAKE_FREQUENCY_MAX, "%.3f", 0))
		pCameraShakerComponent->GetFrequency() = std::clamp(pCameraShakerComponent->GetFrequency(), CAMERA_SHAKE_FREQUENCY_MIN, CAMERA_SHAKE_FREQUENCY_MAX);

	ImGui::End();
}
void CImGuiManager::ShowCameraZoomManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("카메라 줌 관리자"), b_open, my_window_flags);
	CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pCurrentAnimation->GetCameraZoomerComponent());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##Zoom"), &pCameraZoomerComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("거리##Zoom"), &pCameraZoomerComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_DISTANCE_MIN, CAMERA_ZOOMINOUT_DISTANCE_MAX, "%.2f", 0))
		pCameraZoomerComponent->GetMaxDistance() = std::clamp(pCameraZoomerComponent->GetMaxDistance(), CAMERA_ZOOMINOUT_DISTANCE_MIN, CAMERA_ZOOMINOUT_DISTANCE_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("시간##Zoom"), &pCameraZoomerComponent->GetMovingTime(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_TIME_MIN, CAMERA_ZOOMINOUT_TIME_MAX, "%.2f", 0))
		pCameraZoomerComponent->GetMovingTime() = std::clamp(pCameraZoomerComponent->GetMovingTime(), CAMERA_ZOOMINOUT_TIME_MIN, CAMERA_ZOOMINOUT_TIME_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("복귀시간##Zoom"), &pCameraZoomerComponent->GetRollBackTime(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_ROLLBACKTIME_MIN, CAMERA_ZOOMINOUT_ROLLBACKTIME_MAX, "%.2f", 0))
		pCameraZoomerComponent->GetRollBackTime() = std::clamp(pCameraZoomerComponent->GetRollBackTime(), CAMERA_ZOOMINOUT_ROLLBACKTIME_MIN, CAMERA_ZOOMINOUT_ROLLBACKTIME_MAX);


	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox("IN / OUT##Zoom", &pCameraZoomerComponent->GetIsIn());

	ImGui::End();
}
void CImGuiManager::ShowShockSoundManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("충격 사운드 관리자"), b_open, my_window_flags);
	std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOCK);

	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShockSoundComponent());

	std::vector<const char*> items;
	for (auto& path : paths) {
		items.push_back(path.c_str());
	}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##effectsound"), &pShockSoundComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Combo(U8STR("소리##effectsound"), (int*)&pShockSoundComponent->GetSoundNumber(), items.data(), items.size());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지연시간##effectsound"), &pShockSoundComponent->GetDelay(), DRAG_FLOAT_UNIT, SHOCK_SOUND_DELAY_MIN, SHOCK_SOUND_DELAY_MAX, "%.2f", 0))
		pShockSoundComponent->GetDelay() = std::clamp(pShockSoundComponent->GetDelay(), SHOCK_SOUND_DELAY_MIN, SHOCK_SOUND_DELAY_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("음량##effectsound"), &pShockSoundComponent->GetVolume(), DRAG_FLOAT_UNIT, SHOCK_SOUND_VOLUME_MIN, SHOCK_SOUND_VOLUME_MAX, "%.2f", 0))
		pShockSoundComponent->GetVolume() = std::clamp(pShockSoundComponent->GetVolume(), SHOCK_SOUND_VOLUME_MIN, SHOCK_SOUND_VOLUME_MAX);

	ImGui::End();
}
void CImGuiManager::ShowShootSoundManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("발사 사운드 관리자"), b_open, my_window_flags);
	std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOOT);

	SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShootSoundComponent());

	std::vector<const char*> items;
	for (auto& path : paths) {
		items.push_back(path.c_str());
	}

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기##shootsound"), &pShootSoundComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Combo(U8STR("소리##shootsound"), (int*)&pShootSoundComponent->GetSoundNumber(), items.data(), items.size());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지연시간##shootsound"), &pShootSoundComponent->GetDelay(), DRAG_FLOAT_UNIT, SHOOTING_SOUND_DELAY_MIN, SHOOTING_SOUND_DELAY_MAX, "%.2f", 0))
		pShootSoundComponent->GetDelay() = std::clamp(pShootSoundComponent->GetDelay(), SHOOTING_SOUND_DELAY_MIN, SHOOTING_SOUND_DELAY_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("음량##shootsound"), &pShootSoundComponent->GetVolume(), DRAG_FLOAT_UNIT, SHOOTING_SOUND_VOLUME_MIN, SHOOTING_SOUND_VOLUME_MAX, "%.2f", 0))
		pShootSoundComponent->GetVolume() = std::clamp(pShootSoundComponent->GetVolume(), SHOOTING_SOUND_VOLUME_MIN, SHOOTING_SOUND_VOLUME_MAX);

	ImGui::End();
}
void CImGuiManager::ShowDamageMoanSoundManager(CState<CPlayer>* pCurrentAnimation)
{
	ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
	bool* b_open = nullptr;

	ImGui::Begin(U8STR("대미지 신음 사운드 관리자"), b_open, my_window_flags);
	std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_VOICE);

	SoundPlayComponent* pGoblinMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetGoblinMoanComponent());
	SoundPlayComponent* pOrcMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetOrcMoanComponent());
	SoundPlayComponent* pSkeletonMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetSkeletonMoanComponent());

	std::vector<const char*> items;
	for (auto& path : paths) {
		items.push_back(path.c_str());
	}

	ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기(Goblin)##goblinmoansound"), &pGoblinMoanComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Combo(U8STR("소리(고블린)##goblinmoansound"), (int*)&pGoblinMoanComponent->GetSoundNumber(), items.data(), GOBLIN_MOAN_SOUND_NUM);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지연시간(고블린)##goblinmoansound"), &pGoblinMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0))
		pGoblinMoanComponent->GetDelay() = std::clamp(pGoblinMoanComponent->GetDelay(), MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("음량(고블린)##goblinmoansound"), &pGoblinMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0))
		pGoblinMoanComponent->GetVolume() = std::clamp(pGoblinMoanComponent->GetVolume(), MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX);

	ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기(오크)##orcmoansound"), &pOrcMoanComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Combo(U8STR("소리(오크)##orcmoansound"), (int*)&pOrcMoanComponent->GetSoundNumber(), items.data() + GOBLIN_MOAN_SOUND_NUM, ORC_MOAN_SOUND_NUM);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지연시간(오크)##orcmoansound"), &pOrcMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0))
		pOrcMoanComponent->GetDelay() = std::clamp(pOrcMoanComponent->GetDelay(), MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("음량(오크)##orcmoansound"), &pOrcMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0))
		pOrcMoanComponent->GetVolume() = std::clamp(pOrcMoanComponent->GetVolume(), MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX);

	ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
	ImGui::Checkbox(U8STR("켜기/끄기(Skeleton)##skeletonmoansound"), &pSkeletonMoanComponent->GetEnable());

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	ImGui::Combo(U8STR("소리(스켈레톤)##skeletonmoansound"), (int*)&pSkeletonMoanComponent->GetSoundNumber(), items.data() + GOBLIN_MOAN_SOUND_NUM + ORC_MOAN_SOUND_NUM, SKELETON_MOAN_SOUND_NUM);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("지연시간(스켈레톤)##skeletonmoansound"), &pSkeletonMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0))
		pSkeletonMoanComponent->GetDelay() = std::clamp(pSkeletonMoanComponent->GetDelay(), MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX);

	ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
	if (ImGui::DragFloat(U8STR("음량(스켈레톤)##skeletonmoansound"), &pSkeletonMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0))
		pSkeletonMoanComponent->GetVolume() = std::clamp(pSkeletonMoanComponent->GetVolume(), MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX);

	ImGui::End();
}
void CImGuiManager::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	::SynchronizeResourceTransition(pd3dCommandList, m_pRTTexture->GetResource(0), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

	CSimulatorScene::GetInst()->OnPrepareRender(pd3dCommandList);

	CSimulatorScene::GetInst()->OnPreRender(pd3dCommandList, fTimeElapsed);

	CSimulatorScene::GetInst()->OnPrepareRenderTarget(pd3dCommandList, 1, &m_pd3dRtvCPUDescriptorHandles, *d3dDsvDescriptorCPUHandle);
	CSimulatorScene::GetInst()->SetHDRRenderSource(m_pRTTexture->GetResource(0));

	CSimulatorScene::GetInst()->Render(pd3dCommandList, fTimeElapsed, fCurrentTime, pCamera);

	::SynchronizeResourceTransition(pd3dCommandList, m_pRTTexture->GetResource(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
}
void CImGuiManager::PrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle)
{
	FLOAT pfDefaultClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pd3dCommandList->ClearDepthStencilView(*d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	pd3dCommandList->ClearRenderTargetView(m_pd3dRtvCPUDescriptorHandles, pfDefaultClearColor, 0, NULL);
	pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvCPUDescriptorHandles, FALSE, d3dDsvDescriptorCPUHandle);
}
void CImGuiManager::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Rendering
	ImGui::Render();

	pd3dCommandList->SetDescriptorHeaps(1, m_pd3dSrvDescHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pd3dCommandList);
}
void CImGuiManager::OnPostRender()
{
	CSimulatorScene::GetInst()->OnPostRender();
}
void CImGuiManager::OnDestroy()
{
}

