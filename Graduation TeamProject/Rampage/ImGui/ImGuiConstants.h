#pragma once

#define DRAG_FLOAT_UNIT 0.01f

// IMPACT
#define IMPACT_LIFETIME_DEFAULT 0.5f

#define IMPACT_LIFETIME_MIN 0.0f
#define IMPACT_LIFETIME_MAX 50.0f

#define IMPACT_SIZE_X_DEFAULT 5.0f
#define IMPACT_SIZE_Y_DEFAULT 5.0f
#define IMPACT_SIZE_MIN 0.0f
#define IMPACT_SIZE_MAX 10.0f

#define IMPACT_EMISSIVE_DEFAULT 4.0f
#define IMPACT_EMISSIVE_MIN 0.0f
#define IMPACT_EMISSIVE_MAX 30.0f

#define IMPACT_ALPHA_DEFAULT 1.0f
#define IMPACT_ALPHA_MIN 0.0f
#define IMPACT_ALPHA_MAX 10.0f

#define IMPACT_COLOR_R_DEFAULT 252.f
#define IMPACT_COLOR_G_DEFAULT 122.f
#define IMPACT_COLOR_B_DEFAULT 20.f

#define IMPACT_TEXTUREINDEX_DEFAULT 20.f

// PARTICLE

#define PARTICLE_SIZE_X_DEFAULT 5.f
#define PARTICLE_SIZE_Y_DEFAULT 5.f
#define PARTICLE_SIZE_MIN 0.0f
#define PARTICLE_SIZE_MAX 10.0f

#define SLASH_SIZE_MIN 0.0f
#define SLASH_SIZE_MAX 100.0f

#define PARTICLE_ALPHA_DEFAULT 1.f
#define PARTICLE_ALPHA_MIN 0.0f
#define PARTICLE_ALPHA_MAX 10.0f

#define PARTICLE_LIFETIME_DEFAULT 1.f
#define PARTICLE_LIFETIME_MIN 0.0f
#define PARTICLE_LIFETIME_MAX 10.0f

#define PARTICLE_COUNT_DEFAULT 30
#define PARTICLE_COUNT_MIN 0
#define PARTICLE_COUNT_MAX 3000

#define PARTICLE_SPEED_DEFAULT 40
#define PARTICLE_SPEED_MIN 0.0f
#define PARTICLE_SPEED_MAX 200.0f

#define PARTICLE_COLOR_R_DEFAULT 223.f
#define PARTICLE_COLOR_G_DEFAULT 178.f
#define PARTICLE_COLOR_B_DEFAULT 31.f

#define PARTICLE_EMISSIVE_DEFAULT 15.f

#define PARTICLE_ROTATE_FACTOR_DEFAULT TRUE

#define PARTICLE_NOISESTRENGTH 1.f

#define PARTICLE_TEXTUREINDEX_DEFAULT 20.f

// SLASHHIT
#define SLASHHIT_SIZE_X_DEFAULT 0.7f
#define SLASHHIT_SIZE_Y_DEFAULT 15.f

#define SLASHHIT_ALPHA_DEFAULT 1.f

#define SLASHHIT_LIFETIME_DEFAULT 0.5f

#define SLASHHIT_COLOR_R_DEFAULT 255.f
#define SLASHHIT_COLOR_G_DEFAULT 118.f
#define SLASHHIT_COLOR_B_DEFAULT 0.f

#define SLASHHIT_EMISSIVE_DEFAULT 10.f

#define SLASHHIT_TEXTUREINDEX_DEFAULT 0

// TRAIL
#define TRAIL_CURVES_NUMBER_DEFAULT 4

#define TRAIL_MAINTEXTUREINDEX_DEFAULT 0
#define TRAIL_NOISETEXTUREINDEX_DEFAULT 0

#define TRAIL_EMISSIVE_DEFAULT 20.f

#define TRAIL_R_CURVEPOINT {0.f, 0.329105049f, 0.345385194f, 1.89199996}
#define TRAIL_G_CURVEPOINT {0.f, 0.459346294f, 0.198863804f, 0.595000029f}
#define TRAIL_B_CURVEPOINT {0.f, 0.256999999f, 0.679128408f, 0.f}

//DAMAGE_ANIMATION

#define DAMAGE_ANIMATION_DISTANCE_DEFAULT 1.0f
#define DAMAGE_ANIMATION_DISTANCE_MIN 0.0f
#define DAMAGE_ANIMATION_DISTANCE_MAX 10.0f

#define DAMAGE_ANIMATION_SPEED_DEFAULT 200.0f
#define DAMAGE_ANIMATION_SPEED_MIN 0.0f
#define DAMAGE_ANIMATION_SPEED_MAX 200.0f

//SHAKE_ANIMATION

#define SHAKE_ANIMATION_DURATION_DEFAULT 0.8f
#define SHAKE_ANIMATION_DURATION_MIN 0.0f
#define SHAKE_ANIMATION_DURATION_MAX 1.0f

#define SHAKE_ANIMATION_DISTANCE_DEFAULT 0.1f
#define SHAKE_ANIMATION_DISTANCE_MIN 0.0f
#define SHAKE_ANIMATION_DISTANCE_MAX 1.0f

#define SHAKE_ANIMATION_FREQUENCY_DEFAULT 0.01f
#define SHAKE_ANIMATION_FREQUENCY_MIN 0.0f
#define SHAKE_ANIMATION_FREQUENCY_MAX 1.0f

//STUN_ANIMATION

#define STUN_ANIMATION_STUNTIME_DEFAULT 0.4f
#define STUN_ANIMATION_STUNTIME_MIN 0.0f
#define STUN_ANIMATION_STUNTIME_MAX 1.0f

//HIT_LAG

#define HIT_LAG_MAXTIME_DEFAULT 0.5f
#define HIT_LAG_MAXTIME_MIN 0.0f
#define HIT_LAG_MAXTIME_MAX 1.0f

#define HIT_LAG_DURATION_DEFAULT 0.5f
#define HIT_LAG_DURATION_MIN 0.0f
#define HIT_LAG_DURATION_MAX 1.0f

#define HIT_LAG_SCALEWEIGHT_DEFAULT 0.5f
#define HIT_LAG_SCALEWEIGHT_MIN 0.1f
#define HIT_LAG_SCALEWEIGHT_MAX 1.0f

// CAMERA_MOVE
#define CAMERA_MOVE_DISTANCE_DEFAULT 0.6f
#define CAMERA_MOVE_DISTANCE_MIN 0.0f
#define CAMERA_MOVE_DISTANCE_MAX 10.0f

#define CAMERA_MOVE_TIME_DEFAULT 0.1f
#define CAMERA_MOVE_TIME_MIN 0.0f
#define CAMERA_MOVE_TIME_MAX 10.0f

#define CAMERA_MOVE_ROLLBACKTIME_DEFAULT 0.05f
#define CAMERA_MOVE_ROLLBACKTIME_MIN 0.0f
#define CAMERA_MOVE_ROLLBACKTIME_MAX 10.0f

// CAMERA_SHAKE
#define CAMERA_SHAKE_MAGNITUDE_DEFAULT 0.5f
#define CAMERA_SHAKE_MAGNITUDE_MIN 0.0f
#define CAMERA_SHAKE_MAGNITUDE_MAX 10.0f

#define CAMERA_SHAKE_DURATION_DEFAULT 0.5f
#define CAMERA_SHAKE_DURATION_MIN 0.0f
#define CAMERA_SHAKE_DURATION_MAX 10.0f

#define CAMERA_SHAKE_FREQUENCY_DEFAULT 0.015f
#define CAMERA_SHAKE_FREQUENCY_MIN 0.0f
#define CAMERA_SHAKE_FREQUENCY_MAX 10.0f

// CAMERA_ZOOM
#define CAMERA_ZOOMINOUT_DISTANCE_DEFAULT 1.0f
#define CAMERA_ZOOMINOUT_DISTANCE_MIN 0.0f
#define CAMERA_ZOOMINOUT_DISTANCE_MAX 10.0f

#define CAMERA_ZOOMINOUT_TIME_DEFAULT 0.08f
#define CAMERA_ZOOMINOUT_TIME_MIN 0.0f
#define CAMERA_ZOOMINOUT_TIME_MAX 10.0f

#define CAMERA_ZOOMINOUT_ROLLBACKTIME_DEFAULT 0.02f
#define CAMERA_ZOOMINOUT_ROLLBACKTIME_MIN 0.0f
#define CAMERA_ZOOMINOUT_ROLLBACKTIME_MAX 10.0f

// SHOCK_SOUND
#define SHOCK_SOUND_DELAY_DEFAULT 0.0f
#define SHOCK_SOUND_DELAY_MIN 0.0f
#define SHOCK_SOUND_DELAY_MAX 10.0f

#define SHOCK_SOUND_VOLUME_DEFAULT 0.4f
#define SHOCK_SOUND_VOLUME_MIN 0.0f
#define SHOCK_SOUND_VOLUME_MAX 10.0f

#define SHOCK_SOUND_NUMBER_DEFAULT 8

// SHOOTING_SOUND
#define SHOOTING_SOUND_DELAY_DEFAULT 0.2f
#define SHOOTING_SOUND_DELAY_MIN 0.0f
#define SHOOTING_SOUND_DELAY_MAX 10.0f

#define SHOOTING_SOUND_VOLUME_DEFAULT 1.1f
#define SHOOTING_SOUND_VOLUME_MIN 0.0f
#define SHOOTING_SOUND_VOLUME_MAX 10.0f

#define SHOOTING_SOUND_NUMBER_DEFAULT 2

// MOAN_SOUND
#define GOBLIN_MOAN_SOUND_DELAY_DEFAULT 0.f
#define ORC_MOAN_SOUND_DELAY_DEFAULT 0.f
#define SKELETON_MOAN_SOUND_DELAY_DEFAULT 0.f
#define MOAN_SOUND_DELAY_MIN 0.0f
#define MOAN_SOUND_DELAY_MAX 10.0f

#define GOBLIN_MOAN_SOUND_VOLUME_DEFAULT 0.75f
#define ORC_MOAN_SOUND_VOLUME_DEFAULT 0.75f
#define SKELETON_MOAN_SOUND_VOLUME_DEFAULT 0.75f
#define MOAN_SOUND_VOLUME_MIN 0.0f
#define MOAN_SOUND_VOLUME_MAX 10.0f

#define GOBLIN_MOAN_SOUND_NUMBER_DEFAULT 0
#define ORC_MOAN_SOUND_NUMBER_DEFAULT 3
#define SKELETON_MOAN_SOUND_NUMBER_DEFAULT 0






