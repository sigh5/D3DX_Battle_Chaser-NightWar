#pragma once
#include "Client_Defines.h"
#include "Player.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CHero_Alumon final : public CGameObject
{
private:
	CHero_Alumon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHero_Alumon(const CHero_Alumon& rhs);
	virtual ~CHero_Alumon() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHero_Alumon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	BaseDelegater<int, int> m_Hero_GullyHPDelegater;

};

END