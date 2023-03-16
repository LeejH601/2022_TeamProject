#pragma once

enum class MessageType {
	PLAYER_ATTACK,
	PLAY_SOUND
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

enum class SOUND_PLAY_TYPE
{
	SOUND_PT_NONE,
	SOUND_PT_ATK1,
	SOUND_PT_ATK2,
	SOUND_PT_ATK3,
};

enum class SCENE_TYPE
{
	LOBBY_SCENE,
	MAIN_SCENE,
};