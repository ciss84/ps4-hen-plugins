#pragma once

#define SCE_KERNEL_O_WRONLY 0x0001
#define SCE_KERNEL_O_CREAT 0x0200
#define SCE_KERNEL_O_RDONLY 0x0000

#define ARRAY_COUNT(arry) sizeof(arry) / sizeof(arry[0])

typedef unsigned char vm_prot_t;	/* protection codes */

#define	VM_PROT_DEFAULT		VM_PROT_ALL

#define	MNT_UPDATE	0x0000000000010000ULL /* not real mount, just update */

void klog(const char* fmt, ...);
void Notify(const char* MessageFMT, ...);

void Get_Page_Table_Stats(int vm, int type, int* Used, int* Free, int* Total);