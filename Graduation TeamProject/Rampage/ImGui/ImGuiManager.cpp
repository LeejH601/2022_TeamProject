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
#include "..\Object\AnimationComponent.h"

#define NUM_FRAMES_IN_FLIGHT 3

//========================================================================
void DataLoader::SaveComponentSets()
{
	std::string path;
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

		path = file_path + std::to_string(i) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "wb");

		SaveComponentSet(pInFile, pCurrentAnimation);

		fclose(pInFile);
	}
}

void DataLoader::LoadComponentSets()
{
	std::string path;
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
		path = file_path + std::to_string(i) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "rb");
		if (!pInFile)
			continue;
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
	WriteStringFromFile(pInFile, std::string("<ParticleNumber>:"));
	WriteIntegerFromFile(pInFile, pParticleComponent->GetParticleNumber());
	WriteStringFromFile(pInFile, std::string("<ParticleIndex>:"));
	WriteIntegerFromFile(pInFile, pParticleComponent->GetParticleIndex());
	WriteStringFromFile(pInFile, std::string("<Alpha>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetAlpha());
	WriteStringFromFile(pInFile, std::string("<LifeTime>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetLifeTime());
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("<Size>:"));
	WriteFloatFromFile(pInFile, pParticleComponent->GetSize());
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
	WriteStringFromFile(pInFile, std::string("<Size>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetSize());
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("<Alpha>:"));
	WriteFloatFromFile(pInFile, pImpactComponent->GetAlpha());
	WriteStringFromFile(pInFile, std::string("</ImpactComponent>:"));

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
				else if (!strcmp(buf, "<ParticleNumber>:"))
				{
					pParticleComponent->SetParticleNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<ParticleIndex>:"))
				{
					pParticleComponent->SetParticleIndex(ReadIntegerFromFile(pInFile));

					std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetParticleTextureList();

					std::shared_ptr pTexture = vTexture[pParticleComponent->GetParticleIndex()];
					pParticleComponent->SetParticleTexture(pTexture);
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
				else if (!strcmp(buf, "<Size>:"))
				{
					pParticleComponent->SetSize(ReadFloatFromFile(pInFile));
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

					std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetBillBoardTextureList();

					std::shared_ptr<CTexture> pTexture = vTexture[pImpactComponent->GetTextureIndex()];
					pImpactComponent->SetImpactTexture(pTexture);
				}
				else if (!strcmp(buf, "<Size>:"))
				{
					pImpactComponent->SetSize(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Speed>:"))
				{
					pImpactComponent->SetSpeed(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Alpha>:"))
				{
					pImpactComponent->SetAlpha(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</ImpactComponent>:"))
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
	d3dDescriptorHeapDesc.NumDescriptors = 3;
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
ID3D12Resource* CImGuiManager::GetRTTextureResource() { return m_pRTTexture->GetResource(0); }
void CImGuiManager::Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, const RECT& DeskTopCoordinatesRect)
{
	CreateSrvDescriptorHeaps(pd3dDevice);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescHeap.Get(),
		m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	/*Locator.CreateSimulatorCamera(pd3dDevice, pd3dCommandList);
	m_pCamera = Locator.GetSimulaterCamera();*/

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
	m_pDataLoader->LoadComponentSets();

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

	// Show my window.
	{
		ImGuiWindowFlags my_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		bool* p_open = NULL;

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowSize(ImVec2(m_lDesktopWidth * 0.85f, 0.0f), ImGuiCond_Always);
		ImGui::Begin("Simulator", p_open, my_window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImVec2 initial_curpos = ImGui::GetCursorPos();
		{
			int my_image_width = 0.55f * m_lDesktopWidth;
			int my_image_height = 0.55f * m_lDesktopHeight;
			ImGui::Image((ImTextureID)m_pRTTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr, ImVec2((float)my_image_width, (float)my_image_height));
		}
		ImVec2 button_pos = ImGui::GetCursorPos();

		// Menu Bar
		initial_curpos.x += 0.555f * m_lDesktopWidth;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Impact Effect"))
		{
			ImpactEffectComponent* pImpactEffectComponent = dynamic_cast<ImpactEffectComponent*>(pCurrentAnimation->GetImpactComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##SpriteEffect", &pImpactEffectComponent->GetEnable());

			std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetBillBoardTextureList();
			std::vector<const char*> items;
			std::vector <std::string> str(100);
			for (int i = 0; i < vTexture.size(); i++)
			{
				std::wstring wstr = vTexture[i]->GetTextureName(0);
				str[i].assign(wstr.begin(), wstr.end());
				items.emplace_back(str[i].c_str());
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if(ImGui::Combo("##Attack", (int*)(&pImpactEffectComponent->GetTextureIndex()), items.data(), items.size()));
			{
				std::shared_ptr<CTexture> pTexture = vTexture[pImpactEffectComponent->GetTextureIndex()];
				pImpactEffectComponent->SetImpactTexture(pTexture);
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Speed", &pImpactEffectComponent->GetSpeed(), DRAG_FLOAT_UNIT, IMPACT_SPEED_MIN, IMPACT_SPEED_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Alpha", &pImpactEffectComponent->GetAlpha(), DRAG_FLOAT_UNIT, IMPACT_ALPHA_MIN, IMPACT_ALPHA_MAX, "%.2f", 0);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Particle Effect"))
		{
			ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(pCurrentAnimation->GetParticleComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##ParticleEffect", &pParticleComponent->GetEnable());

			std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetParticleTextureList();
			std::vector<const char*> items;
			std::vector <std::string> str(100);
			for (int i = 0; i < vTexture.size(); i++)
			{
				std::wstring wstr = vTexture[i]->GetTextureName(0);
				str[i].assign(wstr.begin(), wstr.end());
				items.emplace_back(str[i].c_str());
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::Combo("Texture##ParticleEffect", (int*)(&pParticleComponent->GetParticleIndex()), items.data(), items.size()))
			{
				std::shared_ptr pTexture = vTexture[pParticleComponent->GetParticleIndex()];
				pParticleComponent->SetParticleTexture(pTexture);
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Size##ParticleEffect", &pParticleComponent->GetSize(), DRAG_FLOAT_UNIT, PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Alpha##ParticleEffect", &pParticleComponent->GetAlpha(), DRAG_FLOAT_UNIT, PARTICLE_ALPHA_MIN, PARTICLE_ALPHA_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("LifeTime##ParticleEffect", &pParticleComponent->GetLifeTime(), DRAG_FLOAT_UNIT, PARTICLE_LIFETIME_MIN, PARTICLE_LIFETIME_MAX, "%.1f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragInt("ParticleCount##ParticleEffect", &pParticleComponent->GetParticleNumber(), DRAG_FLOAT_UNIT, PARTICLE_COUNT_MIN, PARTICLE_COUNT_MAX, "%d", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Speed##ParticleEffect", &pParticleComponent->GetSpeed(), DRAG_FLOAT_UNIT, PARTICLE_SPEED_MIN, PARTICLE_SPEED_MAX, "%.1f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(30.f);
			ImGui::ColorEdit3("ParticleEffect", (float*)&pParticleComponent->GetColor()); // Edit 3 floats representing a color
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Afterimage Effect"))
		{
			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Damage Animation"))
		{
			DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pCurrentAnimation->GetDamageAnimationComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##DamageAnimation", &pDamageAnimationComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::DragFloat("MaxDistance##DamageAnimation", &pDamageAnimationComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, DAMAGE_ANIMATION_DISTANCE_MIN, DAMAGE_ANIMATION_DISTANCE_MAX, "%.2f", 0))
				pDamageAnimationComponent->GetMaxDistance() = std::clamp(pDamageAnimationComponent->GetMaxDistance(), DAMAGE_ANIMATION_DISTANCE_MIN, DAMAGE_ANIMATION_DISTANCE_MAX);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::DragFloat("Speed##DamageAnimation", &pDamageAnimationComponent->GetSpeed(), DRAG_FLOAT_UNIT, DAMAGE_ANIMATION_SPEED_MIN, DAMAGE_ANIMATION_SPEED_MAX, "%.2f", 0))
				pDamageAnimationComponent->GetSpeed() = std::clamp(pDamageAnimationComponent->GetSpeed(), DAMAGE_ANIMATION_SPEED_MIN, DAMAGE_ANIMATION_SPEED_MAX);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shake Animation"))
		{
			ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pCurrentAnimation->GetShakeAnimationComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##ShakeAnimation", &pShakeAnimationComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::DragFloat("Duration##ShakeAnimation", &pShakeAnimationComponent->GetDuration(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_DURATION_MIN, SHAKE_ANIMATION_DURATION_MAX, "%.2f", 0))
				pShakeAnimationComponent->GetDuration() = std::clamp(pShakeAnimationComponent->GetDuration(), SHAKE_ANIMATION_DURATION_MIN, SHAKE_ANIMATION_DURATION_MAX);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if(ImGui::DragFloat("Distance##ShakeAnimation", &pShakeAnimationComponent->GetDistance(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_DISTANCE_MIN, SHAKE_ANIMATION_DISTANCE_MAX, "%.2f", 0))
				pShakeAnimationComponent->GetDistance() = std::clamp(pShakeAnimationComponent->GetDistance(), SHAKE_ANIMATION_DISTANCE_MIN, SHAKE_ANIMATION_DISTANCE_MAX);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::DragFloat("Frequency##ShakeAnimation", &pShakeAnimationComponent->GetFrequency(), DRAG_FLOAT_UNIT, SHAKE_ANIMATION_FREQUENCY_MIN, SHAKE_ANIMATION_FREQUENCY_MAX, "%.3f", 0))
				pShakeAnimationComponent->GetFrequency() = std::clamp(pShakeAnimationComponent->GetFrequency(), SHAKE_ANIMATION_FREQUENCY_MIN, SHAKE_ANIMATION_FREQUENCY_MAX);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Stun Animation"))
		{
			StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pCurrentAnimation->GetStunAnimationComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##StunAnimation", &pStunAnimationComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			if (ImGui::DragFloat("StunTime##StunAnimation", &pStunAnimationComponent->GetStunTime(), DRAG_FLOAT_UNIT, STUN_ANIMATION_STUNTIME_MIN, STUN_ANIMATION_STUNTIME_MAX, "%.2f", 0))
				pStunAnimationComponent->GetStunTime() = std::clamp(pStunAnimationComponent->GetStunTime(), STUN_ANIMATION_STUNTIME_MIN, STUN_ANIMATION_STUNTIME_MAX);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera Move"))
		{
			CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pCurrentAnimation->GetCameraMoveComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##Move", &pCameraMoveComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			ImGui::DragFloat("Distance##Move", &pCameraMoveComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, CAMERA_MOVE_DISTANCE_MIN, CAMERA_MOVE_DISTANCE_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Time##Move", &pCameraMoveComponent->GetMovingTime(), DRAG_FLOAT_UNIT, CAMERA_MOVE_TIME_MIN, CAMERA_MOVE_TIME_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("RollBackTime##Move", &pCameraMoveComponent->GetRollBackTime(), DRAG_FLOAT_UNIT, CAMERA_MOVE_ROLLBACKTIME_MIN, CAMERA_MOVE_ROLLBACKTIME_MAX, "%.2f", 0);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera Shake"))
		{
			CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pCurrentAnimation->GetCameraShakeComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##Shake", &pCameraShakerComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			ImGui::DragFloat("Magnitude##Shake", &pCameraShakerComponent->GetMagnitude(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_MAGNITUDE_MIN, CAMERA_SHAKE_MAGNITUDE_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Duration##Shake", &pCameraShakerComponent->GetDuration(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_DURATION_MIN, CAMERA_SHAKE_DURATION_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Frequency##Shake", &pCameraShakerComponent->GetFrequency(), DRAG_FLOAT_UNIT, CAMERA_SHAKE_FREQUENCY_MIN, CAMERA_SHAKE_FREQUENCY_MAX, "%.3f", 0);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera ZoomIn/ZoomOut"))
		{
			CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pCurrentAnimation->GetCameraZoomerComponent());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##Zoom", &pCameraZoomerComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);

			ImGui::DragFloat("Distance##Zoom", &pCameraZoomerComponent->GetMaxDistance(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_DISTANCE_MIN, CAMERA_ZOOMINOUT_DISTANCE_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Time##Zoom", &pCameraZoomerComponent->GetMovingTime(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_TIME_MIN, CAMERA_ZOOMINOUT_TIME_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("RollBackTime##Zoom", &pCameraZoomerComponent->GetRollBackTime(), DRAG_FLOAT_UNIT, CAMERA_ZOOMINOUT_ROLLBACKTIME_MIN, CAMERA_ZOOMINOUT_ROLLBACKTIME_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("IN / OUT##Zoom", &pCameraZoomerComponent->GetIsIn());
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shock Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOCK);
			
			SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShockSoundComponent());
			
			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##effectsound", &pShockSoundComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Combo("Sound##effectsound", (int*)&pShockSoundComponent->GetSoundNumber(), items.data(), items.size());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Delay##effectsound", &pShockSoundComponent->GetDelay(), DRAG_FLOAT_UNIT, SHOCK_SOUND_DELAY_MIN, SHOCK_SOUND_DELAY_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Volume##effectsound", &pShockSoundComponent->GetVolume(), DRAG_FLOAT_UNIT, SHOCK_SOUND_VOLUME_MIN, SHOCK_SOUND_VOLUME_MAX, "%.2f", 0);
		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shooting Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOOT);

			SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShootSoundComponent());

			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off##shootsound", &pShootSoundComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Combo("Sound##shootsound", (int*)&pShootSoundComponent->GetSoundNumber(), items.data(), items.size());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Delay##shootsound", &pShootSoundComponent->GetDelay(), DRAG_FLOAT_UNIT, SHOOTING_SOUND_DELAY_MIN, SHOOTING_SOUND_DELAY_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Volume##shootsound", &pShootSoundComponent->GetVolume(), DRAG_FLOAT_UNIT, SHOOTING_SOUND_VOLUME_MIN, SHOOTING_SOUND_VOLUME_MAX, "%.2f", 0);

		}

		initial_curpos.y += 0.023f * m_lDesktopHeight;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Damage Moan Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_VOICE);

			SoundPlayComponent* pGoblinMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetGoblinMoanComponent());
			SoundPlayComponent* pOrcMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetOrcMoanComponent());
			SoundPlayComponent* pSkeletonMoanComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetSkeletonMoanComponent());

			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off(Goblin)##goblinmoansound", &pGoblinMoanComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Combo("Sound(Goblin)##goblinmoansound", (int*)&pGoblinMoanComponent->GetSoundNumber(), items.data(), GOBLIN_MOAN_SOUND_NUM);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Delay(Goblin)##goblinmoansound", &pGoblinMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Volume(Goblin)##goblinmoansound", &pGoblinMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0);

			initial_curpos.y += 50.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off(Orc)##orcmoansound", &pOrcMoanComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Combo("Sound(Orc)##orcmoansound", (int*)&pOrcMoanComponent->GetSoundNumber(), items.data() + GOBLIN_MOAN_SOUND_NUM, ORC_MOAN_SOUND_NUM);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Delay(Orc)##orcmoansound", &pOrcMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Volume(Orc)##orcmoansound", &pOrcMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0);

			initial_curpos.y += 50.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.125f * m_lDesktopWidth);
			ImGui::Checkbox("On/Off(Skeleton)##skeletonmoansound", &pSkeletonMoanComponent->GetEnable());

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::Combo("Sound(Skeleton)##orcmoansound", (int*)&pSkeletonMoanComponent->GetSoundNumber(), items.data() + GOBLIN_MOAN_SOUND_NUM + ORC_MOAN_SOUND_NUM, SKELETON_MOAN_SOUND_NUM);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Delay(Skeleton)##orcmoansound", &pSkeletonMoanComponent->GetDelay(), DRAG_FLOAT_UNIT, MOAN_SOUND_DELAY_MIN, MOAN_SOUND_DELAY_MAX, "%.2f", 0);

			initial_curpos.y += 0.023f * m_lDesktopHeight;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(0.1f * m_lDesktopWidth);
			ImGui::DragFloat("Volume(Skeleton)##orcmoansound", &pSkeletonMoanComponent->GetVolume(), DRAG_FLOAT_UNIT, MOAN_SOUND_VOLUME_MIN, MOAN_SOUND_VOLUME_MAX, "%.2f", 0);
		}

		button_pos.y += 5.f;
		ImGui::SetCursorPos(button_pos);

		if (ImGui::Button("Animation1", ImVec2(0.09f * m_lDesktopWidth, 0.042f * m_lDesktopHeight))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(0);
			Player_Animation_Number = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Animation2", ImVec2(0.09f * m_lDesktopWidth, 0.042f * m_lDesktopHeight))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(1);
			Player_Animation_Number = 1;

		}
		ImGui::SameLine();
		if (ImGui::Button("Animation3", ImVec2(0.09f * m_lDesktopWidth, 0.042f * m_lDesktopHeight))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(2);
			Player_Animation_Number = 2;
		}

		ImGui::End();
	}
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
	m_pDataLoader->SaveComponentSets();
}

