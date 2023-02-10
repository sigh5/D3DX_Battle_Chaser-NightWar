#pragma once

/* 내 게임의 초기화 */
/* 내 게임의 진행. */
/* 내 게임의 정리까지 수행. */

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	HRESULT LastInitalize();
	void Tick(_double TimeDelta);
	HRESULT Render();	
	const _bool	 ChangeTimeDelta()const { return m_bTimeStop; }


public:
	HRESULT	Resize_BackBuffer();

private:
	CGameInstance*				m_pGameInstance = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	class CPlayerController*	m_pPlayerController = nullptr;

	class CToolManager*			m_pToolManager = nullptr;


private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;




private:
	HRESULT Start_Level(LEVEL eLevelID);	
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObject();

private:
	HRESULT	Ready_BufferLock_UnLock();

	_bool	m_bLastUpdate = false;
	_bool	m_bTimeStop = false;


public:
	static CMainApp* Create();
	virtual void Free() override;
};	

END