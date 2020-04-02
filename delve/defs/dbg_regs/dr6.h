#pragma once

union __dr6_t
{
	unsigned __int64 flags;
	struct
	{
		unsigned __int64 B0 : 1;
		unsigned __int64 B1 : 1;
		unsigned __int64 B2 : 1;
		unsigned __int64 B3 : 1;
		unsigned __int64 always_1 : 8;
		unsigned __int64 always_0 : 1;
		unsigned __int64 BD : 1;
		unsigned __int64 BS : 1;
		unsigned __int64 BT : 1;
		unsigned __int64 RTM : 1;
	};
};

//
// The debug status register (DR6) reports debug conditions that were sampled at the time the last debug exception
// was generated. Updates to this register only occur when an exception is generated. The flags in this register 
// show the following information:
//
// B0 through B3 (breakpoint condition detected) flags (bits 0 through 3)
//		Indicates (when set) that its associated breakpoint condition was met when a debug exception was generated. 
//		These flags are set if the condition described for each breakpoint by the LENn, and R/Wn flags in debug control 
//		register DR7 is true. They may or may not be set if the breakpoint is not enabled by the Ln or the Gn flags in 
//		register DR7. Therefore on a #DB, a debug handler should check only those B0-B3 bits which correspond to an enabled 
//		breakpoint.
//
// BD (debug register access detected) flag (bit 13) 
//		Indicates that the next instruction in the instruction stream accesses one of the debug registers (DR0 through DR7). 
//		This flag is enabled when the GD (general detect) flag in debug control register DR7 is set.
//
// BS (single step) flag (bit 14)
//		Indicates (when set) that the debug exception was triggered by the single-step execution mode 
//		(enabled with the TF flag in the EFLAGS register). The single-step mode is the highest priority
//		debug exception. When the BS flag is set, any of the other debug status bits also may be set.
//
// BT (task switch) flag (bit 15)
//		Indicates (when set) that the debug exception resulted from a task switch where the T flag 
//		(debug trap flag) in the TSS of the target task was set. There is no flag in debug control 
//		register DR7 to enable or disable this exception; the T flag of the TSS is the only enabling flag.
//
// RTM (restricted transactional memory) flag (bit 16) 
//		Indicates (when clear) that a debug exception (#DB) or breakpoint exception (#BP) occurred inside 
//		an RTM region while advanced debugging of RTM transactional regions was enabled. This bit is set 
//		for any other debug exception (including all those that occur when advanced debugging of RTM transactional 
//		regions is not enabled). This bit is always 1 if the processor does not support RTM.
//
//		_1 = required set to 1
//		_0 = required set to 0
//
//		Reserved_1 is the upper 32 bits of the register and required to be set to 0 otherwise a #GP may be generated.
//