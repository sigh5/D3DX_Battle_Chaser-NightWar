#pragma once

#include <Process.h>
#include <random>
#include <time.h>
#include <queue>
#include <set>
namespace Client
{																			
	enum LEVEL {LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY ,LEVEL_END };// 로딩오브젝트는 맨뒤에서 -1

	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };


	


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



