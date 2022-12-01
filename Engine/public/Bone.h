#pragma once
#include "Base.h"

BEGIN(Engine)

/* 
	뼈가 가지고 있는 정보 
	뼈의 이름, 행렬정보, 상태정보, + 기능들
	상태정보 -> 부모행렬을 곱한 값

*/
class CBone final :public CBase
{
public:
	CBone();
	virtual ~CBone() = default;

public:
	const			char* Get_BoneName() const { return szBoneName; }
	void			Set_offSetMatrix(_float4x4 offSetMatrix) { m_OffsetMatrix = offSetMatrix; }

public:
	HRESULT			Initialize(aiNode* pAiNode);
	void			Compute_CombindTransformationMatrix();

private:
	char			szBoneName[MAX_PATH];
	_float4x4		m_OffsetMatrix;		// 한번 읽어오면 수정 불가( 기초적인 로컬??)
	_float4x4		m_TransformMatrix;	// 부모기준의 변환행렬
	_float4x4		m_CombindTransformMatrix;	// 내 부모의 부모 .... 까지 곱해놓은것
	CBone*			m_pParent = nullptr;

public:
	static  CBone*	Create(aiNode* pAiNode);	//aiNode == 뼈를 나타낸느 클래스 3개있음
	virtual void	Free()override;


};

END
