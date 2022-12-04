#pragma once
#include "Component.h"

BEGIN(Engine)

/* 
	모델은 메쉬들의 집합 
	메쉬들은 뼈를 안가질 수도 있지만 
	뼈가 0개있어도 1개를 만들거임 -> 왜? 애니메이션을 써야되니까
	그래서 모델은 뼈의 데이터도 가지고있는다.
*/

class ENGINE_DLL CModel final:public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_MeshNum()const { return m_iMeshNum; }
	class CBone*	Get_BonePtr(char* pBoneName);

public:
	void	Set_AnimIndex(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);


public: /* for_Tool*/
	_bool			PicikingModel(HWND hWnd, class CTransform* pTransform);

public:
	void	Play_Animation(_double TimeDelta);
	HRESULT	Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0, const char* pBoneConstantName=nullptr);

public:
	const  aiScene*						m_pAIScene = nullptr;
	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;
	
	/* 하나의 모델은 교체가 가능한 여러개의 메쉬로 구성되어 있다.*/
	_uint								m_iMeshNum = 0;
	vector<class CMesh*>				m_Meshs;

	_uint								m_iNumMaterials;	// 모델의 메터리얼 숫자
	vector<MODELMATERIAL>				m_Materials;		// 


	/* 모델을 이루는 전체 뼈의 개수*/
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;

	_uint								m_iCurrentAnimIndex = 0; // 현재 애니메이션의 숫자
	_uint								m_iNumAnimations = 0;	// 전체 애니메이션의 숫자
	vector<class CAnimation*>			m_Animations;


public:
	HRESULT		Ready_Bones(aiNode* pNode);		//Node를 뼈라고 한다.
	HRESULT		Ready_MeshContainers();
	HRESULT		Ready_Materials(const char* pModelFilePath);
	HRESULT		Ready_Animation();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END