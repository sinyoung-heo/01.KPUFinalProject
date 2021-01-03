#include "MathMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CMathMgr)

CMathMgr::CMathMgr()
{
}




void CMathMgr::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy MathMgr");
#endif

}
