#pragma once
#include "Component.h"

/* 객체의 월드 스페이스 상에서의 상태를 저장해준다.(m_WorldMatrix) */
/* 상태변환을 위한 인터페이스를 제공해준다.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (원점 또는 부모) 기준으로부터 얼마나 떨어져있어. */
	/* Position : 절대적인 월드공간에서의 위치. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct tagTransformDesc
	{
		/* 초당 움직여야하는 속도. */
		float		fSpeedPerSec;

		/* 초당 회전해야하는 속도. */
		float		fRotationPerSec;
	}TRANSFORMDESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	void Set_State(STATE eState, _fvector vState) {
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);			
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_double TimeDelta);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */








private:	
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END