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
class CCombatMapBroken : public CEnvironment_Object
{
private:
	CCombatMapBroken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCombatMapBroken(const CCombatMapBroken& rhs);
	virtual ~CCombatMapBroken() = default;

public:
	void	Reset_Anim();

	_bool	Is_Finish()const {
		return m_bAnimChange[2];
	}

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
	static  CCombatMapBroken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END