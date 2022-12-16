#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	const char* Get_Name() const { return m_szName; }
	_matrix Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }

	_matrix Get_CombindMatrix() { return XMLoadFloat4x4(&m_CombindTransformMatrix); }

	void Set_OffsetMatrix(_float4x4 OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }

	void Set_TransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}
	_matrix Get_TransformMatrix() {
		return XMLoadFloat4x4(&m_TransformMatrix);
	}

	void Set_Parent(CBone* pBone) { m_pParent = pBone; Safe_AddRef(m_pParent); }
public:
	HRESULT Initialize(class CModel* pModel, HANDLE hFile);
	void Compute_CombindTransformationMatrix();

private:
	char							m_szName[MAX_PATH];
	_float4x4						m_OffsetMatrix;
	_float4x4						m_TransformMatrix;
	_float4x4						m_CombindTransformMatrix;
	 CBone*					     	m_pParent = nullptr;

public:
	static CBone* Create(class CModel* pModel, HANDLE hFile);
	virtual void Free() override;

};

END