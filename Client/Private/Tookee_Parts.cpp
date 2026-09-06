#include "Tookee_Parts.h"

CTookee_Parts::CTookee_Parts(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject{ pGraphic_Device }
{
}

CTookee_Parts::CTookee_Parts(const CTookee_Parts& Prototype) :CGameObject(Prototype)
{
}
