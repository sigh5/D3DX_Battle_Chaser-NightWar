#pragma once

#include "Client_Defines.h"
#include "Environment_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CVIBuffer_Cube;
END


BEGIN(Client)
class CMapOne2D final : public CEnvironment_Object
{
private:
	CMapOne2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapOne2D(const CMapOne2D& rhs);
	virtual ~CMapOne2D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	
	_bool					m_bRenderActive = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMapOne2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};


END