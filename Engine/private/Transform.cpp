#include "..\public\Transform.h"
#include "Shader.h"
#include "ImGuizmo.h"
#include "GameInstance.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	_vector		vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	Set_State(eState, Get_State(eState) * fScale);
}



void CTransform::Final_Update()
{
	if (nullptr == m_pParentTransfrom)
		return;

	_matrix  matParent = m_pParentTransfrom->Get_WorldMatrix();
	_matrix  matScaleSet = XMMatrixIdentity();
	_vector vRight, vUp, vLook, vTranslate;

	memcpy(&vRight, &matParent.r[0], sizeof(_vector));
	memcpy(&vUp, &matParent.r[1], sizeof(_vector));
	memcpy(&vLook ,&matParent.r[2], sizeof(_vector));
	memcpy(&vTranslate, &matParent.r[3], sizeof(_vector));

	memcpy(&matScaleSet.r[0], &XMVector3Normalize(vRight), sizeof(_vector));
	memcpy(&matScaleSet.r[1], &XMVector3Normalize(vUp), sizeof(_vector));
	memcpy(&matScaleSet.r[2], &XMVector3Normalize(vLook), sizeof(_vector));
	memcpy(&matScaleSet.r[3], &vTranslate, sizeof(_vector));

	XMStoreFloat4x4(&m_ParentAndChildWorldMatrix, XMLoadFloat4x4(&m_WorldMatrix) * matScaleSet);

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Imgui_RenderProperty()
{
	ImGuizmo::BeginFrame();
	static float snap[3] = { 1.f, 1.f, 1.f };
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_WorldMatrix), matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translate", matrixTranslation);
	ImGui::InputFloat3("Rotate", matrixRotation);
	ImGui::InputFloat3("Scale", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_WorldMatrix));

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("##something", &useSnap);
	ImGui::SameLine();
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}

	ImGuiIO& io = ImGui::GetIO();
	RECT rt;
	GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
	POINT lt{ rt.left, rt.top };
	ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
	ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4 matView, matProj;
	XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	ImGuizmo::Manipulate(
		reinterpret_cast<float*>(&matView),
		reinterpret_cast<float*>(&matProj),
		mCurrentGizmoOperation,
		mCurrentGizmoMode,
		reinterpret_cast<float*>(&m_WorldMatrix),
		nullptr, useSnap ? &snap[0] : nullptr);
}

void CTransform::Go_Straight(_double TimeDelta, _float fSpeedRatio, CNavigation* pNaviCom)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vLook) * (m_TransformDesc.fSpeedPerSec * fSpeedRatio) * (_float)TimeDelta;

	if (nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else
	{
		_float4 vPos; 
		XMStoreFloat4(&vPos, vPosition);

		if(true==pNaviCom->isMove_OnNavigation_test(vPos))
			Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));

	}
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec *  (_float)TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec *  (_float)TimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

_bool CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit )
{

	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance > fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec *  (_float)TimeDelta;
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
		return false;
	}
	else
		return true;
}

_bool CTransform::JudgeChaseState(_fvector vTargetPos, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance < fLimit)
	{
		return true;
	}
	return false;
}

void CTransform::Chase_Speed(_fvector vTargetPos, _double TimeDelta, _float fSpeedRatio, _float fLimit,CNavigation* pNaviCom)
{
	if (nullptr == pNaviCom)
		return ;

	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));
	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec *  (_float)TimeDelta;
	if (fDistance > fLimit)
	{
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

_bool CTransform::CombatChaseTarget(_fvector vTargetPos, _double TimeDelta, _float fLimit,  _float fSpeedMultiple )
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * fSpeedMultiple*  (_float)TimeDelta;
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
		return  false;
	}
	
	return true;		//거리에 도착했을 경우 트루 반환
}

void CTransform::Chase_Rising(_fvector vTargetPos, _double TimeDelta, _float fLimit, _bool bLeftMove)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;
	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance > fLimit)
	{
		if (false == bLeftMove)
		{
			vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec*2.f *  (_float)TimeDelta;

			_float4		vYPos;
			XMStoreFloat4(&vYPos, vPosition);
			vYPos.y += 30.f* (_float)TimeDelta;

			Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vYPos));
		}


		else
		{
			vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec*2.f *  (_float)TimeDelta;

			_float4		vYPos;
			XMStoreFloat4(&vYPos, vPosition);
			vYPos.x -= 10.f* (_float)TimeDelta;

			Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vYPos));
		}
	}
}

void CTransform::Go_Up(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec *  (_float)TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);


}

void CTransform::Go_Down(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec *  (_float)TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	if(m_pParentTransfrom != nullptr)
		return pShaderCom->Set_Matrix(pConstantName, &m_ParentAndChildWorldMatrix);

	return pShaderCom->Set_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTransform::Free()
{
	__super::Free();

	/*if (m_pParentTransfrom != nullptr)
		Safe_Release(m_pParentTransfrom);*/
}
