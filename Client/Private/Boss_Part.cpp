#include "Boss_Part.h"
#include "Boss.h"

#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "DimensionSwitcher.h"
#include "GameInstance.h"

CBoss_Part::CBoss_Part(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject{ pGraphic_Device }
{
}

CBoss_Part::CBoss_Part(const CBoss_Part& Prototype) :CGameObject(Prototype)
{
}

HRESULT CBoss_Part::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Part::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pHitTimer = m_pGameInstance->CreateTimerWithFrequency(HITBLINKFREQ, true, [=]() {
		m_iBlinkCnt--; 
		}, false);

	return S_OK;
}

void CBoss_Part::Update(_float fTimeDelta)
{
	if (m_iBlinkCnt == 0) 
		m_pHitTimer->SetActive(false);
	m_bBlink = m_iBlinkCnt % 2;
}

void CBoss_Part::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CBoss_Part::Render()
{

	CHKFAIL(Hit_Render());
	return S_OK;
}

HRESULT CBoss_Part::SetParent(CBoss* ppGameObject)
{
	m_pParent = ppGameObject;
	if (!m_pParent)		return E_FAIL;
	return S_OK;
}

HRESULT CBoss_Part::Hit_Render()
{
	if (m_pHitTimer->IsActive() && m_bBlink) {
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_TFACTOR);
	}
	return S_OK;
}

void CBoss_Part::Hit_Reset()
{
	m_iBlinkCnt = HITBLINKCNT;
	m_pHitTimer->Restart();
}

HRESULT CBoss_Part::Ready_Components()
{
	/* Com_Rect*/
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc = {};
	TransformDesc.fRotationPerSec = 30.f;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	//Com_Dimension
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;

	return S_OK;
}

void CBoss_Part::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDimensionCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	
}
