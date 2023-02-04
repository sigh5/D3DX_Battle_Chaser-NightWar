#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CBone;
class CTexture;
class CModel;
END

BEGIN(Client)
class CMeshGround  final : public CGameObject
{
private:
	CMeshGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshGround(const CMeshGround& rhs);
	virtual ~CMeshGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CModel*								m_pModelCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static CMeshGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END