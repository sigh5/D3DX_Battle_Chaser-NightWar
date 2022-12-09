#pragma once
#include "Client_Defines.h"
#include "Base.h"
/* 
	����Ƽ(����� ������Ʈ��� �����ϸ��)
	�Լ��� ���� �ָ��� �͵� ��ü���� ���� ����
*/
BEGIN(Engine)
class CTransform;
class CGameObject;
END


enum  DIR { DIR_RIGHT, DIR_UP , DIR_LOOK, DIR_END};

enum COMPARE_UI_POS {COMPARE_UI_POS_X, COMPARE_UI_POS_Y, COMPARE_UI_POS_END,};

class CClient_Manager  
{


public:
	CClient_Manager() = default;


public:
	static void	Client_Manager_Update();	// ��ü���� �������

public: /* For_UI */
	void distance_Limit_UP(CTransform* pTransform, _float4& vTransformFunction, DIR eType);


	// UI ��ġ��
	template<typename T>
	static T*	Get_MaxValue_Pointer(vector<T*>& pVec, _float &fMAX , COMPARE_UI_POS eType);
	template<typename T>
	static T*	Get_SmallValue_Pointer(vector<T*>& pVec, _float &fSmall, COMPARE_UI_POS eType);
	// ~UI ��ġ��

public:
	static _double	 TimeDelta;

	static  void    Model_Load(ID3D11Device* m_pDevice, ID3D11DeviceContext*	m_pDeviceContext, _tchar* pDataFileName,_uint iLevel);


	// ����Ʈ������, ��ǥ����


};

template<typename T>
static T * CClient_Manager::Get_MaxValue_Pointer(vector<T*>& pVec, _float & fMAX, COMPARE_UI_POS eType)
{
	_float4 vPos;
	
	_float fMax = -1000.f;
	T* pMaxObject = nullptr;

	for (auto pGameObject : pVec)
	{
		Safe_AddRef(pGameObject);
		XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		if (eType == COMPARE_UI_POS_Y &&  vPos.y > fMax)
		{
			fMax = vPos.y;
			pMaxObject = pGameObject;
		}
		else if (eType == COMPARE_UI_POS_X && vPos.x > fMax)
		{
			// X ��ġ�ϋ� 
			bool b = false;
		}
		Safe_Release(pGameObject);
	}

	if (nullptr == pMaxObject)
		assert("CClient_Manager::Get_MaxValue_Pointer");

	fMAX = fMax;
	return pMaxObject;
}

template<typename T>
inline T * CClient_Manager::Get_SmallValue_Pointer(vector<T*>& pVec, _float & fSmallY, COMPARE_UI_POS eType)
{
	_float4 vPos;
	T* pMaxObject = nullptr;
	_float fSmall = 1000.f;
	for (auto pGameObject : pVec)
	{
		Safe_AddRef(pGameObject);
		XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		if (eType == COMPARE_UI_POS_Y && vPos.y < fSmall)
		{
			fSmall = vPos.y;
			pMaxObject = pGameObject;
		}
		else if(eType == COMPARE_UI_POS_X && vPos.x < fSmall)
		{
			// X ��ġ�ϋ� 
			bool b = false;
		}
		Safe_Release(pGameObject);
	}

	if (nullptr == pMaxObject)
		assert("CClient_Manager::Get_MaxValue_Pointer");

	fSmallY = fSmall;
	return pMaxObject;
}


