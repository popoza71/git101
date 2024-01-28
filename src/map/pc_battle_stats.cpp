#include "pc_battle_stats.hpp"

#include "../common/nullpo.hpp"
#include "../common/socket.hpp"
#include "atcommand.hpp"
#include "clif.hpp"
#include "pc.hpp"

#include <cstring>

#define CHECK_STATUS_MENU_COUNT 4 // ��Ѻ�� 3 ��ҵ�ͧ��������� Zeny
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

	clif_displaymessage(fd, "============ ʶҹм���������� ============");
	battlestats_msg(sprintf(output, "����: %s (%s) Lv. %d", sd->status.name, job_name(sd->status.class_), sd->status.base_level));
	clif_displaymessage(fd, "|--- �൵�� ---|");
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

	clif_displaymessage(fd, "|--- ⺹�����ա���Ҿ ---|");
	if (sd->right_weapon.addsize[SZ_SMALL] + sd->left_weapon.addsize[SZ_SMALL]) {
		battlestats_msg(sprintf(output, "   -> ���բ�Ҵ���: %d%%", sd->right_weapon.addsize[SZ_SMALL] + sd->left_weapon.addsize[SZ_SMALL]));
	}
	if (sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> ���բ�Ҵ��ҧ: %d%%", sd->right_weapon.addsize[SZ_MEDIUM] + sd->left_weapon.addsize[SZ_MEDIUM]));
	}
	if (sd->right_weapon.addsize[SZ_BIG] + sd->left_weapon.addsize[SZ_BIG]) {
		battlestats_msg(sprintf(output, "   -> ���շء�˭�: %d%%", sd->right_weapon.addsize[SZ_BIG] + sd->left_weapon.addsize[SZ_BIG]));
	}
	if (sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> ���շء��Ҵ: %d%%", sd->right_weapon.addsize[SZ_ALL] + sd->left_weapon.addsize[SZ_ALL]));
	}
	if (sd->right_weapon.addrace[RC_PLAYER_HUMAN] + sd->left_weapon.addrace[RC_PLAYER_HUMAN]) {
		battlestats_msg(sprintf(output, "   -> ���ռ����蹻�����������: %d%%", sd->right_weapon.addrace[RC_PLAYER_HUMAN] + sd->left_weapon.addrace[RC_PLAYER_HUMAN]));
	}
	if (sd->right_weapon.addrace[RC_DEMIHUMAN] + sd->left_weapon.addrace[RC_DEMIHUMAN]) {
		battlestats_msg(sprintf(output, "   -> ���ջ�����������: %d%%", sd->right_weapon.addrace[RC_DEMIHUMAN] + sd->left_weapon.addrace[RC_DEMIHUMAN]));
	}
	if (sd->right_weapon.addrace[RC_ANGEL] + sd->left_weapon.addrace[RC_ANGEL]) {
		battlestats_msg(sprintf(output, "   -> ���ջ������Ƿٵ: %d%%", sd->right_weapon.addrace[RC_ANGEL] + sd->left_weapon.addrace[RC_ANGEL]));
	}
	if (sd->right_weapon.addrace[RC_BRUTE] + sd->left_weapon.addrace[RC_BRUTE]) {
		battlestats_msg(sprintf(output, "   -> ���ջ������ѵ��: %d%%", sd->right_weapon.addrace[RC_BRUTE] + sd->left_weapon.addrace[RC_BRUTE]));
	}
	if (sd->right_weapon.addrace[RC_DEMON] + sd->left_weapon.addrace[RC_DEMON]) {
		battlestats_msg(sprintf(output, "   -> ���ջ��������Ҩ: %d%%", sd->right_weapon.addrace[RC_DEMON] + sd->left_weapon.addrace[RC_DEMON]));
	}
	if (sd->right_weapon.addrace[RC_DRAGON] + sd->left_weapon.addrace[RC_DRAGON]) {
		battlestats_msg(sprintf(output, "   -> ���ջ������ѧ��: %d%%", sd->right_weapon.addrace[RC_DRAGON] + sd->left_weapon.addrace[RC_DRAGON]));
	}
	if (sd->right_weapon.addrace[RC_FISH] + sd->left_weapon.addrace[RC_FISH]) {
		battlestats_msg(sprintf(output, "   -> ���ջ��������: %d%%", sd->right_weapon.addrace[RC_FISH] + sd->left_weapon.addrace[RC_FISH]));
	}
	if (sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]) {
		battlestats_msg(sprintf(output, "   -> ���ջ���������ٻ��ҧ: %d%%", sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]));
	}
	if (sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]) {
		battlestats_msg(sprintf(output, "   -> ���ջ�������ŧ: %d%%", sd->right_weapon.addrace[RC_INSECT] + sd->left_weapon.addrace[RC_INSECT]));
	}
	if (sd->right_weapon.addrace[RC_PLANT] + sd->left_weapon.addrace[RC_PLANT]) {
		battlestats_msg(sprintf(output, "   -> ���ջ������ת: %d%%", sd->right_weapon.addrace[RC_PLANT] + sd->left_weapon.addrace[RC_PLANT]));
	}
	if (sd->right_weapon.addrace[RC_UNDEAD] + sd->left_weapon.addrace[RC_UNDEAD]) {
		battlestats_msg(sprintf(output, "   -> ���ջ������մԺ: %d%%", sd->right_weapon.addrace[RC_UNDEAD] + sd->left_weapon.addrace[RC_UNDEAD]));
	}
	if (sd->right_weapon.addrace[RC_PLAYER_DORAM] + sd->left_weapon.addrace[RC_PLAYER_DORAM]) {
		battlestats_msg(sprintf(output, "   -> ���ջ�����Doram: %d%%", sd->right_weapon.addrace[RC_PLAYER_DORAM] + sd->left_weapon.addrace[RC_PLAYER_DORAM]));
	}
	if (sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> ���շء������: %d%%", sd->right_weapon.addrace[RC_ALL] + sd->left_weapon.addrace[RC_ALL]));
	}
	if (sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> ���դ��ʻ���: %d%%", sd->right_weapon.addclass[CLASS_NORMAL] + sd->left_weapon.addclass[CLASS_NORMAL]));
	}
	if (sd->right_weapon.addclass[CLASS_BOSS] + sd->left_weapon.addclass[CLASS_BOSS]) {
		battlestats_msg(sprintf(output, "   -> ���դ��ʺ��: %d%%", sd->right_weapon.addclass[CLASS_BOSS] + sd->left_weapon.addclass[CLASS_BOSS]));
	}
	//PRE-RE
	if (sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> ���շء����: %d%%", sd->right_weapon.addclass[CLASS_ALL] + sd->left_weapon.addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- ⺹�������Ƿ�� ---|");
	if (sd->indexed_bonus.magic_addsize[SZ_SMALL]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ�좹Ҵ���: %d%%", sd->indexed_bonus.magic_addsize[SZ_SMALL]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_MEDIUM]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ�좹Ҵ��ҧ: %d%%", sd->indexed_bonus.magic_addsize[SZ_MEDIUM]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_BIG]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ�좹Ҵ�˭�: %d%%", sd->indexed_bonus.magic_addsize[SZ_BIG]));
	}
	if (sd->indexed_bonus.magic_addsize[SZ_ALL]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ��ء��Ҵ: %d%%", sd->indexed_bonus.magic_addsize[SZ_ALL]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_PLAYER_HUMAN]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ�������������: %d%%", sd->indexed_bonus.magic_addrace[RC_PLAYER_HUMAN]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_PLAYER_DORAM]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ�������Doram: %d%%", sd->indexed_bonus.magic_addrace[RC_PLAYER_DORAM]));
	}
	if (sd->indexed_bonus.magic_addrace[RC_ALL]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ��ء������: %d%%", sd->indexed_bonus.magic_addrace[RC_ALL]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_NORMAL]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ����ʻ���: %d%%", sd->indexed_bonus.magic_addclass[CLASS_NORMAL]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_BOSS]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ����ʺ��: %d%%", sd->indexed_bonus.magic_addclass[CLASS_BOSS]));
	}
	if (sd->indexed_bonus.magic_addclass[CLASS_ALL]) {
		battlestats_msg(sprintf(output, "   -> �����Ƿ��ء������: %d%%", sd->indexed_bonus.magic_addclass[CLASS_ALL]));
	}

	clif_displaymessage(fd, "|--- ��ͧ�ѹ�ҵ� ---|");
	struct {
		const char* format;
		int value;
	} ele_resist_info[] = {
		{ "   -> ���ҵ�: %d%%", sd->indexed_bonus.subele_script[ELE_NEUTRAL] },
		{ "   -> ���: %d%%", sd->indexed_bonus.subele_script[ELE_WATER] },
		{ "   -> �Թ: %d%%", sd->indexed_bonus.subele_script[ELE_EARTH] },
		{ "   -> �: %d%%", sd->indexed_bonus.subele_script[ELE_FIRE] },
		{ "   -> ��: %d%%", sd->indexed_bonus.subele_script[ELE_WIND] },
		{ "   -> ���: %d%%", sd->indexed_bonus.subele_script[ELE_POISON] },
		{ "   -> �ѡ����Է���: %d%%", sd->indexed_bonus.subele_script[ELE_HOLY] },
		{ "   -> �״: %d%%", sd->indexed_bonus.subele_script[ELE_DARK] },
		{ "   -> ��: %d%%", sd->indexed_bonus.subele_script[ELE_GHOST] },
		{ "   -> �մԺ: %d%%", sd->indexed_bonus.subele_script[ELE_UNDEAD] },
		{ "   -> �ء�ҵ�: %d%%", sd->indexed_bonus.subele_script[ELE_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(ele_resist_info); i++) {
		if (ele_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, ele_resist_info[i].format, ele_resist_info[i].value));
	}

	clif_displaymessage(fd, "|--- ��ͧ�ѹ������ ---|");
	struct {
		const char* format;
		int value;
	} race_resist_info[] = {
		{ "   -> ����ٻ��ҧ: %d%%", sd->indexed_bonus.subrace[RC_INSECT] },
		{ "   -> �մԺ: %d%%", sd->indexed_bonus.subrace[RC_UNDEAD] },
		{ "   -> �ѵ��: %d%%", sd->indexed_bonus.subrace[RC_BRUTE] },
		{ "   -> �ת: %d%%", sd->indexed_bonus.subrace[RC_PLANT] },
		{ "   -> ��ŧ: %d%%", sd->indexed_bonus.subrace[RC_INSECT] },
		{ "   -> ���: %d%%", sd->indexed_bonus.subrace[RC_FISH] },
		{ "   -> ���Ҩ: %d%%", sd->indexed_bonus.subrace[RC_DEMON] },
		{ "   -> ���������: %d%%", sd->indexed_bonus.subrace[RC_DEMIHUMAN] },
		{ "   -> �Ƿٵ: %d%%", sd->indexed_bonus.subrace[RC_ANGEL] },
		{ "   -> �ѧ��: %d%%", sd->indexed_bonus.subrace[RC_DRAGON] },
		{ "   -> ������(������): %d%%", sd->indexed_bonus.subrace[RC_PLAYER_HUMAN] },
		{ "   -> ������(Doram): %d%%", sd->indexed_bonus.subrace[RC_PLAYER_DORAM] },
		{ "   -> �ء������: %d%%", sd->indexed_bonus.subrace[RC_ALL] },
	};
	for (int i = 0; i < ARRAYLENGTH(race_resist_info); i++) {
		if (race_resist_info[i].value == 0)
			continue;
		battlestats_msg(sprintf(output, race_resist_info[i].format, race_resist_info[i].value));
	}

	clif_displaymessage(fd, "|--- ��ͧ�ѹ��Ҵ ---|");
	struct {
		const char* format;
		int value;
	} Size_resist_info[] = {
		{ "   -> ��Ҵ���: %d%%", sd->indexed_bonus.subsize[SZ_SMALL] },
		{ "   -> ��Ҵ��ҧ: %d%%", sd->indexed_bonus.subsize[SZ_MEDIUM] },
		{ "   -> ��Ҵ�˭�: %d%%", sd->indexed_bonus.subsize[SZ_BIG] },
		{ "   -> ��Ҵ������: %d%%", sd->indexed_bonus.subsize[SZ_ALL] },
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
		clif_displaymessage(sd->fd, "��辺������.");
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
