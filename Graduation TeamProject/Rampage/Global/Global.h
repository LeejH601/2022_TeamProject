#pragma once

enum class MESSAGE_TYPE
{
	Msg_CameraMoveStart,
	Msg_CameraShakeStart,
	Msg_CameraZoomStart,
	Msg_SoundEffectReady,
	Msg_SoundDamageReady,
	Msg_SoundShootReady,
	Msg_PlaySoundEffect,
	Msg_PlaySoundDamage,
	Msg_PlaySoundShoot,
	END_ENUM
};

enum class SOUND_CATEGORY
{
	SOUND_SHOCK,
	SOUND_BACKGROUND,
	SOUND_VOICE,
	SOUND_SHOOT,
	SOUND_EFFECT,
};