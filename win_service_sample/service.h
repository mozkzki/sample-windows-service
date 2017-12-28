/* Copyright 2017 yukkun007 */
#pragma once

#include <windows.h>
#include <tchar.h>

BOOL MySetServiceStatus(SERVICE_STATUS& service_status);  // NOLINT (win api)
DWORD WINAPI MyHandlerEx(
    DWORD dwControl,
    DWORD dwEventType,
    PVOID pvEventData,
    PVOID pvContext);
VOID WINAPI MyServiceMain(DWORD argc, PTSTR* argv);
VOID DebugPrint(LPTSTR szFormat, ...);
