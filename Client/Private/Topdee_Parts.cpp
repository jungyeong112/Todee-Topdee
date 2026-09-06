#include "Topdee_Parts.h"

CTopdee_Parts::CTopdee_Parts(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject{pGraphic_Device}
{
}

CTopdee_Parts::CTopdee_Parts(const CTopdee_Parts& Prototype):CGameObject(Prototype)
{
}
