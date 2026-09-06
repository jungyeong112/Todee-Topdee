#include "Normal_Box.h"
#include "GameInstance.h"

CNormal_Box::CNormal_Box(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CNormal_Box::CNormal_Box(const CNormal_Box& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CNormal_Box::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNormal_Box::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg) {
		_float3 _vPos = *reinterpret_cast<_float3*>(pArg);
		m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	}

	m_iTextureIdx = rand() % 10;

	return S_OK;
}

void CNormal_Box::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CNormal_Box::Update(_float fTimeDelta)
{
}

void CNormal_Box::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
}

HRESULT CNormal_Box::Render()
{
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_Texture(m_iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNormal_Box::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Box"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Normal_Box"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = nullptr;
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = dynamic_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));
	ColliderDesc.strCollisionLayerTag = TEXT("Box");
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 1;
	_collisionInfo.fWidth = 1;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

PickingInfo* CNormal_Box::Picking(RAY tRay)
{
	_float4x4 matWorldInv{};

	RAY tLocalRay = {
		tRay.vPos,
		tRay.vDir
	};

	D3DXMatrixInverse(&matWorldInv, nullptr, m_pTransformCom->Get_WorldMatrixPtr());

	D3DXVec3TransformCoord(&tLocalRay.vPos, &tLocalRay.vPos, &matWorldInv);
	D3DXVec3TransformNormal(&tLocalRay.vDir, &tLocalRay.vDir, &matWorldInv);
	D3DXVec3Normalize(&tLocalRay.vDir, &tLocalRay.vDir);

	PickingInfo* pPickingInfo = m_pVIBufferCom->Picking(tLocalRay);
	if (pPickingInfo)
	{
		pPickingInfo->m_vCenterPos = m_pTransformCom->Get_State(STATE::POSITION);
		pPickingInfo->m_pGameObject = this;
	}

	return pPickingInfo;
}

CNormal_Box* CNormal_Box::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNormal_Box* pInstance = new CNormal_Box(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNormal_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNormal_Box::Clone(void* pArg)
{
	CNormal_Box* pInstance = new CNormal_Box(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNormal_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNormal_Box::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}