

/*
 *  fgpio.h -- Fluke custom GPIO controller. 
 */

#ifndef fgpio_dev_h
#define fgpio_dev_h


#define FGPIO_IOC_MAGIC_NUMBER   'x'

#define FGPIO_INTS_OFF     _IOW(FGPIO_IOC_MAGIC_NUMBER,    0, int)
#define FGPIO_GETINT_MASK  _IOR(FGPIO_IOC_MAGIC_NUMBER,    1, int)
#define FGPIO_SETINT_MASK  _IOW(FGPIO_IOC_MAGIC_NUMBER,    2, int)
#define FGPIO_FLUSH_QUEUE  _IOW(FGPIO_IOC_MAGIC_NUMBER,    3, int)
#define FGPIO_READ_REG0    _IOR(FGPIO_IOC_MAGIC_NUMBER,    4, int)


#endif /* fgpio_dev_h */
