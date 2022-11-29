#pragma once
#include "Client_Defines.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CHero_Gully final :public CPlayer
{
protected:
	CHero_Gully(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Gully(const CHero_Gully& rhs);
	virtual ~CHero_Gully() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static CHero_Gully* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<int, int> m_Hero_GullyHPDelegater;


};

END
