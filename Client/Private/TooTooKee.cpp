#include "TooTooKee.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Block.h"
#include "PortalLink.h"

CTooTooKee::CTooTooKee(LPDIRECT3DDEVICE9 pGraphic_Device) : CTookee{ pGraphic_Device }
{
}

CTooTooKee::CTooTooKee(const CTooTooKee& Prototype)
	:CTookee(Prototype)
{

}

HRESULT CTooTooKee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTooTooKee::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*_float3 _vPos = { 20.f, 14.f, 0.45f };
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);*/

	_float3		vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_float3		vUp = m_pTransformCom->Get_State(STATE::UP);
	_float3		vLook = m_pTransformCom->Get_State(STATE::LOOK);

	m_pTransformCom->Set_State(STATE::RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_fToodeeSize);
	m_pTransformCom->Set_State(STATE::UP, *D3DXVec3Normalize(&vUp, &vUp) * m_fToodeeSize);
	m_pTransformCom->Set_State(STATE::LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_fToodeeSize);

	m_pTextureIDLE->SetAnimationDuration(3.f);
	m_pTextureRUN->SetAnimationDuration(0.8f);
	m_pTextureEDGE->SetAnimationDuration(1.f);
	//m_pTextureDIE->SetAnimationDuration(1.f);


	m_pCurTextureCom = m_pTextureIDLE;

	m_pJumpLimitTimer = m_pGameInstance->CreateTimerWithDuration(0.21f, false, [this]() {m_bJumpLimit = true; }, false);
	m_pDeadAnimTimer = m_pGameInstance->CreateTimerWithDuration(0.1f, true, [this]() { ++m_iDieTextureIdx; }, false);
	CreateKeyContext();

	m_pDimensionCom->Set_Func(EDimensionState::TOPDIM, EDimActionState::Exit, [=]() { m_pColliderCom->SetActive(true); });

	//CPortalLink::GetInstance()->Set_Toodee(this);

	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	
	return S_OK;
}

void CTooTooKee::Priority_Update(_float fTimeDelta)
{
}

void CTooTooKee::Update(_float fTimeDelta)
{
	TOOTOOKEEPOS = m_pTransformCom->Get_WorldState(STATE::POSITION);
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);

	m_bIsAble = !m_pParent->IsConvert();
	auto [_eState, _] = m_pGameInstance->Get_DimensionInfo();

	if (m_bIsAble && _eState == EDimensionState::TWODIM)
	{

		time = 0;
		if (m_pCurTextureCom != m_pTextureJUMP)
		{
			m_pCurTextureCom->SetActive_AnimTexture(true);
		}
		m_pKeyContext->SetActive(true);

		m_pColliderCom->SetActive(true);
		if (!m_bIsDeadAnim)
		{
			ChangeLR();
			Squash();
			Move(fTimeDelta);
			Jump(fTimeDelta);
		}
		if (m_eCurState != m_ePreState)
			Tookee_MotionChange(m_eCurState);
	}
	else
	{

		m_pColliderCom->SetActive(false);
		time += 0.016;// 누적 시간
		if (m_eCurState == RUN)
		{
			m_eCurState = IDLE;
		}
		if (m_fSpeedY > 0)
		{
			m_fSpeedY = 0;
			m_fGravity = -64.f;
		}
		m_bIsAble = false;
		if (m_pCurTextureCom != m_pTextureJUMP)
		{
			m_pCurTextureCom->SetActive_AnimTexture(false);
		}
		m_pKeyContext->SetActive(false);
	}

	m_pDimensionCom->UpdateState();

}


void CTooTooKee::Late_Update(_float fTimeDelta)
{
	Set_Scale();

	m_bWasOnGround = m_bIsGround;
	m_bIsGround = false;
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);

	m_bLeftInput = false;
	m_bRightInput = false;
}


HRESULT CTooTooKee::Render()
{
	CRenderStateGaurd _rsGaurd(m_pGraphic_Device);
	DWORD dwOldCullMode = 0;
	m_pGraphic_Device->GetRenderState(D3DRS_CULLMODE, &dwOldCullMode);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	auto [_eState, _] = m_pGameInstance->Get_DimensionInfo();


	// Setting Alpha Blend/Test && Make Outline Before Rendering Player 
	if (_eState == EDimensionState::TWODIM || _eState== EDimensionState::TOTWODIM )
	{


		Render_Shadow();


		float brightness = 0.5f;

		// Save Old Color
		DWORD oldColorOp, oldColorArg1, oldColorArg2;
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg1);
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG2, &oldColorArg2);

		// Set Bright Color
		D3DCOLOR dimColor = D3DCOLOR_COLORVALUE(brightness, brightness, brightness, 1.0f);
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, dimColor);

		// Texture Calc
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		// Alpha Test
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x80);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);




		// ColorOp OFF
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, oldColorOp);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, oldColorArg1);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, oldColorArg2);


	}
	else
	{
		// Alpha Blend (Lower Transparency)

		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(_uint(m_fTransparency * 255), 255, 255, 255));

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}


	// Render Player
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (m_eCurState == JUMP)
	{
		if (FAILED(m_pCurTextureCom->Bind_Texture(m_iCurTextureIdx)))
			return E_FAIL;
	}
	else if (m_eCurState == DIE)
	{
		if (FAILED(m_pCurTextureCom->Bind_Texture(m_iDieTextureIdx)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pCurTextureCom->Bind_AnimTexture()))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// Alpha Blend/Test Setting Off
	if (!m_bIsAble)
	{
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	}
	else
	{
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}


	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, dwOldCullMode);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	return S_OK;
}

void CTooTooKee::SetParent(CTookee* pTookee)
{
	m_pParent = pTookee;
	/* Com_Transform */

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));

	m_pTransformCom->SetParent(&TransformDesc);
}


HRESULT CTooTooKee::Ready_Components()
{
	//Toodee buffer
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 6.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	//Com_Texture
	// Toodee
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeIDLE"),
		TEXT("Com_TextureIDLE"), reinterpret_cast<CComponent**>(&m_pTextureIDLE))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeRUN"),
		TEXT("Com_TextureRUN"), reinterpret_cast<CComponent**>(&m_pTextureRUN))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeJUMP"),
		TEXT("Com_TextureJUMP"), reinterpret_cast<CComponent**>(&m_pTextureJUMP))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeDIE"),
		TEXT("Com_TextureDIE"), reinterpret_cast<CComponent**>(&m_pTextureDIE))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TookeeEDGE"),
		TEXT("Com_TextureEDGE"), reinterpret_cast<CComponent**>(&m_pTextureEDGE))))
		return E_FAIL;




	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) { OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pTransformCom;
	ColliderDesc.strCollisionLayerTag = TEXT("TooTookee");
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 1.35;
	_collisionInfo.fWidth = 0.9375;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;

	return S_OK;
}

CTooTooKee* CTooTooKee::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTooTooKee* pInstance = new CTooTooKee(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTooTooKee");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTooTooKee::Clone(void* pArg)
{
	CTooTooKee* pInstance = new CTooTooKee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTooTooKee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTooTooKee::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureIDLE);
	Safe_Release(m_pTextureRUN);
	Safe_Release(m_pTextureJUMP);
	Safe_Release(m_pTextureEDGE);
	Safe_Release(m_pTextureDIE);
	Safe_Release(m_pDimensionCom);
	Safe_Release(m_pColliderCom);

}

void CTooTooKee::OnCollision(FCollisionInfo _fCollisionInfo)
{
	if (m_bIsAble)
	{

		//Collision with Fixed Object
		if (_fCollisionInfo.strCollisionLayerTag == L"Wall" ||
			_fCollisionInfo.strCollisionLayerTag == L"FIX" ||
			_fCollisionInfo.strCollisionLayerTag == L"KINETIC" ||
			_fCollisionInfo.strCollisionLayerTag == L"SPIKE"

			) {
			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			if (_fCollisionInfo.collisionType == CCollider::CF_Left)
			{
				if (m_fSpeedY)
					m_fPreSpeedY = m_fSpeedY;
				_vPos.x += _fCollisionInfo.vOverlapDepth.x;
			}

			if (_fCollisionInfo.collisionType == CCollider::CF_Right)
			{
				if (m_fSpeedY)
					m_fPreSpeedY = m_fSpeedY;
				_vPos.x -= _fCollisionInfo.vOverlapDepth.x;
			}

			if (_fCollisionInfo.collisionType == CCollider::CF_Top)
			{
				if (m_fSpeedY)
					m_fPreSpeedY = m_fSpeedY;
				_vPos.y -= _fCollisionInfo.vOverlapDepth.y;
				if (m_fSpeedY > 0)
					m_fSpeedY = 0;
			}

			if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)
			{
				if (m_fSpeedY)
					m_fPreSpeedY = m_fSpeedY;
				if (_fCollisionInfo.strCollisionLayerTag == L"SPIKE")
				{
					Set_IsDead();
				}
				_vPos.y += _fCollisionInfo.vOverlapDepth.y;
				m_bIsGround = true;
			}



			m_pTransformCom->Set_State(STATE::POSITION, _vPos);
		}
	}
	else
	{
		auto [curState, fRatio] = m_pGameInstance->Get_DimensionInfo();
		if (curState == EDimensionState::TOTWODIM)
		{
			Topdim_Overlapped(_fCollisionInfo);
		}
	}
	//Collision With Non-Fixed Object
	if (_fCollisionInfo.strCollisionLayerTag == L"CLEAR_PORTAL")
	{
		m_bIsClear = true;
	}

}

void CTooTooKee::Topdim_Overlapped(FCollisionInfo _fCollisionInfo)
{
	if (
		_fCollisionInfo.strCollisionLayerTag == L"KINETIC"
		) {
		auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);

		_vPos.y += _fCollisionInfo.vOverlapDepth.y;

		m_pTransformCom->Set_State(STATE::POSITION, _vPos);

	}
}

void CTooTooKee::Reset_Toodee(_float3 vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	m_bIsDeadAnim = false;
	m_bIsDead = false;
	m_pDeadAnimTimer->Reset();
	m_iDieTextureIdx = 0;
	m_eCurState = IDLE;
}

void CTooTooKee::CreateKeyContext()
{
	m_pKeyContext = m_pGameInstance->CreateKeyContext(false);

	m_pKeyContext->AddFunc(VK_RIGHT, [=]() {
		if (m_bIsGround)
			m_eCurState = RUN;
		m_bIsCurRight = true;
		m_bRightInput = true;
		}, EKeyActionState::Stay);

	m_pKeyContext->AddFunc(VK_RIGHT, [=]() {
		if (m_bIsGround)
			m_eCurState = IDLE;

		}, EKeyActionState::Exit);

	m_pKeyContext->AddFunc(VK_LEFT, [=]() {
		if (m_bIsGround)
			m_eCurState = RUN;
		m_bIsCurRight = false;
		m_bLeftInput = true;
		}, EKeyActionState::Stay);
	m_pKeyContext->AddFunc(VK_LEFT, [=]() {
		if (m_bIsGround)
			m_eCurState = IDLE;

		}, EKeyActionState::Exit);


	//Jump
	m_pKeyContext->AddFunc('Z', [=]() {
		if (m_bIsGround)
		{
			m_fSpeedY = 14.f;
			m_bJumpLimit = false;
			m_pJumpLimitTimer->Restart();
			m_bIsGround = false;
			m_eCurState = JUMP;
		};

		}, EKeyActionState::Enter);

	m_pKeyContext->AddFunc('Z', [=]() {

		if (m_bJumpLimit) {
			m_fGravity = -64.f;
		}
		else { m_fGravity = 0.f; };

		}, EKeyActionState::Stay);

	m_pKeyContext->AddFunc('Z', [=]() {
		m_bJumpLimit = true;
		m_fGravity = -64.f;
		}, EKeyActionState::Exit);





}

void CTooTooKee::Tookee_MotionChange(ETOODEESTATE _eCurState)
{
	switch (_eCurState)
	{
	case IDLE:
		m_pCurTextureCom = m_pTextureIDLE;
		break;
	case RUN:
		m_pCurTextureCom = m_pTextureRUN;
		break;
	case JUMP:
		m_pCurTextureCom = m_pTextureJUMP;
		break;
	case EDGE:
		m_pCurTextureCom = m_pTextureEDGE;
		break;
	case DIE:
		m_pCurTextureCom = m_pTextureDIE;
		break;
	case CLEAR:
		break;
	case DISABLE:
		break;
	case ENTER:
		break;
	case TDST_END:

		break;

	}

	m_ePreState = m_eCurState;
}


void CTooTooKee::Jump(_float fTimeDelta)
{

	_float3 vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	m_fSpeedY += m_fGravity * fTimeDelta;
	m_fSpeedX -= m_fSpeedX * fTimeDelta;

	m_fSpeedY = max(m_fSpeedY, -30.f);
	vPosition.y += m_fSpeedY * fTimeDelta + (m_fGravity * fTimeDelta * fTimeDelta) * 0.5f;

	if (m_fSpeedX)
	{
		vPosition.x += m_fSpeedX * fTimeDelta;
	}



	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	if (!m_bIsGround)
	{
		m_eCurState = JUMP;
		Change_JumpTexIdx();
	}
	else
	{
		if (!m_bIsTeleporting)
		{
			m_fSpeedY = 0.f;
			m_fSpeedX = 0.f;
		}
		else
		{
			m_bIsTeleporting = false;
			m_fPreSpeedY = 0.f;
		}
		m_pJumpLimitTimer->Reset();
		if (!m_bWasOnGround)
		{
			m_eCurState = IDLE;
			m_iCurTextureIdx = 0;
		}
	}

}

void CTooTooKee::Move(float _fTimeDelta)
{
	if (m_bRightInput || m_bLeftInput)
	{
		if (m_bRightInput && m_bLeftInput)
		{
			if (m_eCurState == RUN)
				m_eCurState = IDLE;
		}
		else
		{
		/*	auto pTransform = static_cast<CTransform*>(m_pParent->Get_Component(L"Com_Transform"));
			pTransform->Go_Right(_fTimeDelta);*/
		}

	}
}

void CTooTooKee::Set_Scale()
{

	if (m_eCurState == JUMP)
	{
		if (m_fSpeedY > 0)
		{
			if (m_fToodeeSize * (1.f - (0.018 * m_fSpeedY)) > 0)
			{
				_float ScaleX = m_fToodeeSize * (1.f - (0.018 * m_fSpeedY));
				_float ScaleY = m_fToodeeSize * (1.f + (0.036 * m_fSpeedY));

				m_pTransformCom->Scale(ScaleX, ScaleY, 0.f);
			}
		}
		if (m_fSpeedY < 0)
		{
			if (m_fToodeeSize * (1.f - (0.018 * m_fSpeedY)) > 0)
			{
				_float ScaleX = m_fToodeeSize * (1.f + (0.018 * m_fSpeedY));
				_float ScaleY = m_fToodeeSize * (1.f - (0.018 * m_fSpeedY));


				m_pTransformCom->Scale(ScaleX, ScaleY, 0.f);
			}
		}
	}

}

void CTooTooKee::Squash()
{
	if (m_bIsGround && !m_bWasOnGround)
	{
		_float ScaleX = m_fToodeeSize * (1.f + (0.03125 * abs(m_fSpeedY)));
		_float ScaleY = m_fToodeeSize * (1.f - (0.023 * abs(m_fSpeedY)));

		m_pTransformCom->Scale(ScaleX, ScaleY, 0.f);
	}
	else if (m_bIsGround && m_bWasOnGround)
	{

		_float3 Scale = m_pTransformCom->Get_Scaled();
		Scale.x = Lerp(m_fToodeeSize, Scale.x, 0.5f);
		Scale.y = Lerp(Scale.y, m_fToodeeSize, 0.5f);

		m_pTransformCom->Scale(Scale.x, Scale.y, 0.f);

	}
}

void CTooTooKee::Change_JumpTexIdx()
{
	if (m_eCurState != DIE)
	{
		if (m_fSpeedY > 1.5f)
		{
			m_iCurTextureIdx = 0;
		}
		else if (m_fSpeedY < 3 && m_fSpeedY > 1)
		{
			m_iCurTextureIdx = 1;
		}
		else if (m_fSpeedY < 1 && m_fSpeedY > -1)
		{
			m_iCurTextureIdx = 2;
		}
		else if (m_fSpeedY < -1 && m_fSpeedY >-3)
		{
			m_iCurTextureIdx = 3;
		}
		else
		{
			m_iCurTextureIdx = 4;
		}
	}
}

void CTooTooKee::ChangeLR()
{
	if (m_bIsCurRight != m_bIsPreRight)
	{
		_float3		vRight = m_pTransformCom->Get_State(STATE::RIGHT);
		m_pTransformCom->Set_State(STATE::RIGHT, vRight * -1.f);
		m_bIsPreRight = m_bIsCurRight;
	}
}

bool CTooTooKee::Check_OnAir(bool _bIsCol)
{
	if (_bIsCol == false && m_bIsGround == true)
	{
		return true;
	}
	else
		return false;
}


HRESULT CTooTooKee::Render_Shadow()
{

	// Save Old Color
	DWORD oldColorOp, oldColorArg1, oldColorArg2;
	m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
	m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg1);
	m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG2, &oldColorArg2);

	// Texture Calc
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	// Alpha Blend (Lower Transparency)

	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(150, 0, 0, 0));

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	vPos.x -= 0.4f;
	vPos.y -= 0.3f;
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (m_eCurState == JUMP || m_eCurState == DIE)
	{
		if (FAILED(m_pCurTextureCom->Bind_Texture(m_iCurTextureIdx)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pCurTextureCom->Bind_AnimTexture()))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	vPos = m_pTransformCom->Get_State(STATE::POSITION);
	vPos.x += 0.4f;
	vPos.y += 0.3f;
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	// ColorOp OFF
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, oldColorOp);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, oldColorArg1);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, oldColorArg2);

	//Alpha Blend Off
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



}
