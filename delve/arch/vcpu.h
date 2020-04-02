#pragma once

struct __vmcs_t
{
    union
    {
        unsigned int all;

        struct
        {
            unsigned int revision_identifier : 31;
            unsigned int shadow_vmcs_indicator : 1;
        } bits;
    } header;

    unsigned int abort_indicator;
    char data[ 0x1000 - 2 * sizeof( unsigned ) ];
};

struct __vcpu_t
{
    struct __vmcs_t* vmcs;
    unsigned __int64 vmcs_physical;

    struct __vmcs_t* vmxon;
    unsigned __int64 vmxon_physical;
};