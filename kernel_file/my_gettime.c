# include <linux/ktime.h>
# include <linux/timekeeping.h>

asmlinkage  long sys_my_timestamp(unsigned long *sec,unsigned long *nsec)
{
   timespec t;
   getnstimeofday(&t)
   *sec=t.tv_sec;
   *nsec=t.tv_nsec;
    return 0;

}