#include "Tookee.h"
#include "GameInstance.h" 
#include "Collider.h"
#include "Topdee.h"
#include "Toodee.h"

CTookee::CTookee(LPDIRECT3DDEVICE9 pGraphic_Device) : CGameObject(pGraphic_Device)
{
}

CTookee::CTookee(const CTookee& Prototype) : CGameObject(Prototype)
{
}

HRESULT CTookee::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CTookee::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3 _vPos = { 5.f, 14.f, 0.45f };
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	return S_OK;
}

void CTookee::Priority_Update(_float fTimeDelta)
{
}

void CTookee::Update(_float fTimeDelta)
{
	Move();
}

void CTookee::Late_Update(_float fTimeDelta)
{
}

HRESULT CTookee::Render()
{
	return S_OK;
}

void CTookee::Move()
{
	auto [_eState, _Ratio] = m_pGameInstance->Get_DimensionInfo();

	if (_eState == EDimensionState::TWODIM || _eState == EDimensionState::TOTWODIM)
	{
		m_bIsConvert = false;
		m_pTargetTransformCom = static_cast<CTransform*>(m_pToodee->Get_Component(L"Com_Transform"));
	}
	else if (_eState == EDimensionState::TOPDIM || _eState == EDimensionState::TOTOPDIM)
	{
		m_bIsConvert = true;
		m_pTargetTransformCom = static_cast<CTransform*>(m_pTopdee->Get_Component(L"Com_Transform"));
	
	}


	auto _targetPos = m_pTargetTransformCom->Get_State(STATE::POSITION);
	m_pairCurPos.first = _targetPos.x;
	m_pairCurPos.second = _targetPos.y;

	if (!m_pairPrePos.first && !m_pairPrePos.second)
	{
		m_pairPrePos.first = m_pairCurPos.first;
		m_pairPrePos.second = m_pairCurPos.second;
	}

	if (_eState == EDimensionState::TOPDIM)
	{
		if ((m_pairCurPos.first != m_pairPrePos.first || m_pairCurPos.second != m_pairPrePos.second)
			&&!m_pTopdee->Get_IsTelePorting())
		{
			_float fKeneticX = m_pairCurPos.first - m_pairPrePos.first;
			_float fKeneticY = m_pairCurPos.second - m_pairPrePos.second;

			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.x += fKeneticX;
			_vPos.y += fKeneticY;
			m_pTransformCom->Set_State(STATE::POSITION, _vPos);

			m_pairPrePos = m_pairCurPos;
		}
		if (m_pTopdee->Get_IsTelePorting())
			m_pairPrePos = {};
	}
	else if (_eState == EDimensionState::TWODIM)
	{

		if (m_pairCurPos.first != m_pairPrePos.first && !m_pToodee->Get_IsTeleporting())
		{
			_float fKeneticX = m_pairCurPos.first - m_pairPrePos.first;
			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.x += fKeneticX;
			m_pTransformCom->Set_State(STATE::POSITION, _vPos);

			m_pairPrePos = m_pairCurPos;
		}
		if (m_pToodee->Get_IsTeleporting())
		{
			m_pairPrePos = {};
		}
		
	}
	else {
		m_pairPrePos.first	= m_pairCurPos.first;
		m_pairPrePos.second = m_pairCurPos.second;
	}



}

void CTookee::Set_Position(_float3 vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pairCurPos = {};
	m_pairPrePos = {};
}

HRESULT CTookee::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 50.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CTookee::OnCollision(FCollisionInfo _fCollisionInfo)
{
}

CTookee* CTookee::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTookee* pInstance = new CTookee(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTookee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTookee::Clone(void* pArg)
{
	CTookee* pInstance = new CTookee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTookee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTookee::Free()
{
	__super::Free();
	//Safe_Release(m_pTopdee);
	//Safe_Release(m_pToodee);
	Safe_Release(m_pTransformCom);
}
