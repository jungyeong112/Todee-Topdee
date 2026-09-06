#include "Tookee_SpinningHead.h"
#include "GameInstance.h"
#include "Collider.h"

CTookee_SpinningHead::CTookee_SpinningHead(LPDIRECT3DDEVICE9 pGraphic_Device) : CTookee_Parts{ pGraphic_Device }
{
}

CTookee_SpinningHead::CTookee_SpinningHead(const CTookee_SpinningHead& Prototype)
	:CTookee_Parts(Prototype)
{

}

HRESULT CTookee_SpinningHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTookee_SpinningHead::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pUpdownTimer = m_pGameInstance->CreateTimerWithDuration(0.25f, true, [=]() {
		m_iDir *= -1;
		}, true
	);
	_float3 _vPos = { 0.f, 0.4f, -float(10 * 0.0625) };
	m_fOriginPos = _vPos;
	m_pTransformCom->Scale(g_fDot*17, g_fDot*10, 0.f);
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);

	return S_OK;
}

void CTookee_SpinningHead::Priority_Update(_float fTimeDelta)
{
}

void CTookee_SpinningHead::Update(_float fTimeDelta)
{
	auto [_, fRatio] = m_pGameInstance->Get_DimensionInfo();

	m_pDimensionCom->UpdateState();

	m_iCurTextureIdx = m_pParent->Get_TextureIdx();
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	auto eState = m_pParent->Get_State();


	if (m_pParent->Get_State() == CTopTookee::ETOPDEESTATE::WALK)
	{
		_float Ratio = m_pUpdownTimer->GetElapsedTime();
		_float fOffset = sin(m_iDir * Ratio * fTimeDelta);
		_vPos.x += 3.f * fOffset;
	}
	else
	{
		_vPos = m_fOriginPos;
	}

	if (m_pParent->Get_State() == CTopTookee::ETOPDEESTATE::IDLE ||
		m_pParent->Get_State() == CTopTookee::ETOPDEESTATE::SIDE_WALK)
	{
		_float Ratio = m_pUpdownTimer->GetElapsedTime();

		_float fOffset = sin(m_iDir * Ratio * fTimeDelta);
		_vPos.y += 10.f * fOffset;
	}
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
}

void CTookee_SpinningHead::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
}

HRESULT CTookee_SpinningHead::Render()
{
	_bool bIsConvert = m_pParent->Get_Able();
	auto eState = m_pParent->Get_State();

	if (!bIsConvert && eState != CTopTookee::DIE)
	{
		DWORD dwOldCullMode = 0;
		m_pGraphic_Device->GetRenderState(D3DRS_CULLMODE, &dwOldCullMode);
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x80);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		if (FAILED(m_pTransformCom->Bind_Resource()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(m_iCurTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, dwOldCullMode);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	return S_OK;
}

HRESULT CTookee_SpinningHead::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeSpinningHead"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;

	return S_OK;
}

CTookee_SpinningHead* CTookee_SpinningHead::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTookee_SpinningHead* pInstance = new CTookee_SpinningHead(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTookee_SpinningHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTookee_SpinningHead::Clone(void* pArg)
{
	CTookee_SpinningHead* pInstance = new CTookee_SpinningHead(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTookee_SpinningHead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTookee_SpinningHead::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDimensionCom);
}

void CTookee_SpinningHead::SetParent(CTopTookee* ppGameObject)
{
	m_pParent = ppGameObject;
	/* Com_Transform */

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->SetParent(&TransformDesc);
}
