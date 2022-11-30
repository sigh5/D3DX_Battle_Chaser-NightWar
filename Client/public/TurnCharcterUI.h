#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Client_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CTurnCharcterUI final : public CUI
{
private:
	CTurnCharcterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTurnCharcterUI(const CTurnCharcterUI& rhs);
	virtual ~CTurnCharcterUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /*Observer Pattern*/
	void	UI_Event(_double TileDelta, _uint iMoveSpeed);


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool					m_bMoveCheck = false;
	_float4					m_vLimitPos;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTurnCharcterUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


private:
	std::function<void(CTransform*, _float4&, OBJ_TYPE eType)> m_CheckFunction;
	
};

END
