#ifndef REMUX_WIN_H
#define REMUX_WIN_H

#define WIN32_LEAN_AND_MEAN
#define UNICODE

#define  NTDDI_VERSION  NTDDI_VISTA
#define _WIN32_WINNT   _WIN32_WINNT_VISTA
#define  WINVER        _WIN32_WINNT_VISTA
#define _WIN32_IE      _WIN32_IE_IE80

#include <windows.h>
#include <shlobj.h>

#endif
