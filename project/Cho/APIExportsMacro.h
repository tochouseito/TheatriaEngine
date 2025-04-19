#pragma once
#ifdef CHOENGINE_EXPORTS
#define CHO_API __declspec(dllexport)
#else
#define CHO_API __declspec(dllimport)
#endif