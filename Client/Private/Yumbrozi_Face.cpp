#include "Yumbrozi_Face.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

CYumbrozi_Face::CYumbrozi_Face(LPDIRECT3DDEVICE9 pGraphic_Device) : CYumbrozi_Parts(pGraphic_Device)
{
}

CYumbrozi_Face::CYumbrozi_Face(const CYumbrozi_Face& Prototype) :CYumbrozi_Parts(Prototype)
{
}

HRESULT CYumbrozi_Face::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CYumbrozi_Face::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	m_pTextureCom->SetAnimationDuration(3.f);

	return S_OK;
}

void CYumbrozi_Face::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CYumbrozi_Face::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CYumbrozi_Face::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CYumbrozi_Face::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
		InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_AnimTexture()))	                return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

CYumbrozi_Face* CYumbrozi_Face::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CYumbrozi_Face* pInstance = new CYumbrozi_Face(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi_Face");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi_Face::Clone(void* pArg)
{

	CYumbrozi_Face* pInstance = new CYumbrozi_Face(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi_Face");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void CYumbrozi_Face::Free()
{
	__super::Free();
}

_uint CYumbrozi_Face::GetTextureIdx()
{
	auto _vPos = m_pBaseTransform->Get_State(STATE::POSITION);
	auto _fAngle = atan2(m_vOffset.z - _vPos.z, (m_bFlip) ? -(m_vOffset.x - _vPos.x) : (m_vOffset.x - _vPos.x));
	_float _fAngleOffset = (2.f * D3DX_PI / 13);
	_fAngle -= D3DX_PI * 0.5f + _fAngleOffset * 1.5f;

	_fAngle = wrap(_fAngle, 0.f, 2 * D3DX_PI);

	return min(_uint(_fAngle / _fAngleOffset), 12);
}

HRESULT CYumbrozi_Face::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE2), TEXT("Prototype_Component_Texture_Boss_Yumbrozi_Face"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(70.f / g_iCubicDot, 37.f / g_iCubicDot, 1.f);

	return S_OK;
}
