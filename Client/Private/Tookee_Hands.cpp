#include "Tookee_Hands.h"
#include "GameInstance.h" 


CTookee_Hands::CTookee_Hands(LPDIRECT3DDEVICE9 pGraphic_Device) : CTookee_Parts{ pGraphic_Device }
{
}

CTookee_Hands::CTookee_Hands(const CTookee_Hands& Prototype) : CTookee_Parts(Prototype)
{
}

HRESULT CTookee_Hands::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTookee_Hands::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pUpdownTimer = m_pGameInstance->CreateTimerWithDuration(0.4f, true, [=]() {
		m_iDir *= -1;
		}, true
	);


	if (pArg)
	{
		_int* _iIdx = reinterpret_cast<_int*>(pArg);

		m_iCurTextureIdx = _iIdx[1];
		m_pTransformCom->Scale(g_fDot * 49, g_fDot * 49, 1.f);
		_float3 _vPos = { 0, float((_iIdx[1] * -0.0625) - 0.0625) , -float(7 * 0.0625) };
		m_fOriginPos = _vPos;
		m_pTransformCom->Set_State(STATE::POSITION, _vPos);
		


		if (_iIdx[0])
		{
			m_pTransformCom->Final_Scale(-1.f, 1.f, 1.f);
		}

	}


	return S_OK;
}

void CTookee_Hands::Priority_Update(_float fTimeDelta)
{
}

void CTookee_Hands::Update(_float fTimeDelta)
{
	auto eState = m_pParent->Get_State();
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);

	if (eState == CTopTookee::IDLE || eState == CTopTookee::WALK || eState == CTopTookee::SIDE_WALK)
	{
		_float Ratio = m_pUpdownTimer->GetElapsedTime();
		_float fOffset = sin(m_iDir * Ratio * fTimeDelta);
		_vPos.y += 1.f * fOffset;
	}
	else
	{
		_vPos = m_fOriginPos;
	}

	m_fAngle = m_pParent->Get_TextureIdx() * 10;
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	m_pTransformCom->Rotation(_float3{ 0.f,0.f,1.f }, D3DXToRadian(m_fAngle));
}

void CTookee_Hands::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
}

HRESULT CTookee_Hands::Render()
{
	DWORD dwOldCullMode = 0;
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x80);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	_bool bIsAble = m_pParent->Get_Able();

	auto  eState = m_pParent->Get_State();

	if (!bIsAble && eState != CTopTookee::DIE)
	{
		if (FAILED(m_pTransformCom->Bind_Resource()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(m_iCurTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, dwOldCullMode);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	return S_OK;
}

CTookee_Hands* CTookee_Hands::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTookee_Hands* pInstance = new CTookee_Hands(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTookee_Hands");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTookee_Hands::Clone(void* pArg)
{
	CTookee_Hands* pInstance = new CTookee_Hands(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTookee_Hands");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTookee_Hands::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDimensionCom);
}

HRESULT CTookee_Hands::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeHands"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;
	return S_OK;
}

void CTookee_Hands::SetParent(CTopTookee* ppGameObject)
{
	m_pParent = ppGameObject;
	/* Com_Transform */

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->SetParent(&TransformDesc);
}