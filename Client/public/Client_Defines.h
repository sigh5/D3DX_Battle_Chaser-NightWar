#pragma once

#include <Process.h>
#include <random>
#include <time.h>
#include <queue>
#include <set>
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



