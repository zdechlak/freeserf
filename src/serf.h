/* serf.h */

#ifndef _SERF_H
#define _SERF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "map.h"
#include "misc.h"
#include "log.h"

#define SERF_INDEX(ptr)    ((int)((ptr) - globals.serfs))

#define SERF_PLAYER(serf)  ((int)((serf)->type & 3))
#define SERF_TYPE(serf)    ((serf_type_t)(((serf)->type >> 2) & 0x1f))


#define serf_log_state_change(serf, new_state)	\
	LOGV("serf", "serf %i: state %s -> %s (%s:%i)", SERF_INDEX(serf), \
	     serf_get_state_name((serf)->state),			\
	     serf_get_state_name((new_state)), __FUNCTION__, __LINE__)


typedef enum {
	SERF_TRANSPORTER = 0,
	SERF_SAILOR,
	SERF_DIGGER,
	SERF_BUILDER,
	SERF_4,
	SERF_LUMBERJACK,
	SERF_SAWMILLER,
	SERF_STONECUTTER,
	SERF_FORESTER,
	SERF_MINER,
	SERF_SMELTER,
	SERF_FISHER,
	SERF_PIGFARMER,
	SERF_BUTCHER,
	SERF_FARMER,
	SERF_MILLER,
	SERF_BAKER,
	SERF_BOATBUILDER,
	SERF_TOOLMAKER,
	SERF_WEAPONSMITH,
	SERF_GEOLOGIST,
	SERF_GENERIC,
	SERF_KNIGHT_0,
	SERF_KNIGHT_1,
	SERF_KNIGHT_2,
	SERF_KNIGHT_3,
	SERF_KNIGHT_4
} serf_type_t;

typedef enum {
	SERF_STATE_NULL = 0,
	SERF_STATE_IDLE_IN_STOCK,
	SERF_STATE_WALKING,
	SERF_STATE_TRANSPORTING,
	SERF_STATE_ENTERING_BUILDING,
	SERF_STATE_LEAVING_BUILDING, /* 5 */
	SERF_STATE_READY_TO_ENTER,
	SERF_STATE_READY_TO_LEAVE,
	SERF_STATE_DIGGING,
	SERF_STATE_BUILDING,
	SERF_STATE_BUILDING_CASTLE, /* 10 */
	SERF_STATE_MOVE_RESOURCE_OUT,
	SERF_STATE_WAIT_FOR_RESOURCE_OUT,
	SERF_STATE_DROP_RESOURCE_OUT,
	SERF_STATE_DELIVERING,
	SERF_STATE_READY_TO_LEAVE_INVENTORY, /* 15 */
	SERF_STATE_FREE_WALKING,
	SERF_STATE_LOGGING,
	SERF_STATE_PLANNING_LOGGING,
	SERF_STATE_PLANNING_PLANTING,
	SERF_STATE_PLANTING, /* 20 */
	SERF_STATE_PLANNING_STONECUTTING,
	SERF_STATE_22, /* similar to free walking but specifically for stonecutter */
	SERF_STATE_STONECUTTING,
	SERF_STATE_SAWING,
	SERF_STATE_LOST, /* 25 */
	SERF_STATE_26,
	SERF_STATE_27,
	SERF_STATE_ESCAPE_BUILDING,
	SERF_STATE_MINING,
	SERF_STATE_SMELTING, /* 30 */
	SERF_STATE_PLANNING_FISHING,
	SERF_STATE_FISHING,
	SERF_STATE_PLANNING_FARMING,
	SERF_STATE_FARMING,
	SERF_STATE_MILLING, /* 35 */
	SERF_STATE_BAKING,
	SERF_STATE_PIGFARMING,
	SERF_STATE_BUTCHERING,
	SERF_STATE_MAKING_WEAPON,
	SERF_STATE_MAKING_TOOL, /* 40 */
	SERF_STATE_BUILDING_BOAT,
	SERF_STATE_LOOKING_FOR_GEO_SPOT,
	SERF_STATE_SAMPLING_GEO_SPOT,
	SERF_STATE_KNIGHT_ENGAGING_BUILDING,
	SERF_STATE_KNIGHT_PREPARE_ATTACKING, /* 45 */
	SERF_STATE_KNIGHT_LEAVE_FOR_FIGHT,
	SERF_STATE_KNIGHT_PREPARE_DEFENDING,
	SERF_STATE_KNIGHT_ATTACKING,
	SERF_STATE_KNIGHT_DEFENDING,
	SERF_STATE_KNIGHT_ATTACKING_VICTORY, /* 50 */
	SERF_STATE_KNIGHT_ATTACKING_DEFEAT,
	SERF_STATE_KNIGHT_OCCUPY_ENEMY_BUILDING,
	SERF_STATE_KNIGHT_FREE_WALKING,
	SERF_STATE_54,
	SERF_STATE_55, /* 55 */
	SERF_STATE_56,
	SERF_STATE_57,
	SERF_STATE_58,
	SERF_STATE_59,
	SERF_STATE_60, /* 60 */
	SERF_STATE_61,
	SERF_STATE_62,
	SERF_STATE_63,
	SERF_STATE_64,
	SERF_STATE_KNIGHT_LEAVE_FOR_WALK_TO_FIGHT, /* 65 */
	SERF_STATE_IDLE_ON_PATH,
	SERF_STATE_WAIT_IDLE_ON_PATH,
	SERF_STATE_WAKE_AT_FLAG,
	SERF_STATE_WAKE_ON_PATH,
	SERF_STATE_DEFENDING_HUT, /* 70 */
	SERF_STATE_DEFENDING_TOWER,
	SERF_STATE_DEFENDING_FORTRESS,
	SERF_STATE_73,
	SERF_STATE_FINISHED_BUILDING,
	SERF_STATE_DEFENDING_CASTLE /* 75 */
} serf_state_t;


typedef struct {
	int type;
	int animation; /* Index to animation table in data file. */
	int counter;
	map_pos_t pos;
	uint16_t anim;
	serf_state_t state;

	union {
		struct {
			uint inv_index; /* E */
		} idle_in_stock;

		/* States: walking, transporting, delivering */
		/* res: resource carried (when transporting),
		   otherwise direction. */
		struct {
			int res; /* B */
			uint dest; /* C */
			int dir; /* E */
			int wait_counter; /* F */
		} walking;

		struct {
			int field_B; /* B */
			uint slope_len; /* C */
		} entering_building;

		/* States: leaving_building, ready_to_leave */
		struct {
			int field_B; /* B */
			int dest; /* C */
			int dest2; /* D */
			int dir; /* E */
			serf_state_t next_state; /* F */
		} leaving_building;

		struct {
			int field_B; /* B */
		} ready_to_enter;

		struct {
			int h_index; /* B */
			uint target_h; /* C */
			int dig_pos; /* D */
			int substate; /* E */
		} digging;

		/* mode: one of three substates (negative, positive, zero).
		   bld_index: index of building. */
		struct {
			int mode; /* B */
			uint bld_index; /* C */
			uint material_step; /* E */
			uint counter; /* F */
		} building;

		struct {
			uint inv_index; /* C */
		} building_castle;

		/* States: move_resource_out, wait_for_resource_out,
		   drop_resource_out */
		struct {
			uint res; /* B */
			uint res_dest; /* C */
			serf_state_t next_state; /* F */
		} move_resource_out;

		struct {
			int mode; /* B */
			uint dest; /* C */
			uint inv_index; /* E */
		} ready_to_leave_inventory;

		/* States: free_walking, logging,
		   planting, stonecutting, fishing,
		   farming, sampling_geo_spot,
		   knight_free_walking */
		struct {
			int dist1; /* B */
			int dist2; /* C */
			int neg_dist1; /* D */
			int neg_dist2; /* E */
			int flags; /* F */
		} free_walking;

		/* No state data: planning_logging,
		   planning_planting, planning_stonecutting */

		struct {
			int mode; /* B */
		} sawing;

		struct {
			int field_B; /* B */
		} lost;

		struct {
			uint substate; /* B */
			uint res; /* D */
			ground_deposit_t deposit; /* E */
		} mining;

		/* type: Type of smelter (0 is steel, else gold). */
		struct {
			int mode; /* B */
			int counter; /* C */
			uint type; /* D */
		} smelting;

		/* No state data: planning_fishing,
		   planning_farming */

		struct {
			int mode; /* B */
		} milling;

		struct {
			int mode; /* B */
		} baking;

		struct {
			int mode; /* B */
		} pigfarming;

		struct {
			int mode; /* B */
		} butchering;

		struct {
			int mode; /* B */
		} making_weapon;

		struct {
			int mode; /* B */
		} making_tool;

		struct {
			int mode; /* B */
		} building_boat;

		/* No state data: looking_for_geo_spot */

		/* States: knight_engaging_building,
		   knight_prepare_attacking, ... */
		struct {
			int field_B; /* B */
			int field_C; /* C */
			int field_D; /* D */
			int def_index; /* E */
		} attacking;

		struct {
			int dist_col; /* B */
			int dist_row; /* C */
			int field_D; /* D */
			int field_E; /* E */
			serf_state_t next_state; /* F */
		} leave_for_walk_to_fight;

		/* States: idle_on_path, wait_idle_on_path,
		   wake_at_flag, wake_on_path. */
		struct {
			int rev_dir; /* B */
			struct flag *flag; /* C */
			int field_E; /* E */
		} idle_on_path;

		/* No state data: finished_building */

		/* States: defending_hut, defending_tower,
		   defending_fortress, defending_castle */
		struct {
			int next_knight; /* E */
		} defending;
	} s;
} serf_t;


serf_t *get_serf(int index); /* in freeserf.c */

void update_serf(serf_t *serf);
const char *serf_get_state_name(serf_state_t state);

#endif /* ! _SERF_H */
