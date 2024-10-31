/*
Copyright (C) 2024 Frank Richter

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "q2proto/q2proto.h"

/*
 * Basic test for compile & link checks.
 * Will not do anything sensible!
 */

typedef q2proto_vec3_t vec3_t;
typedef float vec4_t[4];

// Based on Q2PRO entity state + extension
typedef struct {
    int     number;         // edict index

    vec3_t  origin;
    vec3_t  angles;
    vec3_t  old_origin;     // for lerping
    int     modelindex;
    int     modelindex2, modelindex3, modelindex4;  // weapons, CTF flags, etc
    int     frame;
    int     skinnum;
    uint64_t        effects;        // PGM - we're filling it, so it needs to be unsigned
    int     renderfx;
    int     solid;          // for client side prediction, 8*(bits 0-4) is x/y radius
                            // 8*(bits 5-9) is z down distance, 8(bits10-15) is z up
                            // gi.linkentity sets this properly
    int     sound;          // for looping sounds, to guarantee shutoff
    int     event;          // impulse events -- muzzle flashes, footsteps, etc
                            // events only go out for a single frame, they
                            // are automatically cleared each frame

    float       alpha;
    float       scale;
    float       loop_volume;
    float       loop_attenuation;
} entity_state_t;

#define Q2P_PACK_ENTITY_FUNCTION_NAME   PackEntity
#define Q2P_PACK_ENTITY_TYPE            entity_state_t*

#include "q2proto/q2proto_packing_entitystate_impl.inc"

typedef enum
{
    // can accelerate and turn
    PM_NORMAL,
    PM_SPECTATOR,
    // no acceleration or turning
    PM_DEAD,
    PM_GIB, // different bounding box
    PM_FREEZE
} pmtype_t;

// Q2PRO "new" player pmove
typedef struct {
    pmtype_t    pm_type;

    int32_t     origin[3];      // 19.3
    int32_t     velocity[3];    // 19.3
    uint16_t    pm_flags;       // ducked, jump_held, etc
    uint16_t    pm_time;        // in msec
    int16_t     gravity;
    int16_t     delta_angles[3];    // add to command angles to get view direction
                                    // changed by spawns, rotating objects, and teleporters
} pmove_state_new_t;

typedef struct {
    vec3_t color;
    float density;
    float sky_factor;
} player_fog_t;

typedef struct {
    vec3_t start_color;
    float start_dist;
    vec3_t end_color;
    float end_dist;
    float density;
    float falloff;
} player_heightfog_t;

// Slightly modified Q2PRO "new" player state
typedef struct {
    pmove_state_new_t   pmove;  // for prediction

    // these fields do not need to be communicated bit-precise

    vec3_t      viewangles;     // for fixed views
    vec3_t      viewoffset;     // add to pmovestate->origin
    vec3_t      kick_angles;    // add to view direction to get render angles
                                // set by weapon kicks, pain effects, etc

    vec3_t      gunangles;
    vec3_t      gunoffset;
    int         gunindex;
    int         gunskin;
    int         gunframe;
    int         reserved_1;
    int         reserved_2;

    vec4_t      blend;          // rgba full screen effect
    vec4_t      damage_blend;

    player_fog_t        fog;
    player_heightfog_t  heightfog;

    float       fov;            // horizontal field of view

    int         rdflags;        // refdef flags

    int         reserved_3;
    int         reserved_4;

    int16_t     stats[64];   // fast status bar updates
} player_state_t;

#define Q2P_PACK_PLAYER_FUNCTION_NAME   PackPlayer
#define Q2P_PACK_PLAYER_TYPE            player_state_t*

#include "q2proto/q2proto_packing_playerstate_impl.inc"

int main(int argc, char** argv)
{
    q2proto_clientcontext_t client_context;
    q2proto_init_clientcontext(&client_context);

    q2proto_server_info_t server_info = {0};
    q2proto_connect_t connect_info = {0};
    q2proto_servercontext_t server_context;
    q2proto_init_servercontext(&server_context, &server_info, &connect_info);

    return 0;
}
