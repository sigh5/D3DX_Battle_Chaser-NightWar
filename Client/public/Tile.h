#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVI_Buffer_Terrain;
class CVIBuffer_Rect;
class CTexture;
END


BEGIN(Client)

class CTile final :public CGameObject
{
private:
	CTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTile(const CTile& rhs);
	virtual ~CTile() = default;

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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr, };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	_bool					m_bCreateCheck = false;
public:
	static CTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END