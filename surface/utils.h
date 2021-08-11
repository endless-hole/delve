#pragma once
#include <Windows.h>

void* get_dll_by_file(LPCWSTR file_path)
{
	HANDLE h_dll = CreateFileW(file_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_dll == INVALID_HANDLE_VALUE) 
		return NULL; 

	DWORD dll_file_sz = GetFileSize(h_dll, NULL);
	PVOID dll_buffer = VirtualAlloc(NULL, dll_file_sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!ReadFile(h_dll, dll_buffer, dll_file_sz, NULL, FALSE) || *(PDWORD)dll_buffer != 9460301)
	{
		VirtualFree(dll_buffer, 0, MEM_RELEASE);
		CloseHandle(h_dll);
		return nullptr;
	}

	CloseHandle(h_dll);
	return dll_buffer;
}

DWORD get_process_id_and_thread_id_by_window_class(LPCSTR window_class_name, PDWORD p_thread_id)
{
	DWORD process_id = 0;
	while (!process_id)
	{
		*p_thread_id = GetWindowThreadProcessId(FindWindowA(window_class_name, NULL), &process_id); Sleep(20);
	} return process_id;
}