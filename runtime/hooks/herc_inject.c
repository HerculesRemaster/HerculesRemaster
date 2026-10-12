/* herc_inject.exe <exe> <dll> [seconds]
 * Starts <exe> suspended in its own folder, loads <dll> into it with a remote LoadLibraryA,
 * resumes it, then waits up to [seconds] (0 = until it exits) and closes it (WM_CLOSE, then kill).
 * Prints "pid=<pid> exit=<code>". Must be built x86 so LoadLibraryA matches the game's kernel32.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BOOL CALLBACK close_wnd(HWND w, LPARAM pid) {
    DWORD p;
    GetWindowThreadProcessId(w, &p);
    if (p == (DWORD)pid) PostMessageA(w, WM_CLOSE, 0, 0);
    return TRUE;
}

int main(int argc, char **argv) {
    char exe[MAX_PATH], dll[MAX_PATH], dir[MAX_PATH], *slash;
    STARTUPINFOA si = { sizeof si };
    PROCESS_INFORMATION pi;
    void *mem;
    HANDLE th;
    DWORD code = 0, seconds;
    if (argc < 3) { fprintf(stderr, "usage: herc_inject <exe> <dll> [seconds]\n"); return 2; }
    GetFullPathNameA(argv[1], MAX_PATH, exe, NULL);
    GetFullPathNameA(argv[2], MAX_PATH, dll, NULL);
    seconds = argc > 3 ? (DWORD)atoi(argv[3]) : 0;
    strcpy(dir, exe);
    slash = strrchr(dir, '\\');
    if (slash) *slash = 0;
    {   /* HERC_INJECT_SHOW=minimized: start minimized without activation (background traces) */
        char show[32];
        if (GetEnvironmentVariableA("HERC_INJECT_SHOW", show, sizeof show) && !_stricmp(show, "minimized")) {
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWMINNOACTIVE;
        }
    }
    if (!CreateProcessA(exe, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, dir, &si, &pi)) {
        fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError()); return 1;
    }
    mem = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(pi.hProcess, mem, dll, strlen(dll) + 1, NULL);
    th = CreateRemoteThread(pi.hProcess, NULL, 0,
                            (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
                            mem, 0, NULL);
    if (!th) { fprintf(stderr, "CreateRemoteThread failed: %lu\n", GetLastError()); TerminateProcess(pi.hProcess, 1); return 1; }
    WaitForSingleObject(th, 15000);
    GetExitCodeThread(th, &code);
    if (!code) { fprintf(stderr, "LoadLibrary in target failed\n"); TerminateProcess(pi.hProcess, 1); return 1; }
    CloseHandle(th);
    ResumeThread(pi.hThread);
    if (WaitForSingleObject(pi.hProcess, seconds ? seconds * 1000 : INFINITE) == WAIT_TIMEOUT) {
        EnumWindows(close_wnd, (LPARAM)pi.dwProcessId);
        if (WaitForSingleObject(pi.hProcess, 5000) == WAIT_TIMEOUT) TerminateProcess(pi.hProcess, 0);
    }
    GetExitCodeProcess(pi.hProcess, &code);
    printf("pid=%lu exit=%lu\n", pi.dwProcessId, code);
    return 0;
}
