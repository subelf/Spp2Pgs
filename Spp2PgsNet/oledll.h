/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from Subtitles or AviSynth scripts
* by Giton Xu <adm@subelf.net>
*
* CoCreateInstance from a dll file. This file is free and open sourced.
*----------------------------------------------------------------------------*/

#pragma once

#include <Windows.h>

DECLSPEC_NOINLINE HRESULT WINAPI CoCreateInstanceFormDllFile(const IID& rclsid, const IID& riid, LPVOID* ppv, LPCWSTR lpszDllFile);