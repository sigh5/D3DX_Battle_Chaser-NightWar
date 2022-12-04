#pragma once
#include "VIBuffer.h"
#include "Model.h"
// �޽��� �ﰢ������ ����
// ���� ��ü������ �������̴�.
// �̸޽ø� �׷��������� ����, �ε��� ���۸� �����Ͽ����Ѵ�.

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
	// �� �޽��� m_iMaterialIndex��° ���׸����� ����Ѵ�.
	_uint					m_iMaterialIndex = 0;

	// �̸޽��� �����鿡�� ������ �ִ� ���ǰ���
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
