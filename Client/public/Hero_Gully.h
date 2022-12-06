#pragma once
#include "Client_Defines.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
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
	CModel*					m_pModelCom[MAPTYPE_END] = { nullptr, };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	int AnimIndex = 0;

public:
	static CHero_Gully* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:

	/*Test*/
	BaseDelegater<_double, _uint> m_Hero_GullyHPDelegater;

	/*Test*/
	BaseDelegater<_uint> m_Hero_GullyTestShakingDelegater;

};

END
