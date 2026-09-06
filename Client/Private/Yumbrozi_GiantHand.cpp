#include "Yumbrozi_GiantHand.h"
#include "Transform.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"


CYumbrozi_GiantHand::CYumbrozi_GiantHand(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject(pGraphic_Device)
{
}

CYumbrozi_GiantHand::CYumbrozi_GiantHand(const CYumbrozi_GiantHand& Prototype) :CGameObject(Prototype)
{
}

HRESULT CYumbrozi_GiantHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYumbrozi_GiantHand::Initialize(void* pArg)
{
	m_pGiantHandTimer[GH_FIRST] = m_pGameInstance->CreateTimerWithDuration(0.3f, false, [=]() {m_pGiantHandTimer[GH_WAIT]->Restart(); }, false);
	m_pGiantHandTimer[GH_WAIT] = m_pGameInstance->CreateTimerWithDuration(1.3f, false, [=]() {m_pGiantHandTimer[GH_PULL]->Restart(); }, false);
	m_pGiantHandTimer[GH_PULL] = m_pGameInstance->CreateTimerWithDuration(0.2f, false, [=]() {m_pGiantHandTimer[GH_DROP]->Restart(); }, false);
	m_pGiantHandTimer[GH_DROP] = m_pGameInstance->CreateTimerWithDuration(0.266f, false, [=]() {m_pGiantHandTimer[GH_CRAHSH]->Restart(); }, false);
	m_pGiantHandTimer[GH_CRAHSH] = m_pGameInstance->CreateTimerWithDuration(0.833f, false, [=]() {m_pGiantHandTimer[GH_VANISH]->Restart(); }, false);
	m_pGiantHandTimer[GH_VANISH] = m_pGameInstance->CreateTimerWithDuration(0.466f, false, [=]() {m_bIsTimer = false, Vanish(); }, false);

	CHKFAIL(Ready_Components());

	m_pArmTransformCom->Scaling(0.96, 1, 0);
	m_pArm2TransformCom->Scaling(0.96, 1, 0);
	m_pArmTransformCom->Set_State(STATE::POSITION, _float3{ 0,  13.5f / g_iCubicDot, -0.7f });
	m_pArm2TransformCom->Set_State(STATE::POSITION, _float3{ 0, 26.f / g_iCubicDot ,-1.3f });

	return S_OK;
}

void CYumbrozi_GiantHand::Priority_Update(_float fTimeDelta)
{

}

void CYumbrozi_GiantHand::Update(_float fTimeDelta)
{
	if (!IsActive()) return;

	Move(fTimeDelta);
}

void CYumbrozi_GiantHand::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
	//m_bIsCrash = false;
}

HRESULT CYumbrozi_GiantHand::Render()
{
	if (!IsActive()) return S_OK;

	CRenderStateGaurd _rsGuard(m_pGraphic_Device);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Texture(0)))	                return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))					    return E_FAIL;

	if (FAILED(m_pArmTransformCom->Bind_Resource()))			return E_FAIL;
	if (FAILED(m_pArmTextureCom->Bind_Texture(0)))	            return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))					    return E_FAIL;

	if (FAILED(m_pArm2TransformCom->Bind_Resource()))			return E_FAIL;
	if (FAILED(m_pArm2TextureCom->Bind_Texture(0)))	            return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))					    return E_FAIL;

	auto _vPos = m_pArmTransformCom->Get_State(STATE::POSITION);


	return S_OK;
}

HRESULT CYumbrozi_GiantHand::Ready_Components()
{

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_GiantHand"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Head"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pArm2TextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Head"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pArmTextureCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc{};
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	TransformDesc.parent = m_pTransformCom;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform1"), reinterpret_cast<CComponent**>(&m_pArm2TransformCom), &TransformDesc)))
		return E_FAIL;
	m_pArm2TransformCom->SetParent(&TransformDesc);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform2"), reinterpret_cast<CComponent**>(&m_pArmTransformCom), &TransformDesc)))
		return E_FAIL;
	m_pArmTransformCom->SetParent(&TransformDesc);

	m_pTransformCom->Scale(80.f / g_iCubicDot, 80.f / g_iCubicDot, 1.f);

	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) {OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pTransformCom;
	ColliderDesc.strCollisionLayerTag = L"GiantHand";

	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 80 * g_fDot;
	_collisionInfo.fWidth = 80 * g_fDot;

	_collisionInfo.vCenter = { 0.f, 0.f };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CYumbrozi_GiantHand::Move(_float fTimedelta)
{
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	if (m_pGiantHandTimer[GH_FIRST]->IsActive())
	{
		auto fRatio = m_pGiantHandTimer[GH_FIRST]->GetElapsedRatio();
		_vPos.y = fRatio * m_fWaitPos.y + (1 - fRatio) * _vPos.y;
	}
	if (m_pGiantHandTimer[GH_PULL]->IsActive())
	{
		auto fRatio = m_pGiantHandTimer[GH_PULL]->GetElapsedRatio();
		_vPos.y = fRatio * (m_fWaitPos.y + 3) + (1 - fRatio) * _vPos.y;
	}
	if (m_pGiantHandTimer[GH_DROP]->IsActive())
	{
		auto fRatio = m_pGiantHandTimer[GH_DROP]->GetElapsedRatio();
		if (fRatio >= 0.95)
			m_bIsCrash = true;
		_vPos.y = fRatio * m_fTargetPos.y + (1 - fRatio) * _vPos.y;
	}
	if (m_pGiantHandTimer[GH_CRAHSH]->IsActive())
	{ 
		auto fRatio = m_pGiantHandTimer[GH_CRAHSH]->GetElapsedRatio();
		_float y2 = .5f * exp(-fRatio / 1);
		_float y = -y2 * sin(D3DX_PI * 2.f * 2.f * fRatio) + m_fTargetPos.y;

		_vPos.y = y; // fRatio* y + (1 - fRatio) * _vPos.y;
	}
	if (m_pGiantHandTimer[GH_VANISH]->IsActive())
	{
		auto fRatio = m_pGiantHandTimer[GH_VANISH]->GetElapsedRatio();
		_vPos.y = fRatio * (m_fStartPos.y + 2.5f) + (1 - fRatio) * _vPos.y;
	}


	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
}

CYumbrozi_GiantHand* CYumbrozi_GiantHand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYumbrozi_GiantHand* pInstance = new CYumbrozi_GiantHand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_GiantHand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_GiantHand::Clone(void* pArg)
{
	CYumbrozi_GiantHand* pInstance = new CYumbrozi_GiantHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_GiantHand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYumbrozi_GiantHand::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pArmTransformCom);
	Safe_Release(m_pArm2TransformCom);
	Safe_Release(m_pArm2TextureCom);
	Safe_Release(m_pArmTextureCom);
}

void CYumbrozi_GiantHand::OnCollision(FCollisionInfo _fCollisionInfo)
{
}

void CYumbrozi_GiantHand::Punch(_float3 _vPos)
{
	SetActive(true);
	m_pColliderCom->SetActive(true);
	m_fTargetPos = _vPos;
	m_fTargetPos.y = _vPos.y + 2;
	m_fStartPos = _vPos;
	m_fStartPos.y = g_iMapSizeY;
	m_fWaitPos = _vPos;
	m_fWaitPos.y = g_iMapSizeY - 3;
	if (m_pGiantHandTimer && !m_bIsTimer) m_pGiantHandTimer[GH_FIRST]->Restart();
	m_pTransformCom->Set_State(STATE::POSITION, m_fStartPos);
}

void CYumbrozi_GiantHand::Vanish()
{
	SetActive(false);
	m_pColliderCom->SetActive(false);
}
