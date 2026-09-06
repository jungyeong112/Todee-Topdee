#include "Yumbrozi_Tail.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

CYumbrozi_Tail::CYumbrozi_Tail(LPDIRECT3DDEVICE9 pGraphic_Device) : CYumbrozi_Parts(pGraphic_Device)
{
}

CYumbrozi_Tail::CYumbrozi_Tail(const CYumbrozi_Tail& Prototype) :CYumbrozi_Parts(Prototype)
{
}

HRESULT CYumbrozi_Tail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CYumbrozi_Tail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	return S_OK;
}

void CYumbrozi_Tail::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CYumbrozi_Tail::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CYumbrozi_Tail::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CYumbrozi_Tail::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
	
	InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))	                return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

CYumbrozi_Tail* CYumbrozi_Tail::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CYumbrozi_Tail* pInstance = new CYumbrozi_Tail(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_Tail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_Tail::Clone(void* pArg)
{

	CYumbrozi_Tail* pInstance = new CYumbrozi_Tail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_Tail");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void CYumbrozi_Tail::Free()
{
	__super::Free();
}


HRESULT CYumbrozi_Tail::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Head"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(24.f * g_fDot, 24.f * g_fDot, 1.f);

	return S_OK;
}
