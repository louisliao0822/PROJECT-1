#include<linux/kernel.h>
#include<linux/linkage.h>

asmlinkage long sys_my_prink(char *buf)
{

printf("%s",buf);
return 0;

}