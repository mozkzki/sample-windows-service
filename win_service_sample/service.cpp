/* Copyright 2017 yukkun007 */
#include "service.h"
#include <stdarg.h>

#define SERVICE_NAME (TEXT("Sample_Service"))
#define DEBUG_BUFFER_SIZE (1024)

BOOL g_bRun = TRUE;
BOOL g_bService = TRUE;
SERVICE_STATUS_HANDLE g_hServiceStatus = NULL;


VOID DebugPrint(LPTSTR szFormat, ...) {
    TCHAR buffer[DEBUG_BUFFER_SIZE + 1];
    va_list arguments;

    ZeroMemory(buffer, sizeof(buffer));
    va_start(arguments, szFormat);
    _vsntprintf_s(buffer, _countof(buffer),
        DEBUG_BUFFER_SIZE, szFormat, arguments);
    va_end(arguments);

    OutputDebugString(buffer);
}

BOOL MySetServiceStatus(SERVICE_STATUS& service_status) {  // NOLINT (win api)
    BOOL result = SetServiceStatus(g_hServiceStatus, &service_status);
    if (!SetServiceStatus(g_hServiceStatus, &service_status)) {
        DebugPrint(
            TEXT("SetServiceStatus failed. LAST_ERROR=%u\n"),
            GetLastError());
    }
    return result;
}

DWORD WINAPI MyHandlerEx(
    DWORD dwControl,
    DWORD dwEventType,
    LPVOID lpEventData,
    LPVOID lpContext) {

    SERVICE_STATUS service_status;
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwWin32ExitCode = NO_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 1;
    service_status.dwWaitHint = 3000;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        DebugPrint(TEXT("dwControl: SERVICE_CONTROL_STOP\n"));

        DebugPrint(TEXT("SetServiceStatus: SERVICE_STOP_PENDING\n"));
        service_status.dwCurrentState = SERVICE_STOP_PENDING;
        if (!MySetServiceStatus(service_status)) break;

        // 停止時の処理はここに書く

        g_bService = FALSE;
        Sleep(3 * 1000);

        DebugPrint(TEXT("SetServiceStatus: SERVICE_STOPPED\n"));
        service_status.dwCurrentState = SERVICE_STOPPED;
        service_status.dwCheckPoint = 0;
        service_status.dwWaitHint = 0;
        if (!MySetServiceStatus(service_status)) break;

        break;

    case SERVICE_CONTROL_PAUSE:
        DebugPrint(TEXT("dwControl: SERVICE_CONTROL_PAUSE\n"));

        DebugPrint(TEXT("SetServiceStatus: SERVICE_PAUSE_PENDING\n"));
        service_status.dwCurrentState = SERVICE_PAUSE_PENDING;
        if (!MySetServiceStatus(service_status)) break;

        // 一時停止時の処理はここに書く

        g_bRun = FALSE;

        DebugPrint(TEXT("SetServiceStatus: SERVICE_PAUSE\n"));
        service_status.dwCurrentState = SERVICE_PAUSED;
        service_status.dwCheckPoint = 0;
        service_status.dwWaitHint = 0;
        service_status.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
        if (!MySetServiceStatus(service_status)) break;

        break;

    case SERVICE_CONTROL_CONTINUE:
        DebugPrint(TEXT("dwControl: SERVICE_CONTROL_CONTINUE\n"));

        DebugPrint(TEXT("SetServiceStatus: SERVICE_START_PENDING\n"));
        service_status.dwCurrentState = SERVICE_START_PENDING;
        if (!MySetServiceStatus(service_status)) break;

        // 再開時の処理はここに書く

        g_bRun = TRUE;

        DebugPrint(TEXT("SetServiceStatus: SERVICE_RUNNING\n"));
        service_status.dwCurrentState = SERVICE_RUNNING;
        service_status.dwCheckPoint = 0;
        service_status.dwWaitHint = 0;
        service_status.dwControlsAccepted |= SERVICE_PAUSE_CONTINUE;
        if (!MySetServiceStatus(service_status)) break;

        break;

    default:
        DebugPrint(TEXT("not implemented service control method called\n"));
        DebugPrint(TEXT("dwControl=%u"), dwControl);
        return ERROR_CALL_NOT_IMPLEMENTED;
    }

    return NO_ERROR;
}

VOID WINAPI MyServiceMain(DWORD argc, LPTSTR* argv) {
    DebugPrint(TEXT("[start] MyServiceMain\n"));

    SERVICE_STATUS service_status;
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwWin32ExitCode = NO_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 1;
    service_status.dwWaitHint = 1000;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    g_hServiceStatus =
        RegisterServiceCtrlHandlerEx(SERVICE_NAME, MyHandlerEx, NULL);
    if (!g_hServiceStatus) {
        DebugPrint(
            TEXT("RegisterServiceCtrlHandler failed. LAST_ERROR=%u\n"),
            GetLastError());
        return;
    }

    DebugPrint(TEXT("SetServiceStatus: SERVICE_START_PENDING\n"));
    service_status.dwCurrentState = SERVICE_START_PENDING;
    if (!MySetServiceStatus(service_status)) return;

    DebugPrint(TEXT("SetServiceStatus: SERVICE_RUNNING\n"));
    service_status.dwCurrentState = SERVICE_RUNNING;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 0;
    service_status.dwControlsAccepted =
        SERVICE_ACCEPT_PAUSE_CONTINUE |
        SERVICE_ACCEPT_STOP;
    if (!MySetServiceStatus(service_status)) return;

    while (g_bService) {
        if (g_bRun) {
            DebugPrint(TEXT("%s is running.\n"), SERVICE_NAME);
        }
        Sleep(2 * 1000);
    }

    DebugPrint(TEXT("[end] MyServiceMain\n"));
}

int main() {
    SERVICE_TABLE_ENTRY service_table_entry[] = {
        { SERVICE_NAME, MyServiceMain },
        { NULL, NULL }
    };
    if (!StartServiceCtrlDispatcher(service_table_entry)) {
        DebugPrint(
            TEXT("StartServiceStrlDispatcher failed. LAST_ERROR=%u\n"),
            GetLastError());
        return -1;
    }
    return 0;
}
