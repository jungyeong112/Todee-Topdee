#include "Yumbrozi_Parts.h"
#include "Yumbrozi.h"
#include "Transform.h"

CYumbrozi_Parts::CYumbrozi_Parts(LPDIRECT3DDEVICE9 pGraphic_Device):CBoss_Part(pGraphic_Device)
{
}

CYumbrozi_Parts::CYumbrozi_Parts(const CYumbrozi_Parts& Prototype) : CBoss_Part(Prototype)
{
}

HRESULT CYumbrozi_Parts::SetParent(CBoss* ppGameObject)
{
    if (FAILED(__super::SetParent(ppGameObject)))return E_FAIL;
    m_pBaseTransform = static_cast<CTransform*>(ppGameObject->Get_Component(L"Com_Transform"));
    return S_OK;
}

HRESULT CYumbrozi_Parts::SetParentTransform(CTransform* pTransformCom)
{
    if (!pTransformCom) return E_FAIL;
    m_pParentTransform = pTransformCom;

    return S_OK;
}

HRESULT CYumbrozi_Parts::SetHandTransform(CTransform* pTransformCom)
{
    if (!pTransformCom) return E_FAIL;
    m_pHandTransform = pTransformCom;
    return E_NOTIMPL;
}

void CYumbrozi_Parts::Flip(_bool _bFlip)
{
    if (_bFlip) {
        if (!m_bFlip)  m_pTransformCom->Final_Scale(-1.f, 1.f, 1.f);
        m_bFlip = true;
    }
    else {
        if (m_bFlip)  m_pTransformCom->Final_Scale(-1.f, 1.f, 1.f);
        m_bFlip = false;
    }
}

void CYumbrozi_Parts::InheritPositon_Begin()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_vOffset = m_pParentTransform->Get_WorldState(STATE::POSITION);
    m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + m_vOffset);
}

void CYumbrozi_Parts::InheritPositon_End()
{
    m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - m_vOffset);
    m_vOffset = _float3{ 0.f,0.f,0.f };

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CYumbrozi_Parts::InheritPositon_Begin_Limb()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    auto HandPos = m_pHandTransform->Get_WorldState(STATE::POSITION);
    auto ShoulderPos = m_pParentTransform->Get_WorldState(STATE::POSITION);
    m_vOffset = (HandPos + ShoulderPos) * 0.5f;
    m_vOffset.z = -0.2f;
    m_pTransformCom->Set_State(STATE::POSITION, m_vOffset);

    auto _vDist = HandPos - ShoulderPos;
    _vDist.z = 0;
    _float Length = D3DXVec3Length(&_vDist);
    m_pTransformCom->Scale(Length, m_vOriginScale.y, 0);

    _float _fAngle = atan2(_vDist.y, _vDist.x);
    m_pTransformCom->Rotation(_float3{ 0.f,0.f,1.f }, _fAngle);
}

void CYumbrozi_Parts::InheritPositon_End_Limb()
{
 /*   m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - m_vOffset);
    m_vOffset = _float3{ 0.f,0.f,0.f };*/

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

