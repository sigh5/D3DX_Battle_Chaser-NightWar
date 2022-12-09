#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CHero_Calibretto final :public CPlayer
{
protected:
	CHero_Calibretto(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Calibretto(const CHero_Calibretto& rhs);
	virtual ~CHero_Calibretto() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual _vector	Get_CameraBoneVector()override;
	
public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

public: /*For.Dungeon*/
	virtual _uint	Get_AnimationIndex() override;
	virtual void	AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
private:
	void			ObserverTest();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	int			AnimIndex = 0;
	int			m_iPlayerType = 0;


public:
	static CHero_Calibretto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<HIGHLIGHT_UIDESC> m_Hero_DungeonUIDelegeter;
};

END
