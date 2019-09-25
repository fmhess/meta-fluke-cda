

/*
 *  altera_inttimer-dev.h -- Altera Interval Timer 
 */

#ifndef inttimer_dev_h
#define inttimer_dev_h


#define INTTIMER_IOC_MAGIC_NUMBER   'x'

#define INTTIMER_READ_REG0    _IOR(INTTIMER_IOC_MAGIC_NUMBER,    0, int)
#define INTTIMER_WRITE_FREQL  _IOW(INTTIMER_IOC_MAGIC_NUMBER,    1, int)
#define INTTIMER_WRITE_FREQH  _IOW(INTTIMER_IOC_MAGIC_NUMBER,    2, int)


#endif /* inttimer_dev_h */
