#pragma once

enum class MessageType {
	PLAYER_ATTACK,
	PLAY_SOUND,
	COLLISION,
	UPDATE_CAMERA,
	UPDATE_OBJECT,
	UPDATE_PARTICLE,
};

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
	Msg_MonsterAttackSprite,
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

enum class SCENE_TYPE
{
	LOBBY_SCENE,
	MAIN_SCENE,
};

enum class PARTICLE_CATEGORY
{
	PARTICLE_SPHERE
};