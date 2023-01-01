#pragma once
#include "Client_Defines.h"
#include "Environment_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel_Instancing;
class CModel;
END

BEGIN(Client)
class CMapOneTree  final : public CEnvironment_Object
{
private:
	CMapOneTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapOneTree(const CMapOneTree& rhs);
	virtual ~CMapOneTree() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;
	void	Picking_pos();

	void	Save_TreePos();
	void	Load_TreePos();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel_Instancing*		m_pModelCom = nullptr;


	int						m_iCreateRadioButton = 0;
	char					szName[MAX_PATH] = "";
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMapOneTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};
END
