#pragma once
#include "Base.h"

BEGIN(Engine)

/* 
	���� ������ �ִ� ���� 
	���� �̸�, �������, ��������, + ��ɵ�
	�������� -> �θ������ ���� ��

*/
class CBone final :public CBase
{
public:
	CBone();
	virtual ~CBone() = default;

public:
	const			char* Get_BoneName() const { return m_szBoneName; }
	void			Set_offSetMatrix(_float4x4 offSetMatrix) { m_OffsetMatrix = offSetMatrix; }
	
	void Set_TransformMatrix(_fmatrix TransformMatrix) {XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);}

	_matrix Get_OffsetMatrix() {return XMLoadFloat4x4(&m_OffsetMatrix);}

	_matrix Get_CombindMatrix() {return XMLoadFloat4x4(&m_CombindTransformMatrix);}



public:
	HRESULT			Initialize(aiNode* pAiNode);
	void			Compute_CombindTransformationMatrix();


private:
	char			m_szBoneName[MAX_PATH];
	_float4x4		m_OffsetMatrix;		// �ѹ� �о���� ���� �Ұ�( �������� ����??)
	_float4x4		m_TransformMatrix;	// �θ������ ��ȯ���
	_float4x4		m_CombindTransformMatrix;	// �� �θ��� �θ� .... ���� ���س�����
	CBone*			m_pParent = nullptr;

public:
	static  CBone*	Create(aiNode* pAiNode);	//aiNode == ���� ��Ÿ���� Ŭ���� 3������
	virtual void	Free()override;


};

END
