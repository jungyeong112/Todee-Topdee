#include "Topdee.h"
#include "GameInstance.h" 
#include "Collider.h"
#include "Block.h"
#include "Block_Lightning.h"

CTopdee::CTopdee(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject(pGraphic_Device)
{
}

CTopdee::CTopdee(const CTopdee& Prototype) : CGameObject(Prototype)
{
}

HRESULT CTopdee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTopdee::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float3 _vPos = { 15 , 15 , 0.5f };

	m_pTransformDieCom->Set_State(STATE::POSITION, _vPos);
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	m_pTransformAnimCom->Set_State(STATE::POSITION, _vPos);

	m_pTransformAnimCom->Scale(1.0625f, 1.5625f, 1.f);
	m_pTransformDieCom->Scale(g_fDot * 50, g_fDot * 50, 1.f);
	m_pDieTextureCom->SetAnimationDuration(1.f);

	CreateKeyContext();
	CreateFootStepTimer();
	return S_OK;
}

void CTopdee::Priority_Update(_float fTimeDelta)
{
	_bHoldingLightningBlock = false;
	if (m_pHoldingObject) {
		if (dynamic_cast<CBlock_Lightning*>(m_pHoldingObject)) _bHoldingLightningBlock = true;
	}
}

void CTopdee::Update(_float fTimeDelta)
{
	if (CClearPortal::m_bIsClear) return;
	//move
	_vDir = {0.f,0.f, 0.f};
	if (m_pGameInstance->IsKeyState(VK_RIGHT,	EKeyActionState::Stay)) _vDir.x += 1;
	if (m_pGameInstance->IsKeyState(VK_LEFT,	EKeyActionState::Stay)) _vDir.x -= 1;
	if (m_pGameInstance->IsKeyState(VK_UP,		EKeyActionState::Stay)) _vDir.y += 1;
	if (m_pGameInstance->IsKeyState(VK_DOWN,	EKeyActionState::Stay)) _vDir.y -= 1;
	D3DXVec3Normalize(&_vDir, &_vDir);
	if (m_pKeyContext_Move->IsActive()) {
		_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		vPos += _vDir * fTimeDelta * 5.5f;
		m_pTransformCom->Set_State(STATE::POSITION, vPos);
	}

	m_pFootStepTimer->SetActive((m_eCurState == WALK || m_eCurState == SIDE_WALK));
	

	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformDieCom->Set_State(STATE::POSITION, _vPos);
	_vPos.y += 0.1875f;
	m_pTransformAnimCom->Set_State(STATE::POSITION, _vPos);

	if (m_eCurState != DISABLE)
		m_bIsLift = m_pHoldingObject == nullptr ? false : true;

	

	if (m_pGameInstance->IsKeyState('D', EKeyActionState::Enter))
	{
		Set_IsDead();
	}

	if (m_pGameInstance->IsKeyState('Z', EKeyActionState::Enter) && m_eCurState != DISABLE) {

		Search_Table();
		if (m_pHoldingObject)
			Put_Block();
		else
			Lift_Block();

	}

	auto [curState, fRatio] = m_pGameInstance->Get_DimensionInfo();

	m_fAlphaRatio = fRatio;
	if (m_eCurState == DISABLE && (m_fAngle == m_fPreAngle))
	{
		m_bIsConvert = true;
	}
	m_fAlphaRatio = (1 - 0.2f) * fRatio + 0.2f;


	m_pTransformCom->Rotation(_float3{ -1.f, 0.f, 0.f }, D3DXToRadian(fRatio * 30));

	if (curState == EDimensionState::TOPDIM)
	{
		m_fTime = 0;
		if (m_eCurState == DISABLE)
			m_eCurState = ABLE;
		if (m_eCurState != DIE)
		{
			m_pKeyContext_Move->SetActive(true);
			m_pKeyContext_Rot->SetActive(true);
		}
		m_bIsConvert = false;
	}
	else
		m_fTime += fTimeDelta;

	if (curState == EDimensionState::TWODIM || curState == EDimensionState::TOTWODIM)
	{
		m_eCurState = DISABLE;
		m_pKeyContext_Move->SetActive(false);
		m_pKeyContext_Rot->SetActive(false);
	}

	if (m_eCurState == DIE && m_pDieTextureCom->IsEnd())
		m_bIsDead = true;
	m_pDimensionCom->UpdateState();

	SetAngle();

	if (m_eCurState != DISABLE) m_bIsClear = false;
}

void CTopdee::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
	Topdee_MotionChange();

	m_bIsTelePorting = false;
}

HRESULT CTopdee::Render()
{
	if (CClearPortal::m_bIsClear) return S_OK;

	CRenderStateGaurd _rsGaurd(m_pGraphic_Device);
	DWORD dwOldCullMode = 0;

	if (m_eCurState == DISABLE && m_bIsConvert)
	{

		m_pGraphic_Device->GetRenderState(D3DRS_CULLMODE, &dwOldCullMode);
		m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x08);
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(_uint(m_fAlphaRatio * 255), 255, 255, 255));

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);



		if (FAILED(m_pTransformAnimCom->Bind_Resource())) return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_Texture(0)))       return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))       return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))             return E_FAIL;

	}
	
	auto [_eState, _fRatio] = m_pGameInstance->Get_DimensionInfo();

	if (m_eCurState == DISABLE && _fRatio <= 0.8f)
	{
		DWORD oldColorOp{}, oldColorArg1{}, oldColorArg2{};
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg1);
		m_pGraphic_Device->GetTextureStageState(0, D3DTSS_COLORARG2, &oldColorArg2);


		float pulse = (sin(m_fTime * 3.5f) + 1.0f) * 0.5f; // 0~1 ���� �ݺ� (3.0f�� �ӵ�)

		// �� ���� (�Ͼ� �� �Ķ�)
		_float3 white = { 1.0f, 1.0f, 1.0f };
		_float3 Green = { 0.41f, 0.73f, 0.63f };
		_float3 finalColor = {
			white.x * (1.0f - pulse) + Green.x * pulse,
			white.y * (1.0f - pulse) + Green.y * pulse,
			white.z * (1.0f - pulse) + Green.z * pulse
		};

		// D3D�� D3DCOLOR�� ��ȯ
		D3DCOLOR fxColor = D3DCOLOR_COLORVALUE(finalColor.x, finalColor.y, finalColor.z, 1.0f);
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, fxColor);

		// �ؽ�ó �÷��� ���ϱ�
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		if (FAILED(m_pTransformAnimCom->Bind_Resource())) return E_FAIL;

		if (FAILED(m_pTexturCom_OL->Bind_Texture(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	if (m_eCurState == DIE)
	{
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		if (FAILED(m_pTransformDieCom->Bind_Resource())) return E_FAIL;
		if (FAILED(m_pDieTextureCom->Bind_AnimTexture()))     return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))       return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))             return E_FAIL;
	}



	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, dwOldCullMode);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	return S_OK;

}

void CTopdee::Set_IsDead()
{
	if (m_eCurState!= DIE)
		m_pGameInstance->PlaySoundW(L"Char_DieSnd.wav", CHANNELID::SOUND_TOPDEE, PLAYERSOUND);
	m_eCurState = DIE;
}


void CTopdee::OnCollision(FCollisionInfo _fCollisionInfo)
{

	if (m_eCurState != DISABLE)
	{
		if (_fCollisionInfo.strCollisionLayerTag == L"Wall" ||
			_fCollisionInfo.strCollisionLayerTag == L"FIX" ||
			_fCollisionInfo.strCollisionLayerTag == L"HOLE" ||
			//_fCollisionInfo.strCollisionLayerTag == L"KINETIC" ||
			_fCollisionInfo.strCollisionLayerTag == L"SPIKE"
			) {
			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			if (_fCollisionInfo.collisionType == CCollider::CF_Left)	_vPos.x += _fCollisionInfo.vOverlapDepth.x;
			if (_fCollisionInfo.collisionType == CCollider::CF_Right)	_vPos.x -= _fCollisionInfo.vOverlapDepth.x;
			if (_fCollisionInfo.collisionType == CCollider::CF_Top)		_vPos.y -= _fCollisionInfo.vOverlapDepth.y;
			if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	_vPos.y += _fCollisionInfo.vOverlapDepth.y;
			m_pTransformCom->Set_State(STATE::POSITION, _vPos);

			//PrintDebug("Contact Wall");
		}
		if (_fCollisionInfo.strCollisionLayerTag == L"FireBall" || _fCollisionInfo.strCollisionLayerTag == L"Boss"
			||_fCollisionInfo.strCollisionLayerTag == L"GiantHand" || _fCollisionInfo.strCollisionLayerTag == L"Wave")
		{
			Set_IsDead();
		}
		if (_fCollisionInfo.strCollisionLayerTag == L"LHand" || _fCollisionInfo.strCollisionLayerTag == L"RHand") {
			Set_IsDead();
		}
	}
	if (_fCollisionInfo.strCollisionLayerTag == L"CLEAR_PORTAL")
	{
		m_bIsClear = true;
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
				m_pGameInstance->PlaySoundW(L"Char_PushSnd.wav", CHANNELID::SOUND_TOPDEE, PLAYERSOUND);
			}

			else {
				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				if (_fCollisionInfo.collisionType == CCollider::CF_Left)	_vPos.x += _fCollisionInfo.vOverlapDepth.x;
				if (_fCollisionInfo.collisionType == CCollider::CF_Right)	_vPos.x -= _fCollisionInfo.vOverlapDepth.x;
				if (_fCollisionInfo.collisionType == CCollider::CF_Top)		_vPos.y -= _fCollisionInfo.vOverlapDepth.y;
				if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	_vPos.y += _fCollisionInfo.vOverlapDepth.y;
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			}
		}
	}
}

CTopdee* CTopdee::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTopdee* pInstance = new CTopdee(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTopdee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTopdee::Clone(void* pArg)
{
	CTopdee* pInstance = new CTopdee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTopdee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTopdee::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDimensionCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformAnimCom);
	Safe_Release(m_pDieTextureCom);
	Safe_Release(m_pTransformDieCom);
	if (m_pHoldingObject)
		Safe_Release(m_pHoldingObject);
	Safe_Release(m_pTexturCom_OL);
}

void CTopdee::Set_Position(_float3 v_Pos)
{
	m_pTransformCom->Set_State(STATE::POSITION, v_Pos);
	m_pTransformAnimCom->Set_State(STATE::POSITION, v_Pos);
	m_pTransformDieCom->Set_State(STATE::POSITION, v_Pos);
}



void CTopdee::CreateKeyContext()
{
	m_pKeyContext_Move = m_pGameInstance->CreateKeyContext(true);
	m_pKeyContext_Move->AddFunc(VK_RIGHT, [=]() {
		//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		//vPos.x += 0.076f;
		//m_pTransformCom->Set_State(STATE::POSITION, vPos);
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_LEFT, [=]() {
		//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		//vPos.x -= 0.076f;
		//m_pTransformCom->Set_State(STATE::POSITION, vPos);
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_UP, [=]() {
		//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		//vPos.y += 0.076f;
		//m_pTransformCom->Set_State(STATE::POSITION, vPos);
		}, EKeyActionState::Stay);
	m_pKeyContext_Move->AddFunc(VK_DOWN, [=]() {
		//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		//vPos.y -= 0.076f;
		//m_pTransformCom->Set_State(STATE::POSITION, vPos);
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

HRESULT CTopdee::Ready_Components()
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

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_AnimTransform"), reinterpret_cast<CComponent**>(&m_pTransformAnimCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_DieTransform"), reinterpret_cast<CComponent**>(&m_pTransformDieCom), &TransformDesc)))
		return E_FAIL;


	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TopdeeFake"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TopdeeDIE"),
		TEXT("Com_TextureDIE"), reinterpret_cast<CComponent**>(&m_pDieTextureCom))))
		return E_FAIL;

	// Toodee_Outline
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Topdee_OL"),
		TEXT("Com_Texture_OL"), reinterpret_cast<CComponent**>(&m_pTexturCom_OL))))
		return E_FAIL;


	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) { OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pTransformCom;
	ColliderDesc.strCollisionLayerTag = TEXT("Topdee");
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 0.9375 * 0.8f;
	_collisionInfo.fWidth = 0.9375	* 0.8f;
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

void CTopdee::SetAngle()
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

_bool CTopdee::HoldingLightningBlock()
{
	if (_bHoldingLightningBlock) dynamic_cast<CBlock_Lightning*>(m_pHoldingObject)->OnLightning();
	return _bHoldingLightningBlock;
};


void CTopdee::Reset_TopDee(_float3 _vPos)
{
	m_bIsDead = false;
	Set_Position(_vPos);
	m_bIsDead = false;
	m_eCurState = IDLE;
	m_pDieTextureCom->Bind_Texture(0);
	m_pColliderCom->SetActive(true);

	if (m_pHoldingObject) {
		m_pHoldingObject->SetParent(nullptr);
		m_pHoldingObject->SetPosition(m_pairPutIndex.first, m_pairPutIndex.second, 0);
		Safe_Release(m_pHoldingObject);
		m_pHoldingObject = nullptr;
	}
	m_pairLiftIndex = { 0,0 };

} 
 
 

void CTopdee::Topdee_MotionChange() 
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case CTopdee::IDLE:
			break;
		case CTopdee::WALK:
			break;
		case CTopdee::LIFT:
			break;
		case CTopdee::PUT:
			break;
		case CTopdee::DIE:
			m_pGameInstance->CreateKeyContext(false);
			m_pKeyContext_Move->SetActive(false);
			m_pKeyContext_Rot->SetActive(false);
			m_pDieTextureCom->SetActive_AnimTexture(true);
			m_pTransformDieCom->Rotation(_float3{ -1.f, 0.f, 0.f }, D3DXToRadian(45));
			break;
		case CTopdee::DISABLE:
			if (m_bIsLift && m_pHoldingObject)
			{
				Search_Table();
				Put_Block();
			}
			if (m_fAngle && m_fAngle != 360)
				m_fOriginAngle = m_fAngle;
			if (m_fAngle > 180)
				m_fAngle = 360;
			else
				m_fAngle = 0;

			m_pColliderCom->SetActive(false);
			break;
		case CTopdee::ABLE:
			if (m_bIsLift && !m_pHoldingObject)
			{
				Search_Table();
				Lift_Block();
			}
			if (m_fOriginAngle != 360)
				m_fAngle = m_fOriginAngle;
			else m_fAngle = 0;
			m_pColliderCom->SetActive(true);
		case CTopdee::STATE_END:
			break;
		}
	}
	if (m_eCurState != DISABLE && m_eCurState != ABLE)
		m_ePreState = m_eCurState;
	if (m_eCurState != DIE)
	{
		m_pDieTextureCom->SetActive_AnimTexture(false);
	}
}

void CTopdee::Search_Table()
{
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_iPosX = round(_vPos.x);
	m_iPosY = round(_vPos.y);

	int k = 0;

	for (int i = 1; i > -2; --i)
	{
		for (int j = -1; j < 2; ++j)
		{
			auto _pBlock = (*m_pVecBlockPtrs)[_uint(m_iPosX + j)][_uint(m_iPosY + i)];
			if (!i && !j)
				continue;
			m_vecNearBlock.push_back(make_tuple(_pBlock, 0, k));
			++k;
		}
	}
	get<1>(m_vecNearBlock[0]) = 225.f;
	get<1>(m_vecNearBlock[1]) = 180.f;
	get<1>(m_vecNearBlock[2]) = 135.f;
	get<1>(m_vecNearBlock[3]) = 270.f;
	get<1>(m_vecNearBlock[4]) = 90.f;
	get<1>(m_vecNearBlock[5]) = 315.f;
	get<1>(m_vecNearBlock[6]) = 0.f;
	get<1>(m_vecNearBlock[7]) = 45.f;


	for (int i = 0; i < m_vecNearBlock.size(); ++i)
	{
		float fDiff = abs(m_fAngle - get<1>(m_vecNearBlock[i]));
		get<1>(m_vecNearBlock[i]) = fDiff;
	}

	sort(m_vecNearBlock.begin(), m_vecNearBlock.end(),
		[](const auto& a, const auto& b) {
			return get<1>(a) < get<1>(b);
		});
}

void CTopdee::Lift_Block()
{
	_int iAngle = 99;
	for (size_t i = 0; i < m_vecNearBlock.size(); ++i)
	{
		if (get<0>(m_vecNearBlock[i]) == nullptr || get<0>(m_vecNearBlock[i])->GetBlockFlag() != CBlock::BF_LIFT)
			continue;

		if (get<0>(m_vecNearBlock[i]))
		{
			iAngle = get<2>(m_vecNearBlock[i]);
			break;
		}
	}

	m_vecNearBlock.clear();

	switch (iAngle)
	{
	case 6:
		m_pairLiftIndex = make_pair(_uint(m_iPosX), _uint(m_iPosY - 1));
		break;
	case 7:
		m_pairLiftIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY - 1));
		break;
	case 4:
		m_pairLiftIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY));
		break;
	case 2:
		m_pairLiftIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY + 1));
		break;
	case 1:
		m_pairLiftIndex = make_pair(_uint(m_iPosX), _uint(m_iPosY + 1));
		break;
	case 0:
		m_pairLiftIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY + 1));
		break;
	case 3:
		m_pairLiftIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY));
		break;
	case 5:
		m_pairLiftIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY - 1));
		break;
	}
	if (m_pairLiftIndex.first)
	{
		m_pHoldingObject = (*m_pVecBlockPtrs)[m_pairLiftIndex.first][m_pairLiftIndex.second];
		m_pHoldingObject->SetParent(this);
		m_pHoldingObject->SetPosition(0.f, 1.f, -1.f);
		(*m_pVecBlockPtrs)[m_pairLiftIndex.first][m_pairLiftIndex.second] = nullptr;
		m_pGameInstance->PlaySoundW(L"Char_PickUpSnd.wav", CHANNELID::SOUND_TOPDEE, PLAYERSOUND);
	}

}

void CTopdee::Put_Block()
{
	_int iAngle = 99;
	for (size_t i = 0; i < m_vecNearBlock.size(); ++i)
	{
		if (get<0>(m_vecNearBlock[i]) != nullptr)
			continue;

		if (get<0>(m_vecNearBlock[i]) == nullptr)
		{
			iAngle = get<2>(m_vecNearBlock[i]);
			break;
		}
	}

	m_vecNearBlock.clear();

	switch (iAngle)
	{
	case 6:
		m_pairPutIndex = make_pair(_uint(m_iPosX), _uint(m_iPosY - 1));
		break;
	case 7:
		m_pairPutIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY - 1));
		break;
	case 4:
		m_pairPutIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY));
		break;
	case 2:
		m_pairPutIndex = make_pair(_uint(m_iPosX + 1), _uint(m_iPosY + 1));
		break;
	case 1:
		m_pairPutIndex = make_pair(_uint(m_iPosX), _uint(m_iPosY + 1));
		break;
	case 0:
		m_pairPutIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY + 1));
		break;
	case 3:
		m_pairPutIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY));
		break;
	case 5:
		m_pairPutIndex = make_pair(_uint(m_iPosX - 1), _uint(m_iPosY - 1));
		break;
	}

	m_pHoldingObject->SetParent(nullptr);
	m_pHoldingObject->SetPosition(m_pairPutIndex.first, m_pairPutIndex.second, 0);
	(*m_pVecBlockPtrs)[m_pairPutIndex.first][m_pairPutIndex.second] = m_pHoldingObject;
	m_pHoldingObject->Set_Pos(m_pairPutIndex.first, m_pairPutIndex.second);
	m_pHoldingObject = nullptr;
	m_pGameInstance->PlaySoundW(L"Char_DropSnd.wav", CHANNELID::SOUND_TOPDEE, PLAYERSOUND);

	m_pairLiftIndex = {};
	m_pairPutIndex = {};

	if (m_eCurState != DISABLE)
		m_eCurState = IDLE;
}

void CTopdee::CreateFootStepTimer()
{
	m_pFootStepTimer = m_pGameInstance->CreateTimerWithDuration(.4f, true, [=]() {
		m_pGameInstance->PlaySoundW(L"Char_FootstepsSnd.wav", CHANNELID::SOUND_TOODEE, PLAYERSOUND);
		}, false);
}
