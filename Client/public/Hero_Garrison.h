#pragma once
#include "Client_Defines.h"
#include "Player.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CHero_Garrison final : public CPlayer	
{
private:
	CHero_Garrison(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHero_Garrison(const CHero_Garrison& rhs);
	virtual ~CHero_Garrison() = default;

	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;


public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;

public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;

public: /*For.Dungeon*/
	virtual _uint		Get_AnimationIndex() override;
	virtual void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFsmCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


	_bool		m_bIsCombatScene = false;
	_uint		m_iOldAnim = 0;
	_uint		m_bFinishOption = 0;
	_bool		m_bIsTest = false;
	int		m_bTestImguiButton = 0;
public:
	static CHero_Garrison* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	BaseDelegater<Tag_HighLightUIDesc> m_Hero_DungeonUIDelegeter;
	
private:
	queue<pair<_uint, _double>>	m_CurAnimqeue;


};

END