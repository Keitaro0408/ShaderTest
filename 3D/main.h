#ifndef MAIN_H
#define MAIN_H

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#include <d3dx9.h>


struct SPHERE
{
	D3DXVECTOR3 vecCenter;
	FLOAT fRadius;
};

struct THING
{
	LPD3DXMESH pMesh;
	LPD3DXMESH pSphereMesh;
	D3DMATERIAL9* pMeshMaterials;
	D3DMATERIAL9* pSphereMeshMaterials;
	LPDIRECT3DTEXTURE9* pMeshTextures;
	DWORD dwNumMaterials;
	D3DXVECTOR3 vecPosition;
	SPHERE Sphere;

	THING()
	{
		ZeroMemory(this, sizeof(THING));
	}
};
#endif