#ifdef 	_H_DEVICE
#define _H_DEVICE
#include"list.h"
#include"type.h"
/* for this */
typedef u8 dev_t
/* bit 8 */
/* 0-3  minior num*/ 
/* 4-7  major  num*/
#define DEV_TYPE (0xf)
#define DEV_MIN(dev_num) ((dev_num) & 0x0f)
#define DEV_MAJ(dev_num) (((dev_num) & 0xf0) >>  4)
#define DEV_BLK 	(0x1)
#define DEV_CHAR	(0x2)
#define DEV_NUM(maj, min) (((maj) << 4 | (min)))
struct device
{
	dev_t dev_num; 
	struct list_head list;
}
struct *device get_device(dev_t dev_num);
int register_device(dev_t dev_num , struct device *device);
int alloc_minor(u8 major);
void init_device();

#endif
