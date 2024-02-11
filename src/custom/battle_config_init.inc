// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom battle config initializations
 * Place the definition of custom battle configuration initializations in this file.
 *
 * Do not forget to add a comma at the end of the line, if you want to add another config
 *
 * Format:
 *	 { "name", &battle_config.<variable name>, <default value>, <minimum value>, <maximum value> },
 **/

{ "party_bonus_system_enable",		&battle_config.party_bonus_system_enable,		0,	0,	1,			},
{ "party_bonus_same_map_minimum",	&battle_config.party_bonus_same_map_minimum,	1,	0,	MAX_PARTY,	},
