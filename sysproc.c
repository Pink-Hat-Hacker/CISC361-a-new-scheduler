#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#include "spinlock.h"
extern struct {
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

int sys_crsp(void) {
  cprintf("name\tpid\tstate\n");
  cprintf("---------------------------\n");
  struct proc *process;
  acquire(&ptable.lock);
  for(process = ptable.proc; process < &ptable.proc[NPROC]; process++){
    if(process->state == RUNNING){
      cprintf("%s\t%d\tRUNNING\n", process->name, process->pid);
    }
    else if(process->state == SLEEPING){
      cprintf("%s\t%d\tSLEEPING\n", process->name, process->pid);
    }
  }
  release(&ptable.lock);
  return 1;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
