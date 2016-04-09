#include "main.h"

HRESULT InitSphere(LPDIRECT3DDEVICE9 pDevice, THING* pThing, float collisiondif)
{
	HRESULT hr = NULL;
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	VOID* pVertices = NULL;
	D3DXVECTOR3 vecCenter;
	FLOAT fRadius;

	//���b�V���̒��_�o�b�t�@�[�����b�N����
	if (FAILED(pThing->pMesh->GetVertexBuffer(&pVB)))
	{
		return E_FAIL;
	}
	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))
	{
		SAFE_RELEASE(pVB);
		return E_FAIL;
	}
	//3D���f���̒��S���W���擾����B
	// ���b�V���̊O�ډ~�̒��S�Ɣ��a���v�Z����
	hr = D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices, pThing->pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(pThing->pMesh->GetFVF()), &vecCenter,
		&fRadius);
	fRadius += collisiondif;
	//vecCenter.z += senterdif;
	pVB->Unlock();
	SAFE_RELEASE(pVB);

	if (FAILED(hr))
	{
		return hr;
	}
	pThing->Sphere.vecCenter = vecCenter;
	pThing->Sphere.fRadius = fRadius;
	// ����ꂽ���S�Ɣ��a����Ƀ��b�V���Ƃ��ẴX�t�B�A���쐬����
	hr = D3DXCreateSphere(pDevice, fRadius, 24, 24, &pThing->pSphereMesh, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	//�X�t�B�A���b�V���̃}�e���A���@���F�A�������A����
	pThing->pSphereMeshMaterials = new D3DMATERIAL9;
	pThing->pSphereMeshMaterials->Diffuse.r = 1.0f;
	pThing->pSphereMeshMaterials->Diffuse.g = 1.0f;
	pThing->pSphereMeshMaterials->Diffuse.b = 1.0f;
	pThing->pSphereMeshMaterials->Diffuse.a = 0.5f;
	pThing->pSphereMeshMaterials->Ambient = pThing->pSphereMeshMaterials->Diffuse;
	pThing->pSphereMeshMaterials->Specular.r = 1.0f;
	pThing->pSphereMeshMaterials->Specular.g = 1.0f;
	pThing->pSphereMeshMaterials->Specular.b = 1.0f;
	pThing->pSphereMeshMaterials->Emissive.r = 0.1f;
	pThing->pSphereMeshMaterials->Emissive.g = 0.1f;
	pThing->pSphereMeshMaterials->Emissive.b = 0.1f;
	pThing->pSphereMeshMaterials->Power = 120.0f;

	return S_OK;
}


bool CollisionCheck(THING* pThingA, THING* pThingB)
{
	//�Q�̕��̂̒��S�Ԃ̋��������߂�
	D3DXVECTOR3 vecLength = pThingB->vecPosition - pThingA->vecPosition;
	FLOAT fLength = D3DXVec3Length(&vecLength);
	// ���̋������A2���̂̔��a�𑫂������̂�菬�����Ƃ������Ƃ́A
	//���E�����m���d�Ȃ��Ă���i�Փ˂��Ă���j�Ƃ�������
	if (fLength < pThingA->Sphere.fRadius + pThingB->Sphere.fRadius)
	{
		return TRUE;
	}
	return FALSE;
}