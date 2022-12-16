#pragma once
#include "Client_Defines.h"
#include "Base.h"
/* 
	유니티(빈게임 오브젝트라고 생각하면됌)
	함수로 빼기 애매한 것들 전체적인 변수 관리
*/
BEGIN(Engine)
class CTransform;
class CGameObject;
class CModel;
class CNavigation;
class CCollider;
END

//bool compare(pair<int, int>a, pair<int, int>b) {
//	
//	return a.first < b.first;
//	
//}


enum  DIR { DIR_RIGHT, DIR_UP , DIR_LOOK, DIR_END};

enum COMPARE_UI_POS {COMPARE_UI_POS_X, COMPARE_UI_POS_Y, COMPARE_UI_POS_END,};

class CClient_Manager  
{


public:
	CClient_Manager() = default;


public:
	static void	Client_Manager_Update();	// 전체적인 업무담당

public: /* For_UI */
	void distance_Limit_UP(CTransform* pTransform, _float4& vTransformFunction, DIR eType);
	// UI 위치비교
	template<typename T>
	static T*	Get_MaxValue_Pointer(vector<T*>& pVec, _float &fMAX , COMPARE_UI_POS eType);

	template<typename T>
	static T*	Get_SecondMaxValue_Pointer(vector<T*>& pVec, _float fMAX);


	template<typename T>
	static T*	Get_SmallValue_Pointer(vector<T*>& pVec, _float &fSmall, COMPARE_UI_POS eType);
	// ~UI 위치비교

public:
	static _double	 TimeDelta;
	static  void    Model_Load(ID3D11Device* m_pDevice, ID3D11DeviceContext*	m_pDeviceContext, _tchar* pDataFileName,_uint iLevel);

public: /* For_Player */
	static void		CaptinPlayer_ColiderUpdate(CGameObject * pGameObject, CCollider * pColider, CTransform* pTransform);



#ifdef _DEBUG

public:/*For.Imgui_AnimTool*/
	static void		Make_Anim_Queue(queue<pair<_uint, _double>>& AnimQueue, AnimType eType);
	static void		Collider_Render(CGameObject * pGameObject, CCollider * pColider);
	static void		Navigation_Render(CGameObject * pGameObject, CNavigation * pNavigationCom);

#endif // _DEBUG

	

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
			// X 위치일떄 
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
inline T * CClient_Manager::Get_SecondMaxValue_Pointer(vector<T*>& pVec , _float fMAX)
{
	_float4 vPos;
	vector<pair<_float,int>> iSubVec;			// 값 , 인덱스
	_float	iSubValue = 0;
	_int	iIndex = 0;
		
	for (auto pGameObject : pVec)
	{
		_float MaxMAx = fMAX;
		XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		iSubValue = MaxMAx - vPos.y;
		_float	iTemp = fabs(iSubValue);
		iSubVec.push_back({ iTemp, iIndex });
		++iIndex;
	}
		
	sort(iSubVec.begin(), iSubVec.end());
	return	pVec[iSubVec[1].second];
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
			// X 위치일떄 
			bool b = false;
		}
		Safe_Release(pGameObject);
	}

	if (nullptr == pMaxObject)
		assert("CClient_Manager::Get_MaxValue_Pointer");

	fSmallY = fSmall;
	return pMaxObject;
}


