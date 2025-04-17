#include "pch.h"
#include "ChoEngineAPI.h"
#include "Main/ChoEngine.h"

CHO_API Engine* Cho::CreateEngine(RuntimeMode mode)
{
    return new ChoEngine(mode);
}

CHO_API void Cho::DestroyEngine(Engine* engine)
{
	delete engine;
}
