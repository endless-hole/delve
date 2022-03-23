#pragma once
#include "undoc_structs.h"
#include "../log.h"
#include "../stdint.h"

TABLE_SEARCH_RESULT find_node_or_parent
(
	_RTL_AVL_TREE* table,
	uintptr_t starting_vpn,
	_RTL_BALANCED_NODE** node_or_parent
);

bool find_vad
(
	_EPROCESS* eproc,
	uintptr_t address,
	_MMVAD_SHORT** result
);

bool spoof_vad
(
	_EPROCESS* eproc,
	uintptr_t address,
	size_t size,
	ulong_t protection
);