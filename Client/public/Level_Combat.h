#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 이 레벨은 턴제전투용으로 만 쓰인다.*/

BEGIN(Client)

class CLevel_Combat final :public CLevel
{
private:
	CLevel_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Combat() = default;

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
	HRESULT Ready_Layer_UI(const wstring& pLayerTag);
	HRESULT Ready_Lights();

public:
	static CLevel_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END