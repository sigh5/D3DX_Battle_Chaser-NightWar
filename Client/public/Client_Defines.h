#pragma once

#include <Process.h>
#include <random>
#include <time.h>
#include <queue>
#include <set>

#define EPSILON 0.0001
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

namespace Client
{																			
	enum LEVEL {LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,LEVEL_COMBAT ,LEVEL_END };// 로딩오브젝트는 맨뒤에서 -1

	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };

	enum AnimType { ANIM_CHAR1, ANIM_CHAR2, ANIM_CHAR3, ANIM_CHAR4, ANIM_CHAR5, ANIM_CHAR6, ANIM_CHAR_END};
	
	enum ANIM_DIR { ANIM_DIR_STRAIGHT, ANIM_DIR_BACK, ANIM_EMD };
	
	enum UI_REPRESENT {
		REPRESENT_KNOLAN, REPRESENT_GARRISON, REPRESENT_CALIBRETTO,
		REPRESENT_SKELTON_NAKED, REPRESENT_SLIMEKING,REPRESENT_SPIDER_MANA,
		REPRESENT_END,
	};


	enum	BUTTON_STATE {
		BUTTON_STATE_ACTION, BUTTON_STATE_ABLILTY, BUTTON_STATE_ITEM, BUTTON_STATE_END
	};

	enum	BUTTON_FSM_STATE
	{
		BUTTON_FSM_NORMALATTACK, BUTTON_FSM_SKILL1, BUTTON_FSM_SKILL2,
		BUTTON_FSM_ULTIMATE, BUTTON_FSM_BUFF, BUTTON_FSM_WIDEBUFF,
		BUTTON_FSM_USEITEM, BUTTON_FSM_DEFENCE, BUTTON_FSM_FLEE,BUTTON_FSM_STATE_END,
	};

	enum		WeaponType {
		WEAPON_SWORD, WEAPON_SKILL, WEAPON_HAND,
		WEAPON_GUN, WEAPON_HEAD, WEAPON_END,	//생각나느거 있으면 더추가

	};

}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern const unsigned int	g_iFullWinSizeX;
extern const unsigned int	g_iFullWinSizeY;

static  int			m_iLoadingIndex;

using namespace Client;



