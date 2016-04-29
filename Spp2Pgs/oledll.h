#pragma once

#include <Windows.h>

DECLSPEC_NOINLINE HRESULT WINAPI CoCreateInstanceFormDllFile(const IID& rclsid, const IID& riid, LPVOID* ppv, LPCWSTR lpszDllFile);