#include "misc.h"
#include "mm.h"
#include "str.h"
#include "proc.h"
#include "str.h"
#include "ntdll\ntdll.h"

void fill_with_random(uint8_t* outb, size_t size)
{
	uint32_t rseed = 0;
	uint32_t rnum = mm_getrn(&rseed);

	for (ulong_t i = 0; i < size; i++)
	{
		outb[i] = (uint8_t)mm_getrn(&rnum);
	}
}

void sleep(ulong_t ms)
{
	uint64_t t = -(uint64_t)ms * 10000;
	NtDelayExecution(0, (PLARGE_INTEGER)&t);
}

uintptr_t find_pattern(char* pattern)
{
	uint64_t image;
	uint32_t size;
	image_base_size(&image, &size);
	return find_pattern_ex(image, image + size, pattern);
}

uintptr_t find_pattern_rel32(char* pattern, int32_t offset_offset)
{
	uintptr_t data = find_pattern(pattern);

	if(data == 0)
		return 0;

	return rel32_to_abs64(data, offset_offset);
}

uintptr_t find_pattern_rel32_ex(uintptr_t start, uintptr_t end, char* pattern, int32_t offset_offset)
{
    uintptr_t data = find_pattern_ex(start, end, pattern);

    if (data == 0)
        return 0;

    return rel32_to_abs64(data, offset_offset);
}

uintptr_t find_pattern_ex(uintptr_t start, uintptr_t end, char* pattern)
{
    bool_t start_less = start < end;
    int32_t pattern_len = str_alen(pattern) - 1;
	int32_t pattern_pos = start_less ? 0 : pattern_len;
	uint32_t bytes_found = 0;
	uintptr_t current = start;
    uintptr_t first_match = current;

    while (start_less ? current < end : current >= end)
	{
        //for (uintptr_t i = current; ; start_less ? i++ : i--)
        //{
            if (pattern[pattern_pos] == ' ')
            {
                start_less ? pattern_pos++ : pattern_pos--;
                continue;
            }

            uint8_t current_byte = *(uint8_t *)current;
            uint8_t pattern_byte = (uint8_t)str_atobin(
                &pattern[start_less ? pattern_pos : pattern_pos - 1], 2);

            if (current_byte == pattern_byte || pattern[pattern_pos] == '?')
            {
                bytes_found++;

                if (bytes_found == 1)
                {
                    first_match = current;
                }

                if (start_less)
                {
                    pattern_pos += (pattern[pattern_pos] == '?') ? 1 : 2;
                }
                else
                {
                    pattern_pos -= (pattern[pattern_pos] == '?') ? 1 : 2;
                }

                if (pattern_pos < 0 || !pattern[pattern_pos])
                {
                    if (start_less)
                    {
                        return current - bytes_found + 1;
                    }
                    else
                    {
                        return current;
                    }
                }
            }
            else
            {
                if (bytes_found > 0)
                {
                    current = first_match;
                }

                bytes_found = 0;
                pattern_pos = start_less ? 0 : pattern_len;

                //break;
            }
       // }

		start_less ? current++ : current--;
	}

	return 0;
}

uintptr_t find_pattern_raw(uintptr_t start, uintptr_t end,
	uint8_t* pattern_data, size_t pattern_size)
{
	uint32_t pattern_pos = 0;
	uint32_t bytes_found = 0;
	uintptr_t current = start;

	while (current < end)
	{
		uint8_t current_byte = *(uint8_t *)current;
		uint8_t pattern_byte = pattern_data[pattern_pos];

		if (current_byte == pattern_byte)
		{
			bytes_found++;
			pattern_pos++;

			if (pattern_pos >= pattern_size)
			{
				return current - bytes_found + 1;
			}
		}
		else
		{
			bytes_found = 0;
			pattern_pos = 0;
		}

		current++;
	}

	return 0;
}

uintptr_t find_string_ref(char* str)
{
	uint64_t image;
	uint32_t size;
	image_base_size(&image, &size);
	return find_string_ref_ex(image, image + size, str);
}

uintptr_t rel32_to_abs64(uintptr_t src, int32_t offset_offset)
{
	if(src == 0)
	{
		return 0;
	}

	return src + offset_offset + sizeof(int32_t) + *(int32_t *)(src + offset_offset);
}

bool_t is_rel32_opcode_x64(uint8_t* code, uint8_t* out_rel32_offset)
{
	struct opcode_info
	{
		uint8_t opcode;
		uint8_t offset;
	};

	static struct opcode_info opcodes[] =
	{
		{0x4C, +3},
		{0x48, +3},
		{0xE8, +1},
		{0xE9, +1}
	};

	for(ulong_t i = 0; i < countof(opcodes); i++)
	{
		if(code[0] == opcodes[i].opcode)
		{
			if(out_rel32_offset)
			{
				*out_rel32_offset = opcodes[i].offset;
			}

			return true;
		}
	}

	return false;
}

uintptr_t find_string_ref_ex(uintptr_t start, uintptr_t end, char* str)
{
	uintptr_t string_ptr;

	// find the global string pointer
	size_t string_len = str_alen(str);

	string_ptr = find_pattern_raw(start, end, (uint8_t *)str, string_len);

	if(string_ptr == 0)
	{
		return 0;
	}

	// find refs in the code
	uint32_t pattern_pos = 0;
	uint32_t bytes_found = 0;
	uintptr_t current = start;

	while (current < end)
	{
		uint8_t rel_32_offset;

		if(is_rel32_opcode_x64((uint8_t *)current, &rel_32_offset))
		{
			uint8_t current_byte = *(uint8_t *)current;

			if(rel32_to_abs64(current, rel_32_offset) == string_ptr)
			{
				return current;
			}
		}

		current++;
	}

	return 0;
}