#pragma once

#include "Client_Defines.h"
#include "Environment_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CMap_3D_UI  final : public CEnvironment_Object
{
private:
	CMap_3D_UI(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CMap_3D_UI(const CMap_3D_UI& rhs);
	virtual ~CMap_3D_UI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void			Coll_CaptinPlayer();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	_float					m_fMoveTimer = 0.f;
	_int					m_iLeft_Right = -1;
	_bool					m_bRenderFont = false;
	_bool					m_bOnce = false;

	_float					m_FontPosX = 0.f;
	_float					m_FontPosY = 0.f;

	_float					m_FontSizeX = 1.f;
	_float					m_FontSizeY = 1.f;
public:
	static CMap_3D_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END