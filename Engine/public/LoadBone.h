#pragma once
#include "Base.h"

BEGIN(Engine)

class CLoadBone : public CBase
{
private:
	CLoadBone();
	CLoadBone(const CLoadBone& rhs);
	virtual ~CLoadBone() = default;

public:
	const char* Get_Name() const {return m_szName;}
	_matrix Get_OffsetMatrix() {return XMLoadFloat4x4(&m_OffsetMatrix);}

	_matrix Get_CombindMatrix() {return XMLoadFloat4x4(&m_CombindTransformMatrix);}

	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {m_OffsetMatrix = OffsetMatrix;}

	void Set_TransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

public:
	HRESULT Initialize(class CLoadModel* pModel,HANDLE hFile);
	HRESULT Initialize_DeepCopy(void *pArg =nullptr);
	void Compute_CombindTransformationMatrix();

private:
	char							m_szName[MAX_PATH];
	char							m_szParentName[MAX_PATH];
	_float4x4						m_OffsetMatrix;
	_float4x4						m_TransformMatrix;
	_float4x4						m_CombindTransformMatrix;
	class CLoadBone*				m_pParent = nullptr;
	
public:
	static CLoadBone* Create(class CLoadModel* pModel , HANDLE hFile);
	CLoadBone* CreateClone(void* pArg = nullptr);
	virtual void Free() override;

};

END