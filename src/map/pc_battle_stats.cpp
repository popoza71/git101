#include "pc_battle_stats.hpp"

#include "../common/nullpo.hpp"
#include "../common/socket.hpp"
#include "atcommand.hpp"
#include "clif.hpp"
#include "pc.hpp"

#include <cstring>

#define CHECK_STATUS_MENU_COUNT 4 // ª√—∫‡ªÁπ 3 ∂È“µÈÕß°“√„™È‡¡πŸ Zeny
#define CS_EQUIPMENT 785
#define CS_STATS 786
#define CS_ZENY 787
#define CS_ITEMLIST 788

/// Check Equip
/// 0442 <Length>.W <count>.L <Skill_list>.W (ZC_SKILL_SELECT_REQUEST).
int pcb_display_menu(struct map_session_data* sd) {
	int i;
	int fd;
	unsigned short skills[4];
	memset(skills, 0, sizeof(skills));
	skills[0] = CS_EQUIPMENT;
	skills[1] = CS_STATS;
	skills[2] = CS_ZENY;
	skills[3] = CS_ITEMLIST;

	nullpo_ret(sd);

	fd = sd->fd;

	if (!fd)
		return 0;

	WFIFOHEAD(fd, 8 + CHECK_STATUS_MENU_COUNT * 2);
	WFIFOW(fd, 0) = 0x442;

	for (i = 0; i < CHECK_STATUS_MENU_COUNT; i++) {
		WFIFOW(fd, 8 + i * 2) = skills[i];
	}

	WFIFOW(fd, 2) = 8 + CHECK_STATUS_MENU_COUNT * 2;
	WFIFOL(fd, 4) = 3;
	WFIFOSET(fd, WFIFOW(fd, 2));

	sd->menuskill_id = SC_AUTOSHADOWSPELL;
	sd->menuskill_val = 3;

	sd->state.check_equip_skill = true;
	sd->state.workinprogress = WIP_DISABLE_ALL;

	return 1;
}

void pcb_display(int fd, struct map_session_data* sd) {
	status_data* status = status_get_status_data(&sd->bl);
	char output[CHAT_SIZE_MAX];

#define battlestats_msg(__msg__) (__msg__); clif_displaymessage(fd, output)

	clif_displaymessage(fd, "============  ∂“π–ºŸÈ‡≈Ëπ‚¥¬√«¡ ============");
	battlestats_msg(sprintf(output, "™◊ËÕ: %s (%s) Lv. %d", sd->status.name, job_name(sd->status.class_), sd->status.base_level));
	clif_displaymessage(fd, "|---  ‡µµ—  ---|");
	battlestats_msg(sprintf(output, "   -> Hp: %d", sd->status.hp));
	battlestats_msg(sprintf(output, "   -> Maxhp: %d", sd->status.max_hp));
	battlestats_msg(sprintf(output, "   -> Sp: %d", sd->status.sp));
	battlestats_msg(sprintf(output, "   -> Maxsp: %d", sd->status.max_sp));
	battlestats_msg(sprintf(output, "   -> Str: %d + %d", sd->status.str, sd->battle_status.str - sd->status.str));
	battlestats_msg(sprintf(output, "   -> agi: %d + %d", sd->status.agi, sd->battle_status.agi - sd->status.agi));
	battlestats_msg(sprintf(output, "   -> Vit: %d + %d", sd->status.vit, sd->battle_status.vit - sd->status.vit));
	battlestats_msg(sprintf(output, "   -> Int: %d + %d", sd->status.int_, sd->battle_status.int_ - sd->status.int_));
	battlestats_msg(sprintf(output, "   -> Dex: %d + %d", sd->status.dex, sd->battle_status.dex - sd->status.dex));
	battlestats_msg(sprintf(output, "   -> Luk: %d + %d", sd->status.luk, sd->battle_status.luk - sd->status.luk));
	battlestats_msg(sprintf(output, "   -> ATK: %d + %d", pc_leftside_atk(sd), pc_rightside_atk(sd)));
	battlestats_msg(sprintf(output, "   -> MATK: %d + %d", pc_leftside_matk(sd), pc_rightside_matk(sd)));
	battlestats_msg(sprintf(output, "   -> HIT: %d", status->hit));
	battlestats_msg(sprintf(output, "   -> CRIT: %d", status->cri / 10));
	battlestats_msg(sprintf(output, "   -> DEF: %d + %d", pc_leftside_def(sd), pc_rightside_def(sd)));
	battlestats_msg(sprintf(output, "   -> MDEF: %d + %d", pc_leftside_mdef(sd), pc_rightside_mdef(sd)));
	battlestats_msg(sprintf(output, "   -> FLEE: %d + %d", status->flee, status->flee2 / 10));
	battlestats_msg(sprintf(output, "   -> ASPD: %d", (2000 - status->amotion) / 10));
	battlestats_msg(sprintf(output, "   -> EXP +: %d%%", sd->indexed_bonus.expaddrace[status->race] + sd->indexed_bonus.expaddrace[RC_ALL] + sd->indexed_bonus.expaddclass[status->class_] + sd->indexed_bonus.expaddclass[CLASS_ALL]));
	battlestats_msg(sprintf(output, "   -> Drop +: %d%%", sd->indexed_bonus.dropaddclass[CLASS_ALL] + sd->indexed_bonus.dropaddrace[RC_ALL]));

	clif_displaymessage(fd, "|--- ‚∫π— ‚®¡µ’°“¬¿“æ ---|");
	if (sd->right_weapon.addsize[SZ_SMALL] + sd->left_weapon.addsize[SZ_SMALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’¢π“¥‡≈Á°: %d%%", sd->right_weapon.addsize[SZ_SMALL] + sd->left_weapon.addsize[SZ_SMALL]));
	}
	if (sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’¢π“¥°≈“ß: %d%%", sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]));
	}
	if (sd->right_weapon.addsize[SZ_BIG] + sd->left_weapon.addsize[SZ_BIG]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’∑ÿ°„À≠Ë: %d%%", sd->right_weapon.addsize[SZ_BIG] + sd->left_weapon.addsize[SZ_BIG]));
	}
	if (sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’∑ÿ°¢π“¥: %d%%", sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]));
	}
	if (sd->right_weapon.addrace[RC_PLAYER_HUMAN] + sd->left_weapon.addrace[RC_PLAYER_HUMAN]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ºŸÈ‡≈Ëπª√–‡¿∑¡πÿ…¬Ï: %d%%", sd->right_weapon.addrace[RC_PLAYER_HUMAN] + sd->left_weapon.addrace[RC_PLAYER_HUMAN]));
	}
	if (sd->right_weapon.addrace[RC_DEMIHUMAN] + sd->left_weapon.addrace[RC_DEMIHUMAN]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑¡πÿ…¬Ï: %d%%", sd->right_weapon.addrace[RC_DEMIHUMAN] + sd->left_weapon.addrace[RC_DEMIHUMAN]));
	}
	if (sd->right_weapon.addrace[RC_ANGEL] + sd->left_weapon.addrace[RC_ANGEL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑‡∑«∑Ÿµ: %d%%", sd->right_weapon.addrace[RC_ANGEL] + sd->left_weapon.addrace[RC_ANGEL]));
	}
	if (sd->right_weapon.addrace[RC_BRUTE] + sd->left_weapon.addrace[RC_BRUTE]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑ —µ«Ï: %d%%", sd->right_weapon.addrace[RC_BRUTE] + sd->left_weapon.addrace[RC_BRUTE]));
	}
	if (sd->right_weapon.addrace[RC_DEMON] + sd->left_weapon.addrace[RC_DEMON]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑ª’»“®: %d%%", sd->right_weapon.addrace[RC_DEMON] + sd->left_weapon.addrace[RC_DEMON]));
	}
	if (sd->right_weapon.addrace[RC_DRAGON] + sd->left_weapon.addrace[RC_DRAGON]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑¡—ß°√: %d%%", sd->right_weapon.addrace[RC_DRAGON] + sd->left_weapon.addrace[RC_DRAGON]));
	}
	if (sd->right_weapon.addrace[RC_FISH] + sd->left_weapon.addrace[RC_FISH]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑ª≈“: %d%%", sd->right_weapon.addrace[RC_FISH] + sd->left_weapon.addrace[RC_FISH]));
	}
	if (sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑‰√È√Ÿª√Ë“ß: %d%%", sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]));
	}
	if (sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑·¡≈ß: %d%%", sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]));
	}
	if (sd->right_weapon.addrace[RC_PLANT] + sd->left_weapon.addrace[RC_PLANT]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑æ◊™: %d%%", sd->right_weapon.addrace[RC_PLANT] + sd->left_weapon.addrace[RC_PLANT]));
	}
	if (sd->right_weapon.addrace[RC_UNDEAD] + sd->left_weapon.addrace[RC_UNDEAD]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑º’¥‘∫: %d%%", sd->right_weapon.addrace[RC_UNDEAD] + sd->left_weapon.addrace[RC_UNDEAD]));
	}
	if (sd->right_weapon.addrace[RC_PLAYER_DORAM] + sd->left_weapon.addrace[RC_PLAYER_DORAM]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’ª√–‡¿∑Doram: %d%%", sd->right_weapon.addrace[RC_PLAYER_DORAM] + sd->left_weapon.addrace[RC_PLAYER_DORAM]));
	}
	if (sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’∑ÿ°ª√–‡¿∑: %d%%", sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]));
	}
	if (sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’§≈“ ª°µ‘: %d%%", sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]));
	}
	if (sd->right_weapon.addclass[CLASS_BOSS] + sd->left_weapon.addclass[CLASS_BOSS]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’§≈“ ∫Õ : %d%%", sd->right_weapon.addclass[CLASS_BOSS] + sd->left_weapon.addclass[CLASS_BOSS]));
	}
	//PRE-RE
	if (sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’∑ÿ°§≈“ : %d%%", sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- ‚∫π— ‚®¡µ’‡«∑¬Ï ---|");
	if (sd->indexed_bonus.magic_addsize[SZ_SMALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï¢π“¥‡≈Á°: %d%%", sd->indexed_bonus.magic_addsize[SZ_SMALL]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï¢π“¥°≈“ß: %d%%", sd->indexed_bonus.magic_addsize[SZ_MEDIUM]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_BIG]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï¢π“¥„À≠Ë: %d%%", sd->indexed_bonus.magic_addsize[SZ_BIG]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï∑ÿ°¢π“¥: %d%%", sd->indexed_bonus.magic_addsize[SZ_ALL]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_PLAYER_HUMAN]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ïª√–‡¿∑¡πÿ…¬Ï: %d%%", sd->indexed_bonus.magic_addrace[RC_PLAYER_HUMAN]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_PLAYER_DORAM]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ïª√–‡¿∑Doram: %d%%", sd->indexed_bonus.magic_addrace[RC_PLAYER_DORAM]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï∑ÿ°ª√–‡¿∑: %d%%", sd->indexed_bonus.magic_addrace[RC_ALL]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï§≈“ ª°µ‘: %d%%", sd->indexed_bonus.magic_addclass[CLASS_NORMAL]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_BOSS]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï§≈“ ∫Õ : %d%%", sd->indexed_bonus.magic_addclass[CLASS_BOSS]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> ‚®¡µ’‡«∑¬Ï∑ÿ°ª√–‡¿∑: %d%%", sd->indexed_bonus.magic_addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- ªÈÕß°—π∏“µÿ ---|");
	struct {
		const char* format;
		int value;
	} ele_resist_info[] = {
		{ "   -> ‰√È∏“µÿ: %d%%", sd->indexed_bonus.subele_script[ELE_NEUTRAL] },
		{ "   -> πÈ”: %d%%", sd->indexed_bonus.subele_script[ELE_WATER] },
		{ "   -> ¥‘π: %d%%", sd->indexed_bonus.subele_script[ELE_EARTH] },
		{ "   -> ‰ø: %d%%", sd->indexed_bonus.subele_script[ELE_FIRE] },
		{ "   -> ≈¡: %d%%", sd->indexed_bonus.subele_script[ELE_WIND] },
		{ "   -> æ‘…: %d%%", sd->indexed_bonus.subele_script[ELE_POISON] },
		{ "   -> »—°¥‘Ï ‘∑∏‘Ï: %d%%", sd->indexed_bonus.subele_script[ELE_HOLY] },
		{ "   -> ¡◊¥: %d%%", sd->indexed_bonus.subele_script[ELE_DARK] },
		{ "   -> º’: %d%%", sd->indexed_bonus.subele_script[ELE_GHOST] },
		{ "   -> º’¥‘∫: %d%%", sd->indexed_bonus.subele_script[ELE_UNDEAD] },
		{ "   -> ∑ÿ°∏“µÿ: %d%%", sd->indexed_bonus.subele_script[ELE_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(ele_resist_info); i++) {
		if (ele_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, ele_resist_info[i].format, ele_resist_info[i].value));
	}

	clif_displaymessage(fd, "|--- ªÈÕß°—πª√–‡¿∑ ---|");
	struct {
		const char* format;
		int value;
	} race_resist_info[] = {
		{ "   -> ‰√È√Ÿª√Ë“ß: %d%%", sd->indexed_bonus.subrace[RC_INSECT] },
		{ "   -> º’¥‘∫: %d%%", sd->indexed_bonus.subrace[RC_UNDEAD] },
		{ "   ->  —µ«Ï: %d%%", sd->indexed_bonus.subrace[RC_BRUTE] },
		{ "   -> æ◊™: %d%%", sd->indexed_bonus.subrace[RC_PLANT] },
		{ "   -> ·¡≈ß: %d%%", sd->indexed_bonus.subrace[RC_INSECT] },
		{ "   -> ª≈“: %d%%", sd->indexed_bonus.subrace[RC_FISH] },
		{ "   -> ª’»“®: %d%%", sd->indexed_bonus.subrace[RC_DEMON] },
		{ "   -> °÷Ëß¡πÿ…¬Ï: %d%%", sd->indexed_bonus.subrace[RC_DEMIHUMAN] },
		{ "   -> ‡∑«∑Ÿµ: %d%%", sd->indexed_bonus.subrace[RC_ANGEL] },
		{ "   -> ¡—ß°√: %d%%", sd->indexed_bonus.subrace[RC_DRAGON] },
		{ "   -> ºŸÈ‡≈Ëπ(¡πÿ…¬Ï): %d%%", sd->indexed_bonus.subrace[RC_PLAYER_HUMAN] },
		{ "   -> ºŸÈ‡≈Ëπ(Doram): %d%%", sd->indexed_bonus.subrace[RC_PLAYER_DORAM] },
		{ "   -> ∑ÿ°ª√–‡¿∑: %d%%", sd->indexed_bonus.subrace[RC_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(race_resist_info); i++) {
		if (race_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, race_resist_info[i].format, race_resist_info[i].value));
	}

	clif_displaymessage(fd, "|--- ªÈÕß°—π¢π“¥ ---|");
	struct {
		const char* format;
		int value;
	} Size_resist_info[] = {
		{ "   -> ¢π“¥‡≈Á°: %d%%", sd->indexed_bonus.subsize[SZ_SMALL] },
		{ "   -> ¢π“¥°≈“ß: %d%%", sd->indexed_bonus.subsize[SZ_MEDIUM] },
		{ "   -> ¢π“¥„À≠Ë: %d%%", sd->indexed_bonus.subsize[SZ_BIG] },
		{ "   -> ¢π“¥∑—ÈßÀ¡¥: %d%%", sd->indexed_bonus.subsize[SZ_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(Size_resist_info); i++) {
		if (Size_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, Size_resist_info[i].format, Size_resist_info[i].value));
	}

#undef battlestats_msg
}

bool pcb_process_selection(struct map_session_data* sd, int skill) {
	char output[CHAT_SIZE_MAX];
	struct map_session_data* tsd = map_id2sd(sd->ce_gid);

	sd->state.check_equip_skill = false;
	sd->state.workinprogress = WIP_DISABLE_NONE;
	clif_menuskill_clear(sd);

	if (!tsd) {
		clif_displaymessage(sd->fd, "‰¡Ëæ∫ºŸÈ‡≈Ëπ.");
		return false;
	}

	if (!(skill >= CS_EQUIPMENT && skill < CS_EQUIPMENT + CHECK_STATUS_MENU_COUNT)) // Fake zeny check packet will fail if disabled here
		return false;

	switch (skill) {
	case CS_EQUIPMENT:
		if (tsd->status.show_equip || pc_has_permission(sd, PC_PERM_VIEW_EQUIPMENT))
			clif_viewequip_ack(sd, tsd);
		else
			clif_msg(sd, VIEW_EQUIP_FAIL);
		return true;
	case CS_STATS:
		pcb_display(sd->fd, tsd);
		return true;
	case CS_ZENY:
		clif_messagecolor(&sd->bl, color_table[COLOR_CYAN], "============ Check Zeny Info ============", false, SELF);
		sprintf(output, "Zeny: %d", tsd->status.zeny);
		clif_messagecolor(&sd->bl, color_table[COLOR_CYAN], output, false, SELF);
		return true;
	case CS_ITEMLIST: {
		char command[CHAT_SIZE_MAX];
		safesnprintf(command, sizeof(command), "%citemlist %s", charcommand_symbol, tsd->status.name);
		is_atcommand(sd->fd, sd, command, 0);
		return true;
	}
	default:
		return false;
	}
	return false;
}

#undef CHECK_STATUS_MENU_COUNT
