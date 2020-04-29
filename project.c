#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#define my_printk 333
#define my_time 334
#define false 0
#define true 1
#define FIFO 1
#define SJF 2
#define PSJF 3
#define RR 4
typedef struct PROCESS
{
     char name[50];
     int ready_time;
     int exec_time;
     pid_t p_pid;

}Process;

Process process[30];

int compare(const void *a,const void *b)
{
return (((Process *)a)->ready_time - ((Process*)b)->ready_time);
}




   static int finish;
   static int nunit;
   static int t_last;
   static int run;


int assign_cpu(int pid,int assign_core)
{
    cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(assign_core, &mask);

return 0;

}


void RUN_UNIT()
{
volatile unsigned long i;
for (i = 0; i < 1000000UL; i++);
}



int set_high(int id)
{
struct sched_param parameter;

 parameter.sched_priority = 0;

    return(sched_setscheduler(id,SCHED_OTHER,&parameter));

}

int set_low(int id)
{
     struct sched_param parameter;

parameter.sched_priority = 0;

return(sched_setscheduler(id,SCHED_IDLE, &parameter));

}


int execution(int j)
{
        int pid=fork();
       if(pid==0)
       {
           unsigned long s_sec,s_nsec, e_sec, e_nsec;
char write[200];
syscall(my_time, &s_sec, &s_nsec);
for (int i = 0; i < process[j].exec_time; i++) {
RUN_UNIT();}
   
    syscall(my_time,&e_sec,&e_nsec);
    sprintf(write,"[Project1] %d %lu.%09lu %lu.%09lu\n",getpid(),s_sec,s_nsec,e_sec,e_nsec);
     syscall(my_printk,write);
     exit(0);   
}
        assign_cpu(pid,1);

       return pid;
}

int next_p(int N,int policy)
{
       if(run!=-1&&(policy==SJF||policy==FIFO))
       {
         return run;

       }
       int min=-1;

       if(policy==FIFO)
        {

               for(int i=0;i<N;i++)
               {
                 if(process[i].p_pid==-1||process[i].exec_time==0)
                   continue;
                   else if(min==-1)
                      min=i;
                  else if(process[min].ready_time>process[i].ready_time)
                       min=i;


               }

        }
         else if(policy==SJF||policy==PSJF)
         {

            for(int i=0;i<N;i++)
            {
               if(process[i].p_pid==-1||process[i].exec_time==0)
                continue;
               else if(min==-1)
                min=i;
               else if(process[min].exec_time>process[i].exec_time)
                        min=i;

            }

         }

        else if(policy==RR)
        {
           if(run==-1)
           {
              for(int i=0;i<N;i++)
               {
                 if(process[i].p_pid!=-1&&process[i].exec_time>0)
                  {min=i;
                  break;
               }

           }

           }
            else if((nunit-t_last)%500==0)
           {
              min = (run + 1)%N;
              while(process[min].p_pid == -1||process[min].exec_time==0)
              min= (min + 1) % N;


           }
           else
            min =run;
       }
       return min;

}





int schedule(int N,int policy)
{


      for(int i=0;i<N;i++)
                process[i].p_pid=-1;

      finish=0;
      nunit=0;
      int next=0;
      run=-1;

   
      assign_cpu(getpid(),0);

      set_high(getpid());




      while(1)
      {

       if(run!=-1&&process[run].exec_time==0)
       {

          waitpid(process[run].p_pid,NULL,0);
          printf("%s %d\n",process[run].name,process[run].p_pid);
          run=-1;

          finish++;

          if(finish==N)
          break;

       }






          for(int i=0;i<N;i++)
           {
             if(process[i].ready_time==nunit)
              {process[i].p_pid=execution(i);

              set_low(process[i].p_pid);

              }

           }


        int next = next_p(N,policy);
            if(next!=-1){

              if(run!=next){
              set_low(process[run].p_pid);
                set_high(process[next].p_pid);
                t_last=nunit;
                run= next;
            }


            }

            RUN_UNIT();

            if(run !=-1)
            process[run].exec_time--;
            nunit++;



      }

    return 0;

}

int main()
{

char policy_type[40];
    int N;
    scanf("%s%d",policy_type,&N);
     int policy;
    for(int i=0;i<N;i++)
    {

 scanf("%s%d%d",process[i].name,&process[i].ready_time,&process[i].exec_time);

    }
    if (strcmp(policy_type, "SJF") == 0)
     policy=SJF;
    else if(strcmp(policy_type, "PSJF") == 0)
      policy=PSJF;
    else if(strcmp(policy_type,"FIFO")==0)
     policy=FIFO;
    else if(strcmp(policy_type, "RR") == 0)
    policy=RR;
    qsort(&process,N,sizeof(Process),compare);
    schedule(N,policy);
}
