#pragma once

#include "Client_Defines.h"
#include "Environment_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CModel;
END


BEGIN(Client)

class CWaterTile final :public CEnvironment_Object
{
	enum WATER_TYPE { WATER_NORMAL, WATER_DIFFUSE, WATER_HEIGHT, WATER_UV, WATER_END };

private:
	CWaterTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterTile(const CWaterTile& rhs);
	virtual ~CWaterTile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:	/*For_Imgui*/
	_float4		Get_Position()const;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

	_float					m_TimeDelta = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CModel*					m_pModelCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	_bool					m_bCreateCheck = false;
public:
	static CWaterTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END