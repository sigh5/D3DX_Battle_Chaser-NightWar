#pragma once

#include <Process.h>
#include <random>
#include <time.h>
namespace Client
{																			
	enum LEVEL {LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY ,LEVEL_END };// �ε�������Ʈ�� �ǵڿ��� -1
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



