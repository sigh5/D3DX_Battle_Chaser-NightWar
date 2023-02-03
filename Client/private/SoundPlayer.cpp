#include "stdafx.h"
#include "SoundPlayer.h"
#include "GameInstance.h"
#include "Model.h"
#include "Client_Manager.h"

IMPLEMENT_SINGLETON(CSoundPlayer);

CSoundPlayer::CSoundPlayer()
{
	Model_EffectSoundVec.reserve(100);
	Model_HitSoundVec.reserve(100);
}

void CSoundPlayer::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
	if (pGameInstace->GetCurLevelIdx() != LEVEL_COMBAT)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	for (auto& pPair : Model_EffectSoundVec)
	{
		if (pPair.first != nullptr)
		{
			Anim_Model_SoundDesc SoundDesc = pPair.second;

			if (pPair.first->Control_KeyFrame_Create(SoundDesc.iAnimIndex, SoundDesc.iFrame))
			{
				pGameInstace->Stop_Sound(SoundDesc.iSoundChannel);
				pGameInstace->Play_Sound(SoundDesc.pSoundTag, 0.8f, false, SoundDesc.iSoundChannel);
			}
		}
	}

	for (auto& pPair : Model_HitSoundVec)
	{
		if (pPair.first != nullptr)
		{
			Anim_Model_SoundDesc SoundDesc = pPair.second;

			if (pPair.first->Control_KeyFrame_Create(SoundDesc.iAnimIndex, SoundDesc.iFrame))
			{
				pGameInstace->Stop_Sound(SoundDesc.iSoundChannel);
				pGameInstace->Play_Sound(SoundDesc.pSoundTag, 0.8f, false, SoundDesc.iSoundChannel);
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	/*For_Imgui*/
#ifdef _DEBUG
	
	if (Model_EffectSoundVec.size() == 0)
		return;
	ImGui::CollapsingHeader("Change_Sound_Detail");

	_int iIndex = 0;
	if (ImGui::TreeNode("Selcted_Sound"))
	{
		if (ImGui::BeginListBox("##"))
		{

			for (auto Pair : Model_EffectSoundVec)
			{
				char szobjectTag[128];
				wc2c((Pair.second.pSoundTag), szobjectTag);

				if (ImGui::Selectable(szobjectTag))
				{
					m_iVectorIndex = iIndex;
					m_bSelected = true;
				}
				++iIndex;
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (m_bSelected)
	{
		ImGui::Text("AnimIndex %d", Model_EffectSoundVec[m_iVectorIndex].second.iAnimIndex);
		ImGui::Text("Cur_Frame %d", Model_EffectSoundVec[m_iVectorIndex].second.iFrame);

		ImGui::InputInt("Anim_frame", &m_iChangeFrameIndex);

		if (ImGui::Button("Change_AnimFrame"))
		{
			Model_EffectSoundVec[m_iVectorIndex].second.iFrame = m_iChangeFrameIndex;
		}
	}



#endif



	
}

void CSoundPlayer::Add_SoundEffect_Model(CModel * pModel, tag_SoundDesc & Desc)
{
	assert(nullptr != pModel && "CSoundPlayer::Add_SoundEffect_Model");

	Model_EffectSoundVec.push_back({ pModel, Desc });
	//Safe_AddRef(pModel);
}

void CSoundPlayer::Add_SoundHit_Model(CModel * pModel, tag_SoundDesc & Desc)
{
	assert(nullptr != pModel && "CSoundPlayer::Add_SoundEffect_Model");

	Model_HitSoundVec.push_back({ pModel, Desc });
	//Safe_AddRef(pModel);
}

void CSoundPlayer::Clear_allSound()
{
	Model_HitSoundVec.clear();
	Model_EffectSoundVec.clear();
}

void CSoundPlayer::Free()
{
	/*for (auto& pSoundModel : Model_EffectSoundVec)
		Safe_Release(pSoundModel.first);*/
	Model_EffectSoundVec.clear();

	/*for (auto& pSoundModel : Model_HitSoundVec)
		Safe_Release(pSoundModel.first);*/
	Model_HitSoundVec.clear();

	
}
