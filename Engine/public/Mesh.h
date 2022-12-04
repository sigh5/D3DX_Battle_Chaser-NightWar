#pragma once
#include "VIBuffer.h"
#include "Model.h"
// 메쉬는 삼각형들의 집합
// 모델의 교체가능한 파츠들이다.
// 이메시를 그려내기위한 정점, 인덱스 버퍼를 보유하여야한다.

BEGIN(Engine)


class CMesh final: public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint			Get_MaterialIndex()const { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, aiMesh * pAIMesh, class CModel* pModel);
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override;

public:
	void			SetUp_BoneMatrices(_float4x4* pBoneMatrices);


public:
	virtual _bool	PickingBuffer(HWND hWnd, class CTransform * pCubeTransCom, _float4 &vPosition)override { return false; }
private:
	CModel::TYPE			m_eType;
	// 이 메쉬는 m_iMaterialIndex번째 머테리얼을 사용한다.
	_uint					m_iMaterialIndex = 0;

	// 이메시의 정점들에게 영향을 주는 뼈의개수
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;
	
	
	
private:
	HRESULT			Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh);
	HRESULT			Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh, CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,CModel::TYPE eType ,aiMesh* pAIMesh,class CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();


};

END
