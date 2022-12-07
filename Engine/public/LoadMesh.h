#pragma once

#include "VIBuffer.h"
#include "LoadModel.h"

BEGIN(Engine)

class CLoadMesh final : public CVIBuffer
{
private:
	CLoadMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadMesh(const CLoadMesh& rhs);
	virtual ~CLoadMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	const char* Get_MeshName() const { return m_szName; }

public:
	virtual HRESULT Initialize_Prototype(class CLoadModel* pModel, HANDLE hFile);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

public:
	CLoadModel::LOAD_TYPE		m_eType;
	char						m_szName[MAX_PATH] = "";
	_uint						m_iMaterialIndex = 0;
	_uint						m_iNumBones = 0;
	vector<class CLoadBone*>	m_Bones;


	


private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CLoadModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(HANDLE hFile, class CLoadModel* pModel);

public:
	static CLoadMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  class CLoadModel* pModel , HANDLE hFile);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();

};

END