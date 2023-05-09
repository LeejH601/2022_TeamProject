#include "Map.h"
#include "Object.h"
#include "ModelManager.h"
#include "..\Global\Locator.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"

void CMap::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// Terrain »ý¼º
	XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, _T("Terrain/terrainHeightMap257_2.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
}

void CMap::UpdateObjectArticulation()
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->updateArticulationMatrix();
	}
}

void CMap::Update(float fTimeElapsed)
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->Update(fTimeElapsed);
	}
}

void CMap::RenderTerrain(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);
}

void CMap::RenderMapObjects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->Animate(0.0f);
		m_pMapObjects[i]->Render(pd3dCommandList, true);
	}
}

void CMap::LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	char pstrToken[64] = { '\0' };

	int nGameObjects;

	UINT nRead;

	::ReadStringFromFile(pInFile, pstrToken);

	if (!strcmp(pstrToken, "<GameObjects>:"))
	{
		int nObjects = ReadIntegerFromFile(pInFile);
		for (int i = 0; i < nObjects; ++i) {
			ReadStringFromFile(pInFile, pstrToken);
			nRead = ReadStringFromFile(pInFile, pstrToken);

			float buffer[16];
			std::string objPath{ "Object/Scene/" };
			objPath += static_cast<std::string>(pstrToken) + ".bin";



			FILE* objFile = NULL;
			::fopen_s(&objFile, objPath.data(), "rb");
			::rewind(objFile);

			std::unique_ptr<CGameObject> pObject = std::make_unique<CGameObject>();

			nRead = (UINT)::fread(&buffer, sizeof(float), 16, pInFile);


			XMFLOAT4X4 xmfWorld = {
				buffer[0],buffer[1],buffer[2],buffer[3],
				buffer[4],buffer[5],buffer[6],buffer[7],
				buffer[8],buffer[9],buffer[10],buffer[11],
				buffer[12],buffer[13],buffer[14],buffer[15]
			};

			CLoadedModelInfo* rootObj = CModelManager::GetInst()->LoadGeometryFromFileOfScene(pd3dDevice, pd3dCommandList, objFile);
			rootObj->m_pModelRootObject->m_xmf4x4Transform = xmfWorld;

			std::string name{ pstrToken };
			if (name.find("Tree") != std::string::npos || name.find("Rock") != std::string::npos)
			{
				physx::PxTolerancesScale scale = Locator.GetPxPhysics()->getTolerancesScale();
				physx::PxCookingParams params(scale);

				params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

				physx::PxTriangleMeshDesc meshDesc;
				std::vector<XMFLOAT3> vertexs = rootObj->m_pModelRootObject->m_pMesh->GetVertexs();
				std::vector<XMFLOAT3> world_vertexs; world_vertexs.resize(vertexs.size());
				XMMATRIX trans = XMLoadFloat4x4(&rootObj->m_pModelRootObject->m_xmf4x4Transform);

				XMVECTOR scaling;
				XMVECTOR rotation;
				XMVECTOR translation;
				XMMatrixDecompose(&scaling, &rotation, &translation, XMLoadFloat4x4(&rootObj->m_pModelRootObject->m_xmf4x4Transform));
				XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
				XMMATRIX scalingMatrix = XMMatrixScalingFromVector(scaling);
				XMMATRIX transformMatrix = scalingMatrix * rotationMatrix;

				trans.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				int index = 0;
				for (XMFLOAT3 pos : vertexs) {
					XMFLOAT3 point = Vector3::TransformCoord(pos, transformMatrix);
					world_vertexs[index++] = point;
				}
				meshDesc.points.count = vertexs.size();
				meshDesc.points.stride = sizeof(physx::PxVec3);
				meshDesc.points.data = world_vertexs.data();

				std::vector<UINT> Indices = rootObj->m_pModelRootObject->m_pMesh->GetIndices();
				meshDesc.triangles.count = Indices.size() / 3;
				meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
				meshDesc.triangles.data = Indices.data();


				physx::PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc, Locator.GetPxPhysics()->getPhysicsInsertionCallback());

				physx::PxTransform transform = physx::PxTransform(buffer[12], buffer[13], buffer[14]);
				transform.q.normalize();
				physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.5, 0.5, 0.5);

				physx::PxRigidStatic* actor = physx::PxCreateStatic(*Locator.GetPxPhysics(), transform, physx::PxTriangleMeshGeometry(aTriangleMesh), *material);
				Locator.GetPxScene()->addActor(*actor);

			}

			pObject->SetChild(rootObj->m_pModelRootObject, true);
			XMFLOAT4X4 matrix_scale = {
					10, 0, 0, 0,
					0, 10, 0, 0,
					0, 0, 10, 0,
					0, 0, 0, 1,
			};
			pObject->UpdateTransform(NULL);

			//if (!objPath.contains("Rock"))
			m_pMapObjects.push_back(std::move(pObject));

			fclose(objFile);
		}
	}
}
