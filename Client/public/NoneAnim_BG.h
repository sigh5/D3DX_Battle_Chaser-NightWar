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
class CNoneAnim_BG final : public CEnvironment_Object
{
private:
	CNoneAnim_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNoneAnim_BG(const CNoneAnim_BG& rhs);
	virtual ~CNoneAnim_BG() = default;

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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CNoneAnim_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END