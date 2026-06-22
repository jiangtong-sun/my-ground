#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <QDateTime>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QString>

#pragma comment(lib, "Dbghelp.lib")

static void miniDumpWriteDump(HANDLE hProcess, DWORD ProcessId, HANDLE hFile,
                              CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                              CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam) {
    typedef HRESULT(WINAPI * MiniDumpWriteDumpPtr)(
        HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
        CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
    HMODULE module = LoadLibraryW(L"Dbghelp.dll");
    if (module) {
        MiniDumpWriteDumpPtr mini_dump_write_dump;
        mini_dump_write_dump =
            reinterpret_cast<MiniDumpWriteDumpPtr>(GetProcAddress(module, "MiniDumpWriteDump"));
        if (mini_dump_write_dump) {
            mini_dump_write_dump(hProcess, ProcessId, hFile, MiniDumpNormal,
                                 ExceptionParam, nullptr, CallbackParam);
        }
    }
}

void WriteDump(EXCEPTION_POINTERS *exp, const std::wstring &path) {
    HANDLE h = ::CreateFileW(path.c_str(), GENERIC_WRITE | GENERIC_READ,
                             FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, nullptr);
    MINIDUMP_EXCEPTION_INFORMATION info;
    info.ThreadId = ::GetCurrentThreadId();
    info.ExceptionPointers = exp;
    info.ClientPointers = TRUE;//FALSE;
    miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), h, &info, nullptr);
    ::CloseHandle(h);
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *exp) {
    const QString dumpFileName =
        QString("%1.dmp").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    const QString dumpDirPath = QCoreApplication::applicationDirPath() + "/dmp";
    const QDir dumpDir(dumpDirPath);
    if (!dumpDir.exists()) {
        dumpDir.mkpath(dumpDirPath);
    }
    QString dumpFilePath = dumpDir.filePath(dumpFileName);
    WriteDump(exp, dumpFilePath.toStdWString());
    return EXCEPTION_EXECUTE_HANDLER;
}
