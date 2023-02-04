#pragma once
#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)
class CBossLevel  final :public CLevel
{
private:
	CBossLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBossLevel() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Change_SceneData();

	HRESULT Ready_Layer_BackGround(const wstring& pLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& pLayerTag);
	HRESULT Ready_Layer_Player(const wstring& pLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& pLayerTag);
	HRESULT Ready_Layer_UI(const wstring& pLayerTag);
	HRESULT Ready_Lights();

private:
	void	CombatBoss_Control_Tick(_double TimeDelta);
	void	CombatBoss_Intro();
private:
	class	CCombatController*	 	m_pCombatController = nullptr;

	_double						m_dCombatIntroTimer = 0.0;
	_bool						m_bIntroFinish = false;


	_float						m_fSceneChaneTimer = 0.f;
	_float						m_fSceneChaneTimer_lose = 0.f;
private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumCallDraw = 0;
	_double					m_TimeAcc = 0.f;
	_bool					m_bSceneChange = false;

public:
	static CBossLevel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END
