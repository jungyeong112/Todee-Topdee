#include "Block_Box.h"
#include "Topdee.h"
#include "Transform.h"

CBlock_Box::CBlock_Box(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlock( pGraphic_Device )
{
}

CBlock_Box::CBlock_Box(const CBlock_Box& Prototype)
	: CBlock(Prototype)
{
	m_iBlockFlag = EBLOCKFLAG::BF_LIFT;
}

HRESULT CBlock_Box::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBlock_Box::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	return S_OK;
}

void CBlock_Box::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBlock_Box::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CBlock_Box::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CBlock_Box::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CBlock_Box::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Box"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CBlock_Box* CBlock_Box::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlock_Box* pInstance = new CBlock_Box(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlock_Box");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlock_Box::Free()
{
	__super::Free();
}

Engine::CGameObject* CBlock_Box::Clone(void* pArg)
{
	CBlock_Box* pInstance = new CBlock_Box(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlock_Box");
		Safe_Release(pInstance);
	}

	return pInstance;


}
