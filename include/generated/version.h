#ifndef _KERNEL_VERSION_H_
#define _KERNEL_VERSION_H_

#define ZEPHYR_VERSION_CODE 67683
#define ZEPHYR_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))

#define KERNELVERSION \
0x01086300
#define KERNEL_VERSION_NUMBER     0x010863
#define KERNEL_VERSION_MAJOR      1
#define KERNEL_VERSION_MINOR      8
#define KERNEL_PATCHLEVEL         99
#define KERNEL_VERSION_STRING     "1.8.99"

#endif /* _KERNEL_VERSION_H_ */
