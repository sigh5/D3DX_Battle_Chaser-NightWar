#pragma once
#include "Component.h"

/* ��ü�� ���� �����̽� �󿡼��� ���¸� �������ش�.(m_WorldMatrix) */
/* ���º�ȯ�� ���� �������̽��� �������ش�.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (���� �Ǵ� �θ�) �������κ��� �󸶳� �������־�. */
	/* Position : �������� ������������� ��ġ. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct tagTransformDesc
	{
		/* �ʴ� ���������ϴ� �ӵ�. */
		float		fSpeedPerSec;

		/* �ʴ� ȸ���ؾ��ϴ� �ӵ�. */
		float		fRotationPerSec;
	}TRANSFORMDESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	void	Set_TransfromDesc(_float Speed, _float Rotaiton) { m_TransformDesc.fSpeedPerSec = Speed;
	m_TransformDesc.fRotationPerSec = Rotaiton;
	}

	_matrix Get_WorldMatrix_Inverse() { //�θ� ������ ���� ������ ������
		
		if(nullptr == m_pParentTransfrom)
			return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
		
		return  XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ParentAndChildWorldMatrix));
	
	}
	_matrix Get_WorldMatrix() {		//�θ� ������ ���� ������ ������
		if (nullptr == m_pParentTransfrom)
			return XMLoadFloat4x4(&m_WorldMatrix);
	
		return  XMLoadFloat4x4(&m_ParentAndChildWorldMatrix);
	}
	_matrix	Get_LocalMatrix()
	{
		return  XMLoadFloat4x4(&m_WorldMatrix);
	}
	void	Set_WorldMatrix(_float4x4 WorldFlot4x4)
	{
		memcpy(&m_WorldMatrix, &WorldFlot4x4, sizeof(WorldFlot4x4));
	}
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}
	_float3 Get_Scaled() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}
	void Set_State(STATE eState, _fvector vState) {
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);
	}
	void Set_Scaled(STATE eState, _float fScale); /* fScale������ ���̸� �����Ѵ�. */
	void Set_Scaled(_float3 vScale); /* fScale������ ���̸� �����Ѵ�. */
	void Scaling(STATE eState, _float fScale); /* fScale����� �ø���. */
	void	Set_ParentTransform(CTransform* pTransform) {m_pParentTransfrom = pTransform;}

	CTransform* Get_ParentTransform() { return m_pParentTransfrom; }

	virtual		void	Final_Update()override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

public:
	void Go_Straight(_double TimeDelta,_float fSpeedRatio =1.f, class CNavigation* pNaviCom = nullptr);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	
	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */


												   /* �Ĵٺ���. */
												   /*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);

	/* �����Ѵ� .*/
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f);
	_bool JudgeChaseState(_fvector vTargetPos, _float fLimit = 0.1f);

	void Chase_Speed(_fvector vTargetPos, _double TimeDelta,_float fSpeedRatio ,_float fLimit = 0.1f);

	_bool	CombatChaseTarget(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f,_float fSpeedMultiple=2.f);

	void	Chase_Rising(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f);



public: /* for_UI*/
	void Go_Up(_double TimeDelta);
	void Go_Down(_double TimeDelta);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

private:
	_float4x4				m_WorldMatrix;
	_float4x4				m_ParentAndChildWorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

	std::queue<_vector>		m_pTargetPosQueue;
	
	_float					m_fChaseLerpFixTimer = 0.2f;
	_float					m_fChaseLerpTimer = 0.0f;
private:
	CTransform*				m_pParentTransfrom = nullptr;
	


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};


END