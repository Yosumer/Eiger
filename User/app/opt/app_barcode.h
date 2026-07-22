#ifndef __APP_BARCODE_H_
#define __APP_BARCODE_H_

#include "includes.h"

typedef union _barcode_t
{
    uint32_t word[8];
    struct 
    {
        uint8_t     product[4];       
        uint8_t     project[4];     
        uint8_t     module[4];
        uint8_t     pcb_type[4];
				uint8_t 	version[4];
				uint8_t 	number[4];
				uint8_t 	index[4];
				uint8_t 	rev1[4];
    }info;
}barcode_t;

extern void barcode_handle(uint8_t _ucCmd);

int example_Barcode_register(void);

#endif
