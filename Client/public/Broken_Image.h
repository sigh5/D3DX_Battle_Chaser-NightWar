#pragma once
#include "Client_Defines.h"
#include "Environment_Object.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBroken_Image : public CEnvironment_Object
{
private:
	CBroken_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroken_Image(const CBroken_Image& rhs);
	virtual ~CBroken_Image() = default;

public:
	void	Reset_Anim();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	

	CTexture*				m_pTextureCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool					m_bAnimChange[3] = { false };

public:
	static  CBroken_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END
