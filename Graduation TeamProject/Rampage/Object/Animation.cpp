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

	m_fSequenceWeight = m_fPosition / fAnimationLength;

	return(m_fPosition);
}
float CAnimationTrack::UpdateSequence(float fSequence, float fElapsedTime, float fAnimationLength)
{
	float fTrackElapsedTime = fElapsedTime * m_fSpeed;
	float SequenceElapsedTime = (1.0f * fTrackElapsedTime) / fAnimationLength;

	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		if (m_fSequenceWeight < 0.0f) m_fSequenceWeight = 0.0f;
		else
		{
			m_fSequenceWeight = m_fSequenceWeight + SequenceElapsedTime;
			if (m_fSequenceWeight > 1.0f)
			{
				m_fSequenceWeight = -ANIMATION_CALLBACK_EPSILON;
				return(1.0f);
			}
		}
		//			m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//			m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE_ONCE:
		m_fSequenceWeight = m_fSequenceWeight + fTrackElapsedTime;
		if (m_fSequenceWeight > 1.0f) m_fSequenceWeight = 1.0f;
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}

	return(m_fSequenceWeight);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks.resize(nAnimationTracks);
	m_pAnimationTracks[0].m_fWeight = 1.0f;
	for (int i = 1; i < m_pAnimationTracks.size(); ++i) {
		m_pAnimationTracks[i].m_fWeight = 0.0f;
	}

	m_pAnimationSets = pModel->m_pAnimationSets.get();

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes.resize(m_nSkinnedMeshes);
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms.resize(m_nSkinnedMeshes);
	m_ppcbxmf4x4MappedSkinningBoneTransforms.resize(m_nSkinnedMeshes);

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
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
void CAnimationController::SetSocket(int SkinMeshIndex, std::string& FrameName, CGameObject* DestObject)
{
	if (SkinMeshIndex >= m_nSkinnedMeshes)
		return;

	CSkinnedMesh* mesh = m_ppSkinnedMeshes[SkinMeshIndex];
	for (int i = 0; i < mesh->m_nSkinningBones; ++i) {
		if(strcmp( mesh->m_ppstrSkinningBoneNames[i].data(), FrameName.data()) == 0)
			m_pSockets.push_back(Obj_Socket(std::pair<int,int>(SkinMeshIndex, i), DestObject));
	}
		
		
}
void CAnimationController::UpdateSocketsTransform()
{
	for (int i = 0; i < m_pSockets.size(); ++i) {
		Obj_Socket& Socket = m_pSockets[i];
		int skinMeshIndex = Socket.first.first;
		int BondIndex = Socket.first.second;
		CGameObject* obj = Socket.second;

		XMFLOAT4X4 xmf4x4World; XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppcbxmf4x4MappedSkinningBoneTransforms[skinMeshIndex][BondIndex])));
		obj->m_pChild->UpdateTransform(&xmf4x4World);
	}
}
void CAnimationController::UpdateBoneTransform()
{
	for (int i = 0; i < m_nSkinnedMeshes; ++i) {
		XMFLOAT4X4* p = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];

		for (int j = 0; j < m_ppSkinnedMeshes[i]->m_nSkinningBones; j++) {
			XMStoreFloat4x4(&p[j], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinnedMeshes[i]->m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));
		}
	}
}
void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks.data())
	{
		m_pAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[nAnimationTrack].SetPosition(0.0f);
		m_pAnimationTracks[nAnimationTrack].SetUpdateEnable(true);
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
void CAnimationController::SetTrackUpdateEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks.data()) m_pAnimationTracks[nAnimationTrack].SetUpdateEnable(bEnable);
}
void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i].Get();
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
	UpdateSocketsTransform();
}
void CAnimationController::AdvanceTime(float fTimeElapsed, CGameObject* pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks.data())
	{
		if (m_nAnimationTracks == 1) {
			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++) m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = Matrix4x4::Zero();

			for (int k = 0; k < m_nAnimationTracks; k++)
			{
				if (m_pAnimationTracks[k].m_bEnable)
				{
					CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
					float fPosition;
					if (m_pAnimationTracks[k].m_bUpdate)
						m_pAnimationTracks[k].UpdatePosition(m_pAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_fLength);
					
					fPosition = max(m_pAnimationTracks[k].m_fPosition, 0.0f);

					for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
					{
						XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform;
						XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
						XMVECTOR q = XMQuaternionRotationMatrix(XMLoadFloat4x4(&xmf4x4TrackTransform));


						XMFLOAT4X4 xmf4x4ScaledTransform = Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight);
						xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, xmf4x4ScaledTransform);
						m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = xmf4x4Transform;
					}
					m_pAnimationTracks[k].HandleCallback();
				}
			}
		}
		
		else {
			/*float MaxAnimationLength = 0.f;
		for (int k = 0; k < m_nAnimationTracks; ++k) {
			float length = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet]->m_fLength;
			MaxAnimationLength = (length > MaxAnimationLength) ? length : MaxAnimationLength;
		}
		MaxAnimationLength = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[0].m_nAnimationSet]->m_fLength;*/

			for (int k = 0; k < m_nAnimationTracks; ++k) {
				CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
				//m_pAnimationTracks[k].UpdateSequence(m_pAnimationTracks[k].m_fSequenceWeight, fTimeElapsed, MaxAnimationLength);
				//m_pAnimationTracks[k].UpdatePosition(m_pAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_fLength); // 0번 트랙의 애니메이션 길이에 귀속되도록
				if(m_pAnimationTracks[k].m_bUpdate)
					m_pAnimationTracks[k].UpdatePosition(m_pAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_fLength);
				
			}

			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++) {
				XMFLOAT4X4 xmf4x4Transform{};
				ZeroMemory(&xmf4x4Transform, sizeof(XMFLOAT4X4));
				XMFLOAT3 transfrom{};
				XMVECTOR q{};
				XMVECTOR rotation[2]{};
				/*xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform;
				q = XMQuaternionRotationMatrix(XMLoadFloat4x4(&xmf4x4Transform));*/

				for (int k = 0; k < m_nAnimationTracks; ++k) {
					float fScale = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4World._44;
					if (m_pAnimationTracks[k].m_bEnable)
					{
						XMFLOAT4X4 xmf4x4Rotation{};
						CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
						float fPosition = max(m_pAnimationTracks[k].m_fPosition, 0.0f);
						//float fPosition = m_pAnimationTracks[k].ConvertSequenceToPosition(pAnimationSet->m_fLength);
						//m_pAnimationTracks[k].m_fPosition = fPosition;

						XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
						XMVECTOR q1 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&xmf4x4TrackTransform));
						q1 = XMQuaternionNormalize(q1);
						rotation[k] = q1;

						/*q1 = q1 * m_pAnimationTracks[k].m_fWeight;
						q1 = XMQuaternionNormalize(q1);*/
						//q = q1 + q;



						XMFLOAT3 dt = XMFLOAT3(xmf4x4TrackTransform._41, xmf4x4TrackTransform._42, xmf4x4TrackTransform._43);
						transfrom = Vector3::Add(transfrom, Vector3::ScalarProduct(dt, m_pAnimationTracks[k].m_fWeight, false));


						m_pAnimationTracks[k].HandleCallback();
					}
				}
				q = XMQuaternionSlerp(rotation[0], rotation[1], 1.0f - m_pAnimationTracks[0].m_fWeight);
				//q = XMQuaternionNormalize(q);
				XMStoreFloat4x4(&xmf4x4Transform, XMMatrixRotationQuaternion(q));
				xmf4x4Transform._41 = transfrom.x;
				xmf4x4Transform._42 = transfrom.y;
				xmf4x4Transform._43 = transfrom.z;

				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = xmf4x4Transform;
			}
		}

		for (int k = 0; k < m_nSubAnimationTracks; ++k) {
			CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pSubAnimationTracks[k].m_nAnimationSet];
			if (m_pSubAnimationTracks[k].m_bUpdate)
				m_pSubAnimationTracks[k].UpdatePosition(m_pSubAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_fLength);

			XMFLOAT3 transfrom{};
			XMFLOAT4X4 xmf4x4Transform{};
			for (int j = m_nLayerBlendBaseBoneIndex[k]; j < m_nLayerBlendRange[k]; j++) {
				if (m_pSubAnimationTracks[k].m_bEnable) {
					XMFLOAT4X4 xmf4x4Rotation{};
					

					float fPosition = max(m_pSubAnimationTracks[k].m_fPosition, 0.0f);

					transfrom = XMFLOAT3(
						m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform._41,
						m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform._42,
						m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform._43);

					transfrom = Vector3::ScalarProduct(transfrom, 1.0f - m_fLayerBlendWeights[k], false);

					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
					XMVECTOR q1 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&xmf4x4TrackTransform));
					q1 = XMQuaternionNormalize(q1);

					XMFLOAT3 dt = XMFLOAT3(xmf4x4TrackTransform._41, xmf4x4TrackTransform._42, xmf4x4TrackTransform._43);
					transfrom = Vector3::Add(transfrom, Vector3::ScalarProduct(dt, m_fLayerBlendWeights[k], false));

					m_pSubAnimationTracks[k].HandleCallback();

					XMVECTOR q = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform));
					q = XMQuaternionNormalize(q);
					q = XMQuaternionSlerp(q, q1, m_fLayerBlendWeights[k]);

					XMStoreFloat4x4(&xmf4x4Transform, XMMatrixRotationQuaternion(q));
					xmf4x4Transform._41 = transfrom.x;
					xmf4x4Transform._42 = transfrom.y;
					xmf4x4Transform._43 = transfrom.z;

					m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4Transform = xmf4x4Transform;
				}
			}
		}
		

		OnRootMotion(pRootGameObject, fTimeElapsed);
		OnAnimationIK(pRootGameObject);

		pRootGameObject->UpdateTransform(NULL);

		UpdateBoneTransform();
		
	}
}
