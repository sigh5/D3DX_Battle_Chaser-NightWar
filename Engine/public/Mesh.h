#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	const char* Get_MeshName() const { return m_szName; }

	VTXMODEL*		Get_NonAnimVertex() { return m_pVertices; }
	VTXANIMMODEL*	Get_AnimVertex() { return m_pAnimVertices; }
	FACEINDICES32*	Get_ModelIndeices() { return m_pIndices; }
	 _uint		Get_NumPrimitive() {return m_iNumPrimitive;}


public:
	virtual HRESULT Initialize_Prototype(class CModel* pModel, HANDLE hFile, CModel::LOAD_TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;




	void	LoadFile(HANDLE hFile,CModel* pModel);

public:
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

public:
	CModel::LOAD_TYPE			m_eType;
	char						m_szName[MAX_PATH] = "";
	_uint						m_iMaterialIndex = 0;
	_uint						m_iNumBones = 0;
	vector<class CBone*>		m_Bones;


private:
	VTXMODEL*	 				m_pVertices = nullptr;
	VTXANIMMODEL*	 			m_pAnimVertices = nullptr;
	FACEINDICES32*				m_pIndices = nullptr;


private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(HANDLE hFile, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CModel* pModel, HANDLE hFile, CModel::LOAD_TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();

};

END