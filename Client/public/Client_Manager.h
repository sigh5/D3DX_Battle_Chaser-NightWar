#pragma once
#include "Client_Defines.h"
#include "Base.h"
/* 
	유니티(빈게임 오브젝트라고 생각하면됌)
	함수로 빼기 애매한 것들 전체적인 변수 관리
*/
BEGIN(Engine)
class CTransform;

END


enum DIR { DIR_RIGHT, DIR_UP , DIR_LOOK, DIR_END};

class CClient_Manager  
{


public:
	CClient_Manager() = default;


public:
	static void	Client_Manager_Update();	// 전체적인 업무담당

public: /* For_UI */
	void distance_Limit_UP(CTransform* pTransform, _float4& vTransformFunction, DIR eType);


public:
	static _double	 TimeDelta;

	// 현재트랜스폼, 목표지점


};

