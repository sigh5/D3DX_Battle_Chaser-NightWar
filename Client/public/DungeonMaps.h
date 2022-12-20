#pragma once

#include "Client_Defines.h"
#include "Environment_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CDungeonMaps final : public CEnvironment_Object
{
private:
	CDungeonMaps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeonMaps(const CDungeonMaps& rhs);
	virtual ~CDungeonMaps() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;
	void	Change_Model(_uint iLevel, const wstring& NewComPonentTag);
	void	Create_NaviMesh();



private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_uint					m_iShaderPass=0;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDungeonMaps* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END