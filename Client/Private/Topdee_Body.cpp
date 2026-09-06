#include "Topdee_Body.h"
#include "GameInstance.h" 

CTopdee_Body::CTopdee_Body(LPDIRECT3DDEVICE9 pGraphic_Device) : CTopdee_Parts{ pGraphic_Device }
{

}

CTopdee_Body::CTopdee_Body(const CTopdee_Body& Prototype) : CTopdee_Parts(Prototype)
{
}

HRESULT CTopdee_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTopdee_Body::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float _fRatio = 0.4f;
	m_pTransformCom->Scale(3.0625f, 3.0625f, 0.f);
	
	if (pArg)
	{
		_int _iIdx = *reinterpret_cast<_int*>(pArg);

		m_iCurTextureIdx = _iIdx;
		_float3 _vPos = { 0 , float(_iIdx * 0.0625)-0.5f , -float(_iIdx * 0.0625) };
		m_pTransformCom->Set_State(STATE::POSITION, _vPos);

	}

	//CreateKeyContext();
	return S_OK;
}

void CTopdee_Body::Priority_Update(_float fTimeDelta)
{
}

void CTopdee_Body::Update(_float fTimeDelta)
{
	if (CClearPortal::m_bIsClear) return;

	m_fAngle = m_pParent->Get_TextureIdx()*10;
	
	m_pTransformCom->Rotation(_float3{ 0.f,0.f,1.f }, D3DXToRadian(m_fAngle));
}
	

void CTopdee_Body::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
}

HRESULT CTopdee_Body::Render()
{
	if (CClearPortal::m_bIsClear) return S_OK;

	_bool bIsConvert = m_pParent->Get_IsConvert();
	auto eState = m_pParent->Get_State();
	if (!bIsConvert && eState != CTopdee::DIE)
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

HRESULT CTopdee_Body::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)));

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TopdeeBody"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;
	return S_OK;
}

void CTopdee_Body::SetParent(CTopdee* ppGameObject)
{
	m_pParent = ppGameObject;
	/* Com_Transform */

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->SetParent(&TransformDesc);

}

CTopdee_Body* CTopdee_Body::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTopdee_Body* pInstance = new CTopdee_Body(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTopdee_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTopdee_Body::Clone(void* pArg)
{
	CTopdee_Body* pInstance = new CTopdee_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTopdee_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTopdee_Body::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDimensionCom);
}
