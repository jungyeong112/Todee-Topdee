#include "Yumbrozi_Hand.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

CYumbrozi_Hand::CYumbrozi_Hand(LPDIRECT3DDEVICE9 pGraphic_Device) : CYumbrozi_Parts(pGraphic_Device)
{
}

CYumbrozi_Hand::CYumbrozi_Hand(const CYumbrozi_Hand& Prototype) :CYumbrozi_Parts(Prototype)
{
}

HRESULT CYumbrozi_Hand::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CYumbrozi_Hand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	m_pTextureCom->SetAnimationDuration(3.f);
	m_iTextureIdx = 4;
	return S_OK;
}

void CYumbrozi_Hand::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CYumbrozi_Hand::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if (m_pTextureCom->IsEnd())
		m_bIsAnim = false;
}

void CYumbrozi_Hand::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CYumbrozi_Hand::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
		InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

	if (m_bIsAnim)
	{
		m_pTextureCom->SetActive_AnimTexture(true);
		m_pTextureCom->Bind_AnimTexture();
	}
	else 
	{
		m_pTextureCom->Bind_Texture(m_iTextureIdx);
		m_pTextureCom->SetActive_AnimTexture(false);
	}
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

CYumbrozi_Hand* CYumbrozi_Hand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CYumbrozi_Hand* pInstance = new CYumbrozi_Hand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_Hand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_Hand::Clone(void* pArg)
{

	CYumbrozi_Hand* pInstance = new CYumbrozi_Hand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_Hand");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void CYumbrozi_Hand::Free()
{
	__super::Free();
}

_uint CYumbrozi_Hand::GetTextureIdx()
{
	auto _vPos = m_pBaseTransform->Get_State(STATE::POSITION);
	auto _fAngle = atan2(m_vOffset.z - _vPos.z, (m_bFlip) ? -(m_vOffset.x - _vPos.x) : (m_vOffset.x - _vPos.x));
	_float _fAngleOffset = (2.f * D3DX_PI / 13);
	_fAngle -= D3DX_PI * 0.5f + _fAngleOffset * 1.5f;

	_fAngle = wrap(_fAngle, 0.f, 2 * D3DX_PI);

	return min(_uint(_fAngle / _fAngleOffset), 12);
}

HRESULT CYumbrozi_Hand::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Hand"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(77.f / g_iCubicDot, 72.f / g_iCubicDot, 1.f);

	return S_OK;
}
