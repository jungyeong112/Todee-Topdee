#include "TopTookee.h"
#include "GameInstance.h" 
#include "Collider.h"
#include "Block.h"
#include "Tookee.h"
#include "TooTooKee.h"
CTopTookee::CTopTookee(LPDIRECT3DDEVICE9 pGraphic_Device) :CTookee(pGraphic_Device)
{
}

CTopTookee::CTopTookee(const CTopTookee& Prototype) : CTookee(Prototype)
{
}

HRESULT CTopTookee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTopTookee::Initialize(void* pArg)
{

	if (pArg)
	{
		auto pTooTookee = static_cast<CTooTooKee*>(pArg);
		auto pTransform = static_cast<CTransform*>(pTooTookee->Get_Component(L"Com_Transform"));

		if (FAILED(Ready_Components(pTransform)))
			return E_FAIL;
	}





	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);


	CreateKeyContext();
	return S_OK;
}

void CTopTookee::Priority_Update(_float fTimeDelta)
{

}

void CTopTookee::Update(_float fTimeDelta)
{
	m_bIsAble = !m_pParent->IsConvert();

	if (m_pGameInstance->IsKeyState('D', EKeyActionState::Enter))
	{
		m_eCurState = DIE;
	}
	auto [_eState, fRatio] = m_pGameInstance->Get_DimensionInfo();
	if (_eState == EDimensionState::TOPDIM)
	{
		m_pKeyContext_Move->SetActive(true);
		m_pKeyContext_Rot->SetActive(true);
		m_pColliderCom->SetActive(true);
	}
	else 
	{
		m_pKeyContext_Move->SetActive(false);
		m_pKeyContext_Rot->SetActive(false);
		m_pColliderCom->SetActive(false);
	}

	m_pTransformCom->Rotation(_float3{ -1.f, 0.f, 0.f }, D3DXToRadian(fRatio * 30));

	SetAngle();

	auto targetTransform = static_cast<CTransform*>(m_pTooTookee->Get_Component(L"Com_Transform"));
	auto target_Pos = targetTransform->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, target_Pos);



}

void CTopTookee::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
	TopTookee_MotionChange();
}

HRESULT CTopTookee::Render()
{
	CRenderStateGaurd _rsGaurd(m_pGraphic_Device);
	DWORD dwOldCullMode = 0;

	if (m_eCurState == DIE)
	{
		/*if (FAILED(m_pTransformDieCom->Bind_Resource())) return E_FAIL;
		if (FAILED(m_pDieTextureCom->Bind_AnimTexture()))     return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))       return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))             return E_FAIL;*/
	}
	return S_OK;
}

void CTopTookee::SetParent(CTookee* pTookee)
{
	m_pParent = pTookee;
	/* Com_Transform */

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->SetParent(&TransformDesc);
}


void CTopTookee::OnCollision(FCollisionInfo _fCollisionInfo)
{
	auto targetTransform = static_cast<CTransform*>(m_pTooTookee->Get_Component(L"Com_Transform"));
	auto target_Pos = targetTransform->Get_State(STATE::POSITION);

	if (m_eCurState != DISABLE)
	{
		if (_fCollisionInfo.strCollisionLayerTag == L"Wall" ||
			_fCollisionInfo.strCollisionLayerTag == L"FIX" ||
			_fCollisionInfo.strCollisionLayerTag == L"HOLE" ||
			//_fCollisionInfo.strCollisionLayerTag == L"KINETIC" ||
			_fCollisionInfo.strCollisionLayerTag == L"SPIKE"
			) {
			if (_fCollisionInfo.collisionType == CCollider::CF_Left)	target_Pos.x += _fCollisionInfo.vOverlapDepth.x;
			if (_fCollisionInfo.collisionType == CCollider::CF_Right)	target_Pos.x -= _fCollisionInfo.vOverlapDepth.x;
			if (_fCollisionInfo.collisionType == CCollider::CF_Top)		target_Pos.y -= _fCollisionInfo.vOverlapDepth.y;
			if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	target_Pos.y += _fCollisionInfo.vOverlapDepth.y;
			targetTransform->Set_State(STATE::POSITION, target_Pos);


		}
		if (_fCollisionInfo.strCollisionLayerTag == L"FireBall")
		{
			//m_eCurState = DIE;
		}
	}
	if (_fCollisionInfo.strCollisionLayerTag == L"KINETIC")
	{
		auto pBlock = static_cast<CBlock*>(_fCollisionInfo.pObject);
		if (pBlock->GetBlockFlag() == CBlock::BF_LIFT)
		{
			_bool bRes = false;
			_bool bDiff = false;
			_float fx = 0;
			_float fy = 0;

			if (_fCollisionInfo.collisionType == CCollider::CF_Left)	 fx = -1.f, fy = 0.f;
			if (_fCollisionInfo.collisionType == CCollider::CF_Right)	 fx = +1.f, fy = 0.f;
			if (_fCollisionInfo.collisionType == CCollider::CF_Top)		 fx = 0.f, fy = +1.f;
			if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)   fx = 0.f, fy = -1.f;

			bRes = pBlock->Search_Block(fx, fy);
			bDiff = pBlock->DiffKenetic();

			_bool bPush = false;
			switch (_fCollisionInfo.collisionType)
			{
			case CCollider::CF_Left:
				if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay)) bPush = true;
				break;
			case CCollider::CF_Right:
				if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay)) bPush = true;
				break;
			case CCollider::CF_Top:
				if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay)) bPush = true;
				break;
			case CCollider::CF_Bottom:
				if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay)) bPush = true;
				break;
			default:
				break;
			}

			if (bRes && bDiff && bPush)
			{
				pBlock->Push_Block(fx, fy);
				m_pGameInstance->PlaySoundW(L"pushSnd.wav", CHANNELID::SOUND_TOPDEE, 1.f);
			}

			else {
				if (_fCollisionInfo.collisionType == CCollider::CF_Left)	target_Pos.x += _fCollisionInfo.vOverlapDepth.x;
				if (_fCollisionInfo.collisionType == CCollider::CF_Right)	target_Pos.x -= _fCollisionInfo.vOverlapDepth.x;
				if (_fCollisionInfo.collisionType == CCollider::CF_Top)		target_Pos.y -= _fCollisionInfo.vOverlapDepth.y;
				if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	target_Pos.y += _fCollisionInfo.vOverlapDepth.y;
				targetTransform->Set_State(STATE::POSITION, target_Pos);
			}

		}
	}


}

CTopTookee* CTopTookee::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTopTookee* pInstance = new CTopTookee(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTopTookee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTopTookee::Clone(void* pArg)
{
	CTopTookee* pInstance = new CTopTookee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTopTookee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTopTookee::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDimensionCom);
	Safe_Release(m_pColliderCom);

}

void CTopTookee::Set_Position(_float3 v_Pos)
{
	m_pTransformCom->Set_State(STATE::POSITION, v_Pos);
}



void CTopTookee::CreateKeyContext()
{
	m_pKeyContext_Move = m_pGameInstance->CreateKeyContext(true);
	m_pKeyContext_Move->AddFunc(VK_RIGHT, [=]() {
		/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		vPos.x += 0.076f;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);*/
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_LEFT, [=]() {
		/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		vPos.x -= 0.076f;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);*/
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_UP, [=]() {
		/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		vPos.y += 0.076f;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);*/
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_DOWN, [=]() {
		/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		vPos.y -= 0.076f;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);*/
		}, EKeyActionState::Stay);

	//Enter
	m_pKeyContext_Rot = m_pGameInstance->CreateKeyContext(true);
	m_pKeyContext_Rot->AddFunc(VK_RIGHT, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				return;

			if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay))
				m_fAngle = 135.f;

			else  if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay))
				m_fAngle = 45.f;

			else
			{
				m_fAngle = 90.f;
			}
			m_eCurState = SIDE_WALK;
		}, EKeyActionState::Enter);


	m_pKeyContext_Rot->AddFunc(VK_LEFT, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				return;
			if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter))
			{
				m_fAngle = 225.f;
			}
			else  if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter))
			{
				if (m_fPreAngle == 0.f)
					m_fPreAngle = 360.f;
				m_fAngle = 315.f;
			}

			else
			{
				if (m_fPreAngle == 0.f)
					m_fPreAngle = 360.f;
				m_fAngle = 270.f;
			}
			m_eCurState = SIDE_WALK;
		}, EKeyActionState::Enter);

	m_pKeyContext_Rot->AddFunc(VK_DOWN, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay))
				return;
			if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				m_fAngle = 45.f;
			else if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				m_fAngle = 315.f;
			else
			{
				if (m_fPreAngle == 270.f)
					m_fAngle = 360.f;
				else
					m_fAngle = 0;
			}
			m_eCurState = WALK;
		}, EKeyActionState::Enter);

	m_pKeyContext_Rot->AddFunc(VK_UP, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay))
				return;
			if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				m_fAngle = 135.f;
			else if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				m_fAngle = 225.f;
			else
			{
				m_fAngle = 180.f;
			}
			m_eCurState = WALK;
		}, EKeyActionState::Enter);


	//Exit

	m_pKeyContext_Rot->AddFunc(VK_UP, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				m_fAngle = 90.f;
			else if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				m_fAngle = 270.f;
			else if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay))
				m_fAngle = 0.f;
			else
				m_fAngle = 180.f;

			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.y = round(_vPos.y);
			m_eCurState = IDLE;
			m_pTransformCom->Set_State(STATE::POSITION, _vPos);

		}, EKeyActionState::Exit);


	m_pKeyContext_Rot->AddFunc(VK_DOWN, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				m_fAngle = 90.f;
			else if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				m_fAngle = 270.f;
			else if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay))
				m_fAngle = 180.f;

			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.y = round(_vPos.y);
			m_eCurState = IDLE;
			m_pTransformCom->Set_State(STATE::POSITION, _vPos);
		}, EKeyActionState::Exit);

	m_pKeyContext_Rot->AddFunc(VK_RIGHT, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay))
				m_fAngle = 180.f;
			else if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay))
				m_fAngle = 0.f;
			else if (m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_LEFT, EKeyActionState::Stay))
				m_fAngle = 270.f;

			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.x = round(_vPos.x);

			m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			m_eCurState = IDLE;
		}, EKeyActionState::Exit);

	m_pKeyContext_Rot->AddFunc(VK_LEFT, [=]()
		{
			if (m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_UP, EKeyActionState::Stay))
				m_fAngle = 180.f;
			else if (m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_DOWN, EKeyActionState::Stay))
				m_fAngle = 360.f;
			else if (m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Enter) || m_pGameInstance->IsKeyState(VK_RIGHT, EKeyActionState::Stay))
				m_fAngle = 90.f;

			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			_vPos.x = round(_vPos.x);

			m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			m_eCurState = IDLE;
		}, EKeyActionState::Exit);
}

HRESULT CTopTookee::Ready_Components(CTransform* pTransform)
{

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 50.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TopdeeFake"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) { OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = pTransform;
	ColliderDesc.strCollisionLayerTag = TEXT("TopTookee");
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 0.9375;
	_collisionInfo.fWidth = 0.9375;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	//Com_Dimension
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;

	return S_OK;
}

void CTopTookee::SetAngle()
{
	if (m_fAngle != m_fPreAngle)
	{
		if (m_fAngle > m_fPreAngle)
		{
			m_iCurTextureIdx = (m_fPreAngle / 10) + 1;
			if (m_iCurTextureIdx == 36)
			{
				m_iCurTextureIdx = 0;
				m_fAngle = m_fPreAngle = 0;
			}
			if ((18 < (m_fAngle - m_fPreAngle) / 10))
			{
				if (5 != abs(m_fAngle - m_fPreAngle))
				{
					if (m_fPreAngle == 0)
						m_fPreAngle = 360.f;
					m_fPreAngle -= 10;
				}

			}
			else
			{
				if (5 != abs(m_fAngle - m_fPreAngle))
					m_fPreAngle += 10;
			}

		}
		else
		{
			m_iCurTextureIdx = (m_fPreAngle / 10) - 1;

			if (m_iCurTextureIdx == 36)
				m_iCurTextureIdx = 0;

			if ((18 < (m_fPreAngle - m_fAngle) / 10))
			{
				if (5 != abs(m_fAngle - m_fPreAngle))
				{
					if (m_fPreAngle == 360)
						m_fPreAngle = 0.f;
					m_fPreAngle += 10;
				}

			}
			else
			{
				if (5 != abs(m_fAngle - m_fPreAngle))
					m_fPreAngle -= 10;
			}
		}
	}
}

HRESULT CTopTookee::Set_Position()
{
	auto targetTransform = static_cast<CTransform*>(m_pTooTookee->Get_Component(L"Com_Transform"));
	_float3 target_Pos = _float3{0.f, 0.f, 0.5f};
	targetTransform->Set_State(STATE::POSITION, target_Pos);
	return S_OK;
}

void CTopTookee::Reset_TopTookee(_float3 _vPos)
{
	m_bIsDead = false;
	Set_Position(_vPos);
	m_bIsDead = false;
	m_eCurState = IDLE;

}

void CTopTookee::TopTookee_MotionChange()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case CTopTookee::IDLE:
			break;
		case CTopTookee::WALK:
			break;
		case CTopTookee::LIFT:
			break;
		case CTopTookee::PUT:
			break;
		case CTopTookee::DIE:
			m_pGameInstance->CreateKeyContext(false);
			m_pKeyContext_Move->SetActive(false);
			m_pKeyContext_Rot->SetActive(false);
			break;
		case CTopTookee::DISABLE:


			m_pColliderCom->SetActive(false);
			break;
		case CTopTookee::ABLE:
			m_pColliderCom->SetActive(true);
		case CTopTookee::STATE_END:
			break;
		}
	}
	if (m_eCurState != DISABLE && m_eCurState != ABLE)
		m_ePreState = m_eCurState;
	if (m_eCurState != DIE)
	{

	}
}




