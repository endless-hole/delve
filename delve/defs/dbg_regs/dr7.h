#pragma once

union __dr7_t
{
	unsigned __int64 flags;
	struct
	{
		unsigned __int64 L0 : 1;
		unsigned __int64 G0 : 1;
		unsigned __int64 L1 : 1;
		unsigned __int64 G1 : 1;
		unsigned __int64 L2 : 1;
		unsigned __int64 G2 : 1;
		unsigned __int64 L3 : 1;
		unsigned __int64 G3 : 1;
		unsigned __int64 LE : 1;
		unsigned __int64 GE : 1;
		unsigned __int64 always_1 : 1;
		unsigned __int64 RTM : 1;
		unsigned __int64 always_0 : 1;
		unsigned __int64 GD : 1;
		unsigned __int64 reserved_0 : 2;
		unsigned __int64 RW0 : 2;
		unsigned __int64 LEN0 : 2;
		unsigned __int64 RW1 : 2;
		unsigned __int64 LEN1 : 2;
		unsigned __int64 RW2 : 2;
		unsigned __int64 LEN2 : 2;
		unsigned __int64 RW3 : 2;
		unsigned __int64 LEN3 : 2;
		unsigned __int64 reserved_1 : 32;
	} bits;
};

//
// The debug control register (DR7) enables or disables breakpoints and sets breakpoint conditions.
//
// L0-L3 (local breakpoint enable)
//		Enable the breakpoint condition for the associated breakpoint for the current task. When a
//		breakpoint condition is detected and its associated Ln flag is set, a debug exception is generated.
//		The processor automatically clears these flags on every task switch to avoid unwanted breakpoint
//		conditions in the new task.
//
// G0-G3 (global breakpoint enable)
//		Enable the breakpoint condition for the associated breakpoint for all tasks. When a breakpoint is detected
//		and its associated Gn flag is set, a debug exception is generated. The processor does not clear these flags
//		on a task switch, allowing the breakpoint to be enabled for all tasks.
//
// LE and GE (local and global exact breakpoint enable)
//		Not supported in IA-32, or Intel 64 processors. When set, these flags cause the processor to detect the
//		exact instruction that caused a data breakpoint condition. Recommended to set LE and GE to 1 if exact
//		breakpoints are required.
//
// RTM (restricted transactional memory)
//		Enables advanced debugging of RTM regions. This is only enabled when IA32_DEBUGCTRL.RTM is set.
//
// GD (general detect enable)
//		Enables debug-register protection, which causes a debug exception to be generated prior to any _mov_ instruction
//		that accesses a debug register. When the condition is detected, the BD flag in DR6 is set prior to generating an
//		exception. This condition is provided to support in-circuit emulators.
//
//		The processor clears the GD flag upon entering the debug exception handler, to allow the handler to access the debug
//		registers.
//
// R/W0-R/W3 (read/write)
//		Specifies the breakpoint condition for the corresponding breakpoint. The DE flag in CR4 determines how the bits in the R/Wn
//		fields are interpreted. When the DE flag is set, the processor interprets bits as follows:
//		
//		00 - Break on instruction execution only
//		01 - Break on data writes only
//		10 - Break on I/O read/write
//		11 - Break on data read/write but not instruction fetches
//
//		When the DE flag is clear, the processor interprets the R/Wn bits as follows:
//
//		00 - Break on instruction execution only
//		01 - Break on data writes only
//		10 - Undefined
//		11 - Break on data read/write but not instruction fetches
//
// LEN0-LEN3 (length)
//		These fields specify the size of the memory location at the address specified in the corresponding debug register (DR0-DR3).
//		The fields are interpreted as follows:
//
//		00 - 1-byte length
//		01 - 2-byte length
//		10 - Undefined (or 8-byte length)
//		11 - 4-byte length
//
//		If the corresponding RWn fields in DR7 is 00 (instruction execution), then the LENn field should also be 00. Using other lengths
//		results in undefined behavior.
//
//		_1 = required set to 1
//		_0 = required set to 0
//
//		Reserved_1 is the upper 32 bits of the register and required to be set to 0 otherwise a #GP may be generated.
//
// Miscellaneous:
//		Instruction-breakpoint and general detect condition result in faults; where as other debug-exception conditions result 
//		in traps.
//