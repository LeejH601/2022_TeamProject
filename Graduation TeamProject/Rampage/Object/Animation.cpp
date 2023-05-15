#include "Animation.h"
#include "Object.h"
#include "Mesh.h"
#include "ModelManager.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSet::CAnimationSet(float fLength, int nFramesPerSecond, int nKeyFrames, int nAnimatedBones, char* pstrName)
{
	m_fLength = fLength;
	m_nFramesPerSecond = nFramesPerSecond;
	m_nKeyFrames = nKeyFrames;

	strcpy_s(m_pstrAnimationSetName, 64, pstrName);

	m_pfKeyFrameTimes.resize(nKeyFrames);
	m_ppxmf4x4KeyFrameTransforms.resize(nKeyFrames);
	for (int i = 0; i < nKeyFrames; i++) m_ppxmf4x4KeyFrameTransforms[i] = new XMFLOAT4X4[nAnimatedBones];
}
CAnimationSet::~CAnimationSet()
{
}
XMFLOAT4X4 CAnimationSet::GetSRT(int nBone, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
	for (int i = 0; i < (m_nKeyFrames - 1); i++)
	{
		if ((m_pfKeyFrameTimes[i] <= fPosition) && (fPosition < m_pfKeyFrameTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTimes[i]) / (m_pfKeyFrameTimes[i + 1] - m_pfKeyFrameTimes[i]);
			xmf4x4Transform = Matrix4x4::Interpolate(m_ppxmf4x4KeyFrameTransforms[i][nBone], m_ppxmf4x4KeyFrameTransforms[i + 1][nBone], t);
			break;
		}
	}
	if (fPosition >= m_pfKeyFrameTimes[m_nKeyFrames - 1]) xmf4x4Transform = m_ppxmf4x4KeyFrameTransforms[m_nKeyFrames - 1][nBone];
	return(xmf4x4Transform);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSets::CAnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_pAnimationSets.resize(nAnimationSets);
}
CAnimationSets::~CAnimationSets()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CAnimationTrack::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys.resize(nCallbackKeys);
}
void CAnimationTrack::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}
void CAnimationTrack::SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}
void CAnimationTrack::HandleCallback()
{
	if (m_pAnimationCallbackHandler)
	{
		for (int i = 0; i < m_nCallbackKeys; i++)
		{
			if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
			{
				if (m_pCallbackKeys[i].m_pCallbackData) m_pAnimationCallbackHandler->HandleCallback(m_pCallbackKeys[i].m_pCallbackData, m_fPosition);
				break;
			}
		}
	}
}
float CAnimationTrack::UpdatePosition(float fTrackPosition, float fElapsedTime, float fAnimationLength)
{
	float fTrackElapsedTime = fElapsedTime * m_fSpeed;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		if (m_fPosition < 0.0f) m_fPosition = 0.0f;
		else
		{
			m_fPosition = fTrackPosition + fTrackElapsedTime;
			if (m_fPosition > fAnimationLength)
			{
				m_fPosition = -ANIMATION_CALLBACK_EPSILON;
				return(fAnimationLength);
			}
		}
		//			m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//			m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE_ONCE:
		m_fPosition = fTrackPosition + fTrackElapsedTime;
		if (m_fPosition > fAnimationLength) m_fPosition = fAnimationLength;
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}

	return(m_fPosition);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks.resize(nAnimationTracks);

	m_pAnimationSets = pModel->m_pAnimationSets;

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes.resize(m_nSkinnedMeshes);
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms.resize(m_nSkinnedMeshes);
	m_ppcbxmf4x4MappedSkinningBoneTransforms.resize(m_nSkinnedMeshes);

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256ÀÇ ¹è¼ö
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}
CAnimationController::~CAnimationController()
{
}
void CAnimationController::SetCallbackKeys(int nAnimationTrack, int nCallbackKeys)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetCallbackKeys(nCallbackKeys);
}
void CAnimationController::SetCallbackKey(int nAnimationTrack, int nKeyIndex, float fKeyTime, void* pData)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetCallbackKey(nKeyIndex, fKeyTime, pData);
}
void CAnimationController::SetAnimationCallbackHandler(int nAnimationTrack, CAnimationCallbackHandler* pCallbackHandler)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetAnimationCallbackHandler(pCallbackHandler);
}
void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks.data())
	{
		m_pAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[nAnimationTrack].SetPosition(0.0f);
	}
}
void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}
void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}
void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}
void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}
void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}
void CAnimationController::AdvanceTime(float fTimeElapsed, CGameObject* pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks.data())
	{
		for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++) m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = Matrix4x4::Zero();

		for (int k = 0; k < m_nAnimationTracks; k++)
		{
			if (m_pAnimationTracks[k].m_bEnable)
			{
				CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
				float fPosition = m_pAnimationTracks[k].UpdatePosition(m_pAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_fLength);

				for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
				{
					XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform;
					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
					XMFLOAT4X4 xmf4x4ScaledTransform = Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight);
					xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, xmf4x4ScaledTransform);
					m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = xmf4x4Transform;
				}
				m_pAnimationTracks[k].HandleCallback();
			}
		}

		OnRootMotion(pRootGameObject, fTimeElapsed);
		OnAnimationIK(pRootGameObject);

		pRootGameObject->UpdateTransform(NULL);
	}
}
