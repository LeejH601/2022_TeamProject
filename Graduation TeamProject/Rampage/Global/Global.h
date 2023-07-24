#pragma once
#define GOBLIN_MOAN_SOUND_NUM 3
#define ORC_MOAN_SOUND_NUM 6
#define SKELETON_MOAN_SOUND_NUM 1

enum class MessageType {
	MONSTER_ATTACK,
	MONSTER_DEAD,
	PLAYER_ATTACK,
	PLAY_SOUND,
	COLLISION,
	ONGROUND,
	CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER,
	APPLY_DAMAGE,
	CINEMATIC_ALL_UPDATED,
	UPDATE_CAMERA,
	UPDATE_OBJECT,
	UPDATE_BILLBOARD,
	UPDATE_PARTICLE,
	UPDATE_SMOKEPARTICLE,
	UPDATE_UPDOWNPARTICLE,
	UPDATE_TRAILPARTICLE,
	UPDATE_SLASHHITPARTICLE,
	UPDATE_SWORDTRAIL,
	UPDATE_SPRITE,
	ALLY_DAMAGED,
	AUTO_RESET,
	SPECIALMOVE_DAMAGED,
	SET_DYNAMIC_TIMER_SCALE,
	UPDATE_HITLAG,
	REQUEST_REGISTERARTI,
	REQUEST_SLEEPARTI,
	PLAYER_DEAD,
	DRINK_POTION,
	UPDATE_SPMOVE,
	MESSAGE_END,
};

enum class SOUND_CATEGORY
{
	SOUND_SHOCK,
	SOUND_BACKGROUND,
	SOUND_VOICE,
	SOUND_SHOOT,
	SOUND_EFFECT,
	SOUND_UI_BUTTON,
	SOUND_UI_BUTTON_CLICK,
};

enum class SCENE_TYPE
{
	LOBBY_SCENE,
	MAIN_SCENE,
};

enum class SCENE_STATE
{
	NONE,
	TURN_TO_MAINSCENE,
	TURN_TO_LOBBYSCENE,
	COUNT
};

enum class PARTICLE_CATEGORY
{
	PARTICLE_SPHERE
};

enum class MONSTER_TYPE
{
	GOBLIN,
	ORC,
	SKELETON,
	NONE
};

enum class MAP_OBJ_TYPE
{
	ROCK,
	TREE,
	PROP,
};

enum class SCENE_RETURN_TYPE
{
	NONE,
	POP_LOGOSCENE,
	POP_SIMULATORSCENE,

	RETURN_PREVIOUS_SCENE,

	SWITCH_LOGOSCENE
};