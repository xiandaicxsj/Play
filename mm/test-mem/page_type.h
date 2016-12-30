#ifndef _H_PAGE_TYPE
#define _H_PAGE_TYPE
#define PGD_P (1)
#define PGD_R (0 << 1)
#define PGD_W (1 << 1)
#define PGD_U (1 << 2)
#define PGD_S (0 << 2)
#define PGD_A (1 << 5)
#define PGT_P (1)
#define PGT_R (0 << 1)
#define PGT_W (1 << 1)
#define PGT_U (1 << 2)
#define PGT_S (0 << 2)
#define PGT_A (1 << 5)
#define PGT_D (1 << 6)
#define PAGE_SIZE 4096
#define PAGE_MASK 0x00000fff
#define MASK_PAGE 0xfffff000
#define PGD_SHIFT 22
#define PGT_SHIFT 12
#define PTE_SHIFT 2
#define PDE_SHIFT 2
#define PDE_ADDR_MASK 0x003fffff
#define PTE_ADDR_MASK (~PAGE_MASK)
#define INIT_PGD_TABLE 2
#define PAGE_SHIFT 12
#endif
