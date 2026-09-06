#include "Yumbrozi_Wave.h"
#include "Transform.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"


CYumbrozi_Wave::CYumbrozi_Wave(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject(pGraphic_Device)
{
}

CYumbrozi_Wave::CYumbrozi_Wave(const CYumbrozi_Wave& Prototype) :CGameObject(Prototype)
{
}

HRESULT CYumbrozi_Wave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYumbrozi_Wave::Initialize(void* pArg)
{
	CHKFAIL(Ready_Components());

	m_pTextureCom->SetAnimationDuration(0.5f);

	return S_OK;
}

void CYumbrozi_Wave::Priority_Update(_float fTimeDelta)
{

}

void CYumbrozi_Wave::Update(_float fTimeDelta)
{
	if (!IsActive()) return;
	auto [eState, _] = m_pGameInstance->Get_DimensionInfo();

	auto _vPos = m_pPivotTransform->Get_WorldState(STATE::POSITION);
	auto _vLocPos = m_pPivotTransform->Get_State(STATE::POSITION);
	
		
	_float  fRes = fabs(_vLocPos.x) + fabs(_vLocPos.y);
	if (fRes > 0.0001f)
		m_fColorRatio = _vLocPos. y / fRes;
	else 
		m_fColorRatio = 0.0f;  

	auto [_state, _ratio] = m_pGameInstance->Get_DimensionInfo();

	if (m_fColorRatio>0.5f && !_ratio)
		m_pColliderCom->SetActive(false);

	m_fColorRatio = max(m_fColorRatio, 0.0f, 1.0f);


	if (m_pTextureCom->GetRatio() >= 0.2f)
		m_pColliderCom->SetActive(false);

	if (m_pTextureCom->IsEnd())   
		Vanish();  
}

void CYumbrozi_Wave::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CYumbrozi_Wave::Render()
{
	if (!IsActive()) return S_OK;

	CRenderStateGaurd _rsGuard(m_pGraphic_Device);

	InheritPositon_Begin();

	_float3 white = { 1.0f, 1.0f, 1.0f };
	_float3 gray = { 0.2f, 0.2f, 0.2f };

	_float3 finalColor = {
		white.x * (1.0f - m_fColorRatio) + gray.x * m_fColorRatio,
		white.y * (1.0f - m_fColorRatio) + gray.y * m_fColorRatio,
		white.z * (1.0f - m_fColorRatio) + gray.z * m_fColorRatio
	};


	D3DCOLOR fxColor = D3DCOLOR_COLORVALUE(finalColor.x, finalColor.y, finalColor.z, 1.0f);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, fxColor);


	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_AnimTexture()))	            return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))					    return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

HRESULT CYumbrozi_Wave::Ready_Components()
{

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Wave"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc{};
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform1"), reinterpret_cast<CComponent**>(&m_pPivotTransform), &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	m_pTransformCom->Scale(100.f / g_iCubicDot, 100.f / g_iCubicDot, 1.f);

	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) {OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pPivotTransform;
	ColliderDesc.strCollisionLayerTag = L"Wave";

	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 2;
	_collisionInfo.fWidth = 2;

	_collisionInfo.vCenter = { 0.f, 0.f };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}


CYumbrozi_Wave* CYumbrozi_Wave::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYumbrozi_Wave* pInstance = new CYumbrozi_Wave(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_Wave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_Wave::Clone(void* pArg)
{
	CYumbrozi_Wave* pInstance = new CYumbrozi_Wave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_Wave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYumbrozi_Wave::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPivotTransform);
}

void CYumbrozi_Wave::InheritPositon_Begin()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_vOffset = m_pPivotTransform->Get_WorldState(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + m_vOffset);

	auto _vPo3s = m_pTransformCom->Get_State(STATE::POSITION);
}

void CYumbrozi_Wave::InheritPositon_End()
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - m_vOffset);
	m_vOffset = _float3{ 0.f,0.f, 0.f };

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CYumbrozi_Wave::OnCollision(FCollisionInfo _fCollisionInfo)
{
}

void CYumbrozi_Wave::StartWave(_float3 _vPos)
{
	SetActive(true);
	m_pColliderCom->SetActive(true);
	m_fStartPos = _vPos;
	m_pPivotTransform->Set_State(STATE::POSITION, _vPos);
	auto vPos = m_pPivotTransform->Get_State(STATE::POSITION);
	m_pTextureCom->SetActive_AnimTexture(true);
}

void CYumbrozi_Wave::Vanish()
{
	SetActive(false);
	m_pColliderCom->SetActive(false);
}

void CYumbrozi_Wave::SetParent(CGameObject* pGameObject)
{
	CTransform::TRANSFORM_DESC		TransformDesc{};
	if (pGameObject)
		TransformDesc.parent = static_cast<CTransform*>(pGameObject->Get_Component(TEXT("Com_Transform99")));
	else
		TransformDesc.parent = nullptr;
	m_pPivotTransform->SetParent(&TransformDesc);

}

HRESULT CYumbrozi_Wave::SetParentTransform(CTransform* pTransformCom)
{
	if (!pTransformCom) return E_FAIL;
	m_pPivotTransform = pTransformCom;
	return S_OK;
}

