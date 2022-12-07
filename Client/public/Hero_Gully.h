#pragma once
#include "Client_Defines.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CLoadModel;
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

public:
	virtual void KeyInput(_double TimeDelta)override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;


private:
	void			ObserverTest();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	//CModel*					m_pModelCom = nullptr;
	CLoadModel*				m_pModelCom =  nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	int			AnimIndex = 0;
	int			m_iPlayerType = 0;

	_double		m_fWalkTime = 0.f;
	_float		m_fMoveSpeedRatio = 0.f;
	_bool		m_bKeyInput = false;

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
