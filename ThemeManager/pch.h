// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <Windows.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <detours.h>

#pragma comment(lib, R"(..\packages\Detours.4.0.1\lib\native\libs\x64\detours.lib)")

#endif //PCH_H
