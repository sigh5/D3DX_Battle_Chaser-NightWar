#pragma once

#include <Process.h>
#include <random>
#include <time.h>
#include <queue>
#include <set>
#include <string>
#include <array>

#define EPSILON 0.0001
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

#define NOMODLES
//#define FONT_TEST
#define LEVEL_BOSSONLY

namespace Client
{																			
	enum LEVEL {LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,LEVEL_COMBAT,LEVEL_END };// 로딩오브젝트는 맨뒤에서 -1

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
		BUTTON_FSM_NORMALATTACK, BUTTON_FSM_DEFENCE , 
		BUTTON_FSM_SKILL1 , BUTTON_FSM_SKILL2,
		BUTTON_FSM_ULTIMATE, BUTTON_FSM_BUFF, BUTTON_FSM_WIDEBUFF,
		BUTTON_FSM_USE_HP_ITEM, BUTTON_FSM_USE_MP_ITEM , BUTTON_FSM_FLEE 
		,BUTTON_FSM_STATE_END,
	};

	enum Skill_DIR {
		Skill_DIR_straight, Skill_DIR_LISING, Skill_DIR_DOWN, Skill_DIR_ScaleUP_DOWN,SKILL_CREATE_HITER_POS,			SKILL_TEST , Skill_DIR_END
	};

	enum BUFF_TYPE
	{
		BUFF_ATTACK, BUFF_DEFENCE,			
		BUFF_SINGLE_MANA_UP, BUFF_HP_UP,	// positive

		BUFF_BLEEDING, BUFF_BRUN,			// nagative
		BUFF_END

	};

	enum SOUND_TYPE {
		SOUND_BGM =  0,SOUND_MONSTER1 = 1, SOUND_MONSTER2 = 2, SOUND_MONSTER3 = 3, SOUND_VOCIE=4,
		SOUND_TYPE_KNOLAN_EFFECT = 5, SOUND_GARRISON_EFFECT =6 , SOUND_CALIBRETTO_EFFECT=7 ,SOUND_TYPE_HIT = 8, SOUND_ULTIBGM=9, SOUND_TYPE_END
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


static float		g_fMasterBGMVolue = 0.8f;
static  int			m_iLoadingIndex;

using namespace Client;



