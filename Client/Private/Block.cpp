#include "Block.h"
#include "GameInstance.h"
#include "Topdee.h"
#include "TooTooKee.h"

CBlock::CBlock(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CBlock::CBlock(const CBlock& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlock::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg) {
		PairInfo pairInfo = *reinterpret_cast<PairInfo*>(pArg);
		_float3 _vPos{};
		_uint _iDir{}; // 0 - Up, 1 - Down, 2 - Left, 3 - Right

		visit([&_vPos, &_iDir, pairInfo, this](const auto& type) {
			using ObjectType = decay_t<decltype(type)>;

			if (is_same_v<ObjectType, FObjectType1>) {
				FObjectType1 objType1 = get<FObjectType1>(pairInfo.second);
				_vPos = objType1.m_vPosition;

				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			}
			else if (is_same_v<ObjectType, FObjectType2>) {
				FObjectType2 objType2 = get<FObjectType2>(pairInfo.second);
				_vPos = objType2.m_vPosition;
				_iDir = objType2.m_vDirection;

				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
				//Direction Info Insert
			}
			}, pairInfo.second);
	}

	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pairCurPos.first = round(_vPos.x);
	m_pairCurPos.second = round(_vPos.y);

	m_pairInitPos = m_pairCurPos;

	m_vecRootDir.resize(4);
	return S_OK;
}

void CBlock::Priority_Update(_float fTimeDelta)
{

}

void CBlock::Update(_float fTimeDelta)
{
	if (m_bIsHoleFall) return;
	if (m_pParent)
	{
		auto [curState, fRatio] = m_pGameInstance->Get_DimensionInfo();
		m_pTransformCom->Rotation(_float3{ 1.f, 0.f, 0.f }, D3DXToRadian(fRatio * 33));
	}

}

void CBlock::Late_Update(_float fTimeDelta)
{
	auto [_state, _] = m_pGameInstance->Get_DimensionInfo();
	
	if (_state == EDimensionState::TOPDIM	&& !m_pParent && !m_bIsHoleFall) {
		auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);

		_float fRatio = 0.92f;
		_vPos.x = m_pairCurPos.first * (1 - fRatio) + _vPos.x * fRatio;
		_vPos.y = m_pairCurPos.second * (1 - fRatio) + _vPos.y * fRatio;

		m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	}

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CBlock::Render()
{
	auto _fRatio = 0.5;
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	if (m_bIsHoleFall) {
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(_fRatio, _fRatio, _fRatio, 1.f));
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP,		D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TFACTOR);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_TEXTURE);
	}

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlock::Ready_Components()
{
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Box"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = nullptr;
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = dynamic_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));
	ColliderDesc.strCollisionLayerTag = arrColLayer[m_iBlockFlag];

	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 1;
	_collisionInfo.fWidth = 1;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

PickingInfo* CBlock::Picking(RAY tRay)
{
	_float4x4 matWorldInv{};

	RAY tLocalRay = {
		tRay.vPos,
		tRay.vDir
	};

	D3DXMatrixInverse(&matWorldInv, nullptr, m_pTransformCom->Get_WorldMatrixPtr());

	D3DXVec3TransformCoord(&tLocalRay.vPos, &tLocalRay.vPos, &matWorldInv);
	D3DXVec3TransformNormal(&tLocalRay.vDir, &tLocalRay.vDir, &matWorldInv);
	D3DXVec3Normalize(&tLocalRay.vDir, &tLocalRay.vDir);

	PickingInfo* pPickingInfo = m_pVIBufferCom->Picking(tLocalRay);
	if (pPickingInfo)
	{
		pPickingInfo->m_vCenterPos = m_pTransformCom->Get_State(STATE::POSITION);
		pPickingInfo->m_pGameObject = this;
	}

	return pPickingInfo;
}

HRESULT CBlock::Reset()
{
	if (!m_pVecBlockPtrs) return S_OK;

	m_pairCurPos = m_pairInitPos;
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vPos.x = m_pairCurPos.first;
	_vPos.y = m_pairCurPos.second;
	_vPos.z = 0.f;
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);

	m_bIsHoleFall = false;

	m_pColliderCom->SetActive(true);

	(*m_pVecBlockPtrs)[m_pairInitPos.first][m_pairInitPos.second] = this;
	AddRef();
		
	return S_OK;
}

_bool CBlock::Search_Block(_float fx, _float fy)
{

	if (this->GetBlockFlag() == BF_LIFT)
	{

		m_pBlock			= (*m_pVecBlockPtrs)[_uint(m_pairCurPos.first + fx)][_uint(m_pairCurPos.second + fy)];
		auto _vTookeePos	= CTooTooKee::GetPos();
		auto _bBehindTookee = (_uint(m_pairCurPos.first + fx) == round(_vTookeePos.x)) && (_uint(m_pairCurPos.second + fy) == round(_vTookeePos.y));

		if (_bBehindTookee) return false;

		if (m_pBlock)
		{
			if (m_pBlock->GetBlockFlag() == BF_FIX)
				return false;
		}
		else
			return true;

		if (m_pBlock->GetBlockFlag() == BF_LIFT)
		{
			return m_pBlock->Search_Block(fx, fy);
		}



	}

	return false;
}

void CBlock::Push_Block(_float fx, _float fy)
{
	auto pBlock = (*m_pVecBlockPtrs)[_uint(m_pairCurPos.first + fx)][_uint(m_pairCurPos.second + fy)];

	if (pBlock)
	{
		if (pBlock->GetBlockFlag() == BF_FIX)
			return;
		else
		{
			pBlock->Push_Block(fx, fy);
			MoveUpdate(fx, fy);
		}
	}
	else
		MoveUpdate(fx, fy);
}

void CBlock::Set_Pos(_float fX, _float fY)
{
	m_pairCurPos.first = fX;
	m_pairCurPos.second = fY;
}


void CBlock::MoveUpdate(_float fx, _float fy)
{
	(*m_pVecBlockPtrs)[_uint(m_pairCurPos.first)][_uint(m_pairCurPos.second)] = nullptr;

	/*auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);

	_vPos.x = round(_vPos.x) + (fx);
	_vPos.y = round(_vPos.y) + (fy);

	m_pTransformCom->Set_State(STATE::POSITION, _vPos);*/

	m_pairCurPos.first = _uint(m_pairCurPos.first + fx);
	m_pairCurPos.second = _uint(m_pairCurPos.second + fy);
	(*m_pVecBlockPtrs)[_uint(m_pairCurPos.first)][_uint(m_pairCurPos.second)] = this;
}

_bool CBlock::DiffKenetic()
{
	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float fResX = abs(m_pairCurPos.first - _vPos.x);
	_float fResY = abs(m_pairCurPos.second - _vPos.y);

	if (fResX >= 0.2f || fResY >= 0.2f)
		return false;
	else
		return true;
}

void CBlock::HoleFall()
{
	if ((*m_pVecBlockPtrs)[_uint(m_pairCurPos.first)][_uint(m_pairCurPos.second)] == this) {
		Safe_Release((*m_pVecBlockPtrs)[_uint(m_pairCurPos.first)][_uint(m_pairCurPos.second)]);
		(*m_pVecBlockPtrs)[_uint(m_pairCurPos.first)][_uint(m_pairCurPos.second)] = nullptr;
		m_pColliderCom->SetActive(false);

		_float3 _vPos(m_pairCurPos.first , m_pairCurPos.second, 1.0f );
		m_pTransformCom->Set_State(STATE::POSITION, _vPos);
		m_bIsHoleFall = true;
	}
	
}

void CBlock::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}

void CBlock::SetParent(CTopdee* pParent)
{
	m_pParent = pParent;
	CTransform::TRANSFORM_DESC		TransformDesc{};
	if (m_pParent)
		TransformDesc.parent = static_cast<CTransform*>(m_pParent->Get_Component(TEXT("Com_Transform")));
	else
		TransformDesc.parent = nullptr;
	m_pTransformCom->SetParent(&TransformDesc);
}

void CBlock::SetPosition(_float fx, _float fy, _float fz)
{
	_float3     _vPos = { fx,fy, fz };
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);
	m_pParent == nullptr ? m_pColliderCom->SetActive(true), m_pTransformCom->Rotation(_float3{ 1.f, 0.f, 0.f }, D3DXToRadian(0)) : m_pColliderCom->SetActive(false);
}
