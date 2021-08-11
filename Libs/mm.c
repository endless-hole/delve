#include "mm.h"
#include "ntdll\ntdll.h"

uint64_t mm_rdtsc(void)
{
	/* read the processor time stamp */
	return __rdtsc();
}

uint32_t mm_getrn(uint32_t* seed)
{
	uint32_t tmp = (*seed ? *seed : (uint32_t)mm_rdtsc());
	uint32_t result;

	void* ret_addr = _ReturnAddress();
	void* stack_addr = _AddressOfReturnAddress();

	uint32_t tid = (uint32_t)NtCurrentTeb()->ClientId.UniqueThread;
	uint32_t pid = (uint32_t)NtCurrentTeb()->ClientId.UniqueProcess;

	/* start generating noise */
	result = (uint32_t)mm_rdtsc() + tmp;

	tmp += _rotl16((uint16_t)tmp, 2) + 712552996;

	result += (uint32_t)stack_addr;
	result ^= (uint32_t)ret_addr;
	result ^= 398436027;
	result += pid;
	result += (tid << 13);

	tmp ^= result;

	result += *(uint32_t *)((uint8_t *)stack_addr + 8);
	result += *(uint32_t *)((uint8_t *)stack_addr + 12);
	result += *(uint32_t *)((uint8_t *)stack_addr + (tid % 50));
	result += *(uint32_t *)((uint8_t *)stack_addr + (pid % 50));

	*seed = tmp;

	return result;
}

void* mm_halloc(size_t s)
{
	return RtlAllocateHeap(RtlProcessHeap(), 0, s);
}

void mm_hfree(void* m)
{
	RtlFreeHeap(RtlProcessHeap(), 0, m);
}

void* mm_hrealloc(void* m, size_t s)
{
	return RtlReAllocateHeap(RtlProcessHeap(), 0, m, s);
}

void
mm_cpy(void* dst, void* src, size_t size)
{
#if _WIN64
	if (!(size & 7))
	{
		/* if size is a multiple of 8 */
		__movsq(dst, src, size >> 3);
	}
	else
#endif
	if (!(size & 3))
	{
		/* if size is a multiple of 4 */
		__movsd((uint32_t *)dst, (uint32_t *)src, size >> 2);
	}
	else
	{
		/* move byte-by-byte */
		__movsb((uint8_t *)dst, (uint8_t *)src, size);
	}
}

void
mm_set(void* dst, uint32_t data, size_t size)
{
	if (!(size & 3))
	{
		/* if size is a multiple of 4 */
		__stosd((uint32_t *)dst, data, size >> 2);
	}
	else
	{
		/* store byte-by-byte */
		__stosb((uint8_t *)dst, (uint8_t)data, size);
	}
}

bool_t mm_validptr(void* mem)
{
	NTSTATUS status;
	MEMORY_BASIC_INFORMATION mbi;

	status = NtQueryVirtualMemory(NtCurrentProcess(), mem,
		MemoryBasicInformation, &mbi, sizeof(mbi), 0);

	if (!NT_SUCCESS(status) ||
		!((mbi.Protect & PAGE_EXECUTE_READWRITE) ||
		(mbi.Protect & PAGE_EXECUTE_READ) ||
		(mbi.Protect & PAGE_READONLY) ||
		(mbi.Protect & PAGE_READWRITE)))
	{
		return false;
	}

	return true;
}

bool_t mm_validcodeptr(void* mem)
{
	NTSTATUS status;
	MEMORY_BASIC_INFORMATION mbi;

	status = NtQueryVirtualMemory(NtCurrentProcess(), mem,
		MemoryBasicInformation, &mbi, sizeof(mbi), 0);

	if (!NT_SUCCESS(status) ||
		!((mbi.Protect & PAGE_EXECUTE_READWRITE) ||
		(mbi.Protect & PAGE_EXECUTE_READ)) )
	{
		return false;
	}

	return true;
}
