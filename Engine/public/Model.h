#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tag_ModelDesc
	{
		_tchar* szProtoName = TEXT("");
	}MODELDESC;


private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {return m_iNumMeshes;}
	_matrix Get_PivotMatrix() const {return XMLoadFloat4x4(&m_PivotMatrix);}
	class CBone* Get_BonePtr(const char* pBoneName);

public:
	void Set_AnimIndex(_uint iAnimIndex) {m_iCurrentAnimIndex = iAnimIndex;}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	void Play_Animation(_double TimeDelta);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(CShader* pShader, _uint iMeshIndex, _uint iShaderIndex = 0, const char* pBoneConstantName = nullptr);

public:
	const aiScene*						m_pAIScene = nullptr;

	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>				m_Meshes;

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

	/* 전체 뼈의 갯수. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;

	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


	MODELDESC							m_ModelDesc;

	_float4x4							m_PivotMatrix;
	char								m_szModelPath[MAX_PATH] = "";

public:
	HRESULT Ready_Bones(aiNode* pNode, class CBone* pParent);
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Animation();


public:
	void	Save_Model(HANDLE hFile);
	void	Load_Modle(HANDLE hFile);
private:
	void	Save_Bones(HANDLE hFile);
	void	Save_MeshContainers(HANDLE hFile);
	void	Save_Materials(HANDLE hFile);
	void	Save_Animation(HANDLE hFile);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END