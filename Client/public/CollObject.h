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
class CCollObject final : public CEnvironment_Object
{
private:
	CCollObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCollObject(const CCollObject& rhs);
	virtual ~CCollObject() = default;

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
	_bool					m_bActive = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

public:
	static CCollObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END
