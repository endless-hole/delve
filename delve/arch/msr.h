#pragma once

union __ia32_efer_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 syscall_enable : 1;
        unsigned __int64 reserved_0 : 7;
        unsigned __int64 long_mode_enable : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 long_mode_active : 1;
        unsigned __int64 execute_disable : 1;
        unsigned __int64 reserved_2 : 52;
    } bits;
};

union __ia32_feature_control_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 lock : 1;
        unsigned __int64 vmxon_inside_smx : 1;
        unsigned __int64 vmxon_outside_smx : 1;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 senter_local : 6;
        unsigned __int64 senter_global : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 sgx_launch_control_enable : 1;
        unsigned __int64 sgx_global_enable : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 lmce : 1;
        unsigned __int64 system_reserved : 42;
    } bits;
};

union __vmx_basic_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmcs_revision_identifier : 31;
        unsigned __int64 always_0 : 1;
        unsigned __int64 vmxon_region_size : 13;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 vmxon_physical_address_width : 1;
        unsigned __int64 dual_monitor_smi : 1;
        unsigned __int64 memory_type : 4;
        unsigned __int64 io_instruction_reporting : 1;
        unsigned __int64 true_controls : 1;
    } bits;
};

union __vmx_entry_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 load_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 ia32e_mode_guest : 1;
        unsigned __int64 entry_to_smm : 1;
        unsigned __int64 deactivate_dual_monitor_treament : 1;
        unsigned __int64 reserved_3 : 1;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 load_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
    } bits;
};

union __vmx_exit_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 save_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 host_address_space_size : 1;
        unsigned __int64 reserved_2 : 2;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 ack_interrupt_on_exit : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 save_ia32_pat : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 save_ia32_efer : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 save_vmx_preemption_timer_value : 1;
        unsigned __int64 clear_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
    } bits;
};

union __vmx_misc_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmx_preemption_tsc_rate : 5;
        unsigned __int64 store_lma_in_vmentry_control : 1;
        unsigned __int64 activate_state_bitmap : 3;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 pt_in_vmx : 1;
        unsigned __int64 rdmsr_in_smm : 1;
        unsigned __int64 cr3_target_value_count : 9;
        unsigned __int64 max_msr_vmexit : 3;
        unsigned __int64 allow_smi_blocking : 1;
        unsigned __int64 vmwrite_to_any : 1;
        unsigned __int64 interrupt_mod : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 mseg_revision_identifier : 32;
    } bits;
};

union __vmx_pinbased_control_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 external_interrupt_exiting : 1;
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 nmi_exiting : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 virtual_nmis : 1;
        unsigned __int64 vmx_preemption_timer : 1;
        unsigned __int64 process_posted_interrupts : 1;
    } bits;
};

union __vmx_primary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 interrupt_window_exiting : 1;
        unsigned __int64 use_tsc_offsetting : 1;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 hlt_exiting : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 invldpg_exiting : 1;
        unsigned __int64 mwait_exiting : 1;
        unsigned __int64 rdpmc_exiting : 1;
        unsigned __int64 rdtsc_exiting : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 cr3_load_exiting : 1;
        unsigned __int64 cr3_store_exiting : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 cr8_load_exiting : 1;
        unsigned __int64 cr8_store_exiting : 1;
        unsigned __int64 use_tpr_shadow : 1;
        unsigned __int64 nmi_window_exiting : 1;
        unsigned __int64 mov_dr_exiting : 1;
        unsigned __int64 unconditional_io_exiting : 1;
        unsigned __int64 use_io_bitmaps : 1;
        unsigned __int64 reserved_5 : 1;
        unsigned __int64 monitor_trap_flag : 1;
        unsigned __int64 use_msr_bitmaps : 1;
        unsigned __int64 monitor_exiting : 1;
        unsigned __int64 pause_exiting : 1;
        unsigned __int64 active_secondary_controls : 1;
    } bits;
};

union __vmx_secondary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 virtualize_apic_accesses : 1;
        unsigned __int64 enable_ept : 1;
        unsigned __int64 descriptor_table_exiting : 1;
        unsigned __int64 enable_rdtscp : 1;
        unsigned __int64 virtualize_x2apic : 1;
        unsigned __int64 enable_vpid : 1;
        unsigned __int64 wbinvd_exiting : 1;
        unsigned __int64 unrestricted_guest : 1;
        unsigned __int64 apic_register_virtualization : 1;
        unsigned __int64 virtual_interrupt_delivery : 1;
        unsigned __int64 pause_loop_exiting : 1;
        unsigned __int64 rdrand_exiting : 1;
        unsigned __int64 enable_invpcid : 1;
        unsigned __int64 enable_vmfunc : 1;
        unsigned __int64 vmcs_shadowing : 1;
        unsigned __int64 enable_encls_exiting : 1;
        unsigned __int64 rdseed_exiting : 1;
        unsigned __int64 enable_pml : 1;
        unsigned __int64 use_virtualization_exception : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
        unsigned __int64 enable_xsave_xrstor : 1;
        unsigned __int64 reserved_0 : 1;
        unsigned __int64 mode_based_execute_control_ept : 1;
        unsigned __int64 reserved_1 : 2;
        unsigned __int64 use_tsc_scaling : 1;
    } bits;
};