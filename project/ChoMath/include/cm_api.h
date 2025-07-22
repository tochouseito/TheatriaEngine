#ifndef CM_API_H
#define CM_API_H

// ------------------------
// DLLExportMacro
// ------------------------
#ifdef CHOMATH_EXPORTS
#define CM_API __declspec(dllexport)
//#define REGISTER_SCRIPT_FACTORY(SCRIPTNAME) \
//    extern "C" __declspec(dllexport) Marionnette* Create##SCRIPTNAME##Script(GameObject& object) { \
//        return new SCRIPTNAME(object);}
#else
#define CM_API __declspec(dllimport)
#endif

#endif // CM_API_H
