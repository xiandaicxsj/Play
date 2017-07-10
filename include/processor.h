/* this is for x86 */
#ifndef _H_PROCESSOR
#define _H_PROCESSOR
#define EF_CF (1 << 0)
#define EF_PF (1 << 2)
#define EF_AF (1 << 4)
#define EF_ZF (1 << 5)
#define EF_SF (1 << 7)
#define EF_TF (1 << 8)
#define EF_IF (1 << 9)
#define EF_DF (1 << 10)
#define EF_OF (1 << 11)
#define EF_IOPL (0x3 << 12)
#define EF_NT (1 << 14)
#define EF_RF (1 << 16)
#define EF_VM (1 << 17)
#define EF_AC (1ul << 18)
#endif
