#pragma once
#include "GameObject.h"
#include "BaseDelegater.h"		// 애도 체력있어서 알려줘야함

BEGIN(Engine)

class ENGINE_DLL CMonster abstract :public CGameObject
{
protected:
	CMonster(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
public:
	const _bool	Get_IsIdle()const { return m_bIsIdle; }
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.Animamtion*/
	void	  CurAnimQueue_Play_Tick(_double Time, class CModel* pModel);
	void	  CurAnimQueue_Play_LateTick(class CModel* pModel);
	void	  Set_CombatAnim_Index(class CModel* pModel);

protected:
	_uint							m_bFinishOption = 0;
	_uint							m_iOldAnim = 0;
	_uint							m_iAnimIndex = 0;
	_bool							m_bIsCombatScene = false;
	_bool							m_bIsCombatLastInit = false;
	_bool							m_bIsCombatAndAnimSequnce = false;
	queue<pair<_uint, _double>>		m_CurAnimqeue;
	_bool							m_bIsIdle = false;;
	
	_double							m_NormalTikcPerSecond=1.0;
	_double							m_IntroTimer = 0.7;

private:
	_double					m_fWalkTime = 0.f;
	_float					m_fMoveSpeedRatio = 0.f;
	_bool					m_bKeyInput = false;
	CGameObject*			m_pTargetPlayer = nullptr;
	_bool					m_bIsSwap = false;
	_bool					m_isStop = false;


public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END