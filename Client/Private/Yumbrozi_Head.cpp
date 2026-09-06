#include "Yumbrozi_Head.h"
#include "Transform.h"
#include "Texture.h"
#include "Boss.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

CYumbrozi_Head::CYumbrozi_Head(LPDIRECT3DDEVICE9 pGraphic_Device):CYumbrozi_Parts(pGraphic_Device)
{
}

CYumbrozi_Head::CYumbrozi_Head(const CYumbrozi_Head& Prototype):CYumbrozi_Parts(Prototype)
{
}

HRESULT CYumbrozi_Head::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CYumbrozi_Head::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	return S_OK;
}

void CYumbrozi_Head::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CYumbrozi_Head::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CYumbrozi_Head::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CYumbrozi_Head::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
	InheritPositon_Begin();


	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_AnimTexture()))	                return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))					    return E_FAIL;

	InheritPositon_End();
    
    return S_OK;
}

HRESULT CYumbrozi_Head::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Head"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(77.f / g_iCubicDot, 72.f / g_iCubicDot, 1.f);

	return S_OK;
}

CYumbrozi_Head* CYumbrozi_Head::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYumbrozi_Head* pInstance = new CYumbrozi_Head(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_Head::Clone(void* pArg)
{
	CYumbrozi_Head* pInstance = new CYumbrozi_Head(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYumbrozi_Head::Free()
{
    __super::Free();
}
