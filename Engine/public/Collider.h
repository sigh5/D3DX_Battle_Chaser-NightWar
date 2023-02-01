#pragma once
#include "Component.h"

/* 
	�浹�� �ʿ��� ��ü�鿡�� �浹ü�� �߰����ֱ� ����
	��ü�� �浹ü�� ������ �ְ� ������ش�.
	���������� �׸��� ������ ����� ����� �ش�. //Release����϶��� x
*/


BEGIN(Engine)

class ENGINE_DLL CCollider final :public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END};

public:
	typedef struct tagColiderDesc
	{
		_float3			vCenter;
		_float3			vSize;			/* �ʺ� ���� ���� */
		_float3			vRotation;		/* x�� ȸ��, y�� ȸ�� z�� ȸ��*/
	}COLLIDERDESC;
public:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	const _bool		Get_Coll()const { return m_isColl; }
	_float3			Get_Center()const { return m_vCenter; }
	_float3			Get_Extention()const { return m_vExtention; }
	_float			Get_Radius()const { return m_Radius; }
public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_fmatrix TransformMatrix);

public:
	_bool Collision(class CCollider* pTargetCollider);

	/*Only AABB*/
	_bool Collision_Mouse(HWND hWnd);


#ifdef _DEBUG
public:
	virtual HRESULT Render()override;
#endif // _DEBUG

private:
	TYPE						m_eType = TYPE_END;
	BoundingBox*				m_pAABB_Original = nullptr;
	BoundingBox*				m_pAABB = nullptr;
	BoundingOrientedBox*		m_pOBB_Original = nullptr;
	BoundingOrientedBox*		m_pOBB = nullptr;
	BoundingSphere*				m_pSphere_Original = nullptr;
	BoundingSphere*				m_pSphere = nullptr;
	_bool						m_isColl = false;

	_float3						m_vCenter;
	_float3						m_vExtention;
	_float						m_Radius = 0.f;
#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;
	_float4												m_vColor;
#endif // _DEBUG
private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END