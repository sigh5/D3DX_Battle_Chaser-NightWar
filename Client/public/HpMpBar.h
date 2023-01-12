#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CStatus;
END


BEGIN(Client)

class CHpMpBar final : public CUI
{
private:
	CHpMpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHpMpBar(const CHpMpBar& rhs);
	virtual ~CHpMpBar() = default;

public:
	void		Set_HpBarStatus(CStatus* pStatus) ;
	void		Set_MpBarStatus(CStatus* pStatus) ;
	void		Set_Hit(_bool bHit) { m_bHit = bHit; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag);


	virtual void			ShakingControl(_float fCoolTime)override;

private:
	virtual void			Shake_Move(_double TimeDelta)override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	CStatus*				m_pTaskStatus = nullptr;
	_bool					m_bHit = false;
	_uint					m_iOption = 999;		// 0 HP, 1 MP
	
	_float					m_fRatio = 1.f;
	_float					m_fRatioX = 0.f;

/*For_Shaking*/
	_float					m_fCoolTime = 0.f;
	_bool					m_bMove = false;
	_float4					m_vOriginPos;
	_int					m_iSwitching = 1;
	_float4x4				m_WorldMat;
/* ~For_Shaking*/



	


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHpMpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END