#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Dungeon_Controll_Tick(_double TimeDelta);

private:
	HRESULT Ready_Layer_BackGround(const wstring& pLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& pLayerTag);
	HRESULT Ready_Layer_Environment(const wstring& pLayerTag);
	HRESULT Ready_Layer_Player(const wstring& pLayerTag);
	HRESULT Ready_Layer_UI(const wstring& pLayerTag);
	HRESULT Ready_Lights();


private:
	class CPlayerController*		m_pPlayerController = nullptr; // Dungeon


public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END