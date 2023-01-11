#pragma once
#include "Client_Defines.h"
#include "Canvas.h"
#include "CombatController.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CHpMpBuffCanvas final : public CCanvas
{
private:
	CHpMpBuffCanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHpMpBuffCanvas(const CHpMpBuffCanvas& rhs);
	virtual ~CHpMpBuffCanvas() = default;

public:
	void	Set_StatusHpMpBar(map<const wstring, CStatus*>& StatusMap);

	void	Set_HitEvent(CGameObject* pHiter,_bool bHit);
	void	Set_MpEvent( _bool bHit);
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

	virtual void	Set_RenderActive(_bool bTrue)override;
	virtual void	Shaking_Child_UI()override;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
private:
	CStatus*		Find_Status(const wstring& pNameTag);
	CUI*			Find_UI(const  _tchar* pNameTag);
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;
	map<const wstring, CStatus*>	 m_StatusMap;

	/*Imgui*/
	char Name2[MAX_PATH] = "";

public:
	static CHpMpBuffCanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};


END