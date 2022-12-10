#pragma once
#include "Client_Defines.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CHero_Knolan final :public CPlayer
{
protected:
	CHero_Knolan(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Knolan(const CHero_Knolan& rhs);
	virtual ~CHero_Knolan() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

	virtual _vector	Get_CameraBoneVector()override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;


public: /*For.Dungeon*/
	virtual _uint		Get_AnimationIndex() override;
	virtual void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;

public: 


private:
	void			ObserverTest(_double TimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom =  nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	
	int			m_iMeshIndex = 0;
	_bool		m_bIsCombatScene = false;


public:
	static CHero_Knolan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<Tag_HighLightUIDesc> m_Hero_DungeonUIDelegeter;



	/*Test*/
	BaseDelegater<_double, _uint> m_Hero_GullyHPDelegater;

	/*Test*/
	BaseDelegater<_uint> m_Hero_GullyTestShakingDelegater;

};

END
