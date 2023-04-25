#pragma once
#define GOBLIN_MOAN_SOUND_NUM 3
#define ORC_MOAN_SOUND_NUM 6
#define SKELETON_MOAN_SOUND_NUM 1

enum class MessageType {
	PLAYER_ATTACK,
	PLAY_SOUND,
	COLLISION,
	ONGROUND,
	CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER,
	UPDATE_CAMERA,
	UPDATE_OBJECT,
	UPDATE_BILLBOARD,
	UPDATE_PARTICLE,
	UPDATE_SMOKEPARTICLE,
	UPDATE_UPDOWNPARTICLE,
	UPDATE_TRAILPARTICLE,
	UPDATE_SPRITE,
	REQUEST_REGISTERARTI,
	MESSAGE_END,
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

enum class MONSTER_TYPE
{
	NONE,
	GOBLIN,
	ORC,
	SKELETON
};

enum class SCENE_RETURN_TYPE
{
	NONE,
	POP_SCENE,
	RETURN_PREVIOUS_SCENE
};