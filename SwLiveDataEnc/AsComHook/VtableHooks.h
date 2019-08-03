#pragma once

static const IID IID_IFileOperation = {0x947aab5f, 0x0a5c, 0x4c13, {0xb4, 0xd6, 0x4b, 0xf7, 0x83, 0x6f, 0xc9, 0xf8}};

static const IID IID_IFileDialog =    {0x42f85136, 0xdb7e, 0x439c, {0x85, 0xf1, 0xe4, 0x07, 0x5d, 0x13, 0x5f, 0xc8}};

HRESULT InstallComInterfaceHooks(IUnknown* originalInterface);
HRESULT InstallComInterfaceHooksExt(IUnknown* originalInterface);
void WriteLog(const char *fmt,...);