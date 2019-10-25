#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "svmm.h"

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

void test_svm_ds(){
  int i;
  uint * test1_pas = (uint *)kalloc();
  *test1_pas = 234602496;

  uint * test2_pas = (uint *)kalloc();
  *test2_pas = 234602496;
  *(test2_pas+1) = 234315776;

   uint * test3_pas = (uint *)kalloc();
  *test3_pas = 234602496;
  *(test3_pas+1) = 234315776;
  *(test3_pas+2) = 234614784;

   uint * test4_pas = (uint *)kalloc();
  *test4_pas = 234602496;
  *(test4_pas+1) = 234315776;
  *(test4_pas+2) = 234614784;
  *(test4_pas+3) = 234618880;

   uint * test5_pas = (uint *)kalloc();
  *test5_pas = 234602496;
  *(test5_pas+1) = 234315776;
  *(test5_pas+2) = 234614784;
  *(test5_pas+3) = 234618880;
  *(test5_pas+4) = 234622976;


  // svm_t *test1 = 
  add_shared_vm("test1", 1, test1_pas);

  // svm_t *test2 = 
  add_shared_vm("test2", 2, test2_pas);

  // svm_t *test3 = 
  add_shared_vm("test3", 3, test3_pas);

  // svm_t *test4 = 
  add_shared_vm("test4", 4, test4_pas);

  // svm_t *test5 = 
  add_shared_vm("test5", 5, test5_pas);
  // cprintf("Key: %s\n", test1->key);
  // cprintf("npages: %d\n", test1->npages);
  // cprintf("references: %d\n", test1->references);
  // for(i=0; i<test1->npages; i++){
  //   cprintf("\tindex: %d, value: %d\n\n", i, test1->pas[i]);
  // }
  svm_t *test1_get = get_shared_vm("test1");
  cprintf("Key: %s\n", test1_get->key);
  cprintf("npages: %d\n", test1_get->npages);
  cprintf("references: %d\n", test1_get->references);
  for(i=0; i<test1_get->npages; i++){
    cprintf("\tindex: %d, value: %x\n\n", i, test1_get->pas[i]);
  }

  svm_t *test2_get = get_shared_vm("test2");
  cprintf("Key: %s\n", test2_get->key);
  cprintf("npages: %d\n", test2_get->npages);
  cprintf("references: %d\n", test2_get->references);
  for(i=0; i<test2_get->npages; i++){
    cprintf("\tindex: %d, value: %x\n\n", i, test2_get->pas[i]);
  }

  svm_t *test3_get = get_shared_vm("test3");
  cprintf("Key: %s\n", test3_get->key);
  cprintf("npages: %d\n", test3_get->npages);
  cprintf("references: %d\n", test3_get->references);
  for(i=0; i<test3_get->npages; i++){
    cprintf("\tindex: %d, value: %x\n\n", i, test3_get->pas[i]);
  }

  svm_t *test4_get = get_shared_vm("test4");
  cprintf("Key: %s\n", test4_get->key);
  cprintf("npages: %d\n", test4_get->npages);
  cprintf("references: %d\n", test4_get->references);
  for(i=0; i<test4_get->npages; i++){
    cprintf("\tindex: %d, value: %x\n\n", i, test4_get->pas[i]);
  }

  svm_t *test5_get = get_shared_vm("test5");
  cprintf("Key: %s\n", test5_get->key);
  cprintf("npages: %d\n", test5_get->npages);
  cprintf("references: %d\n", test5_get->references);
  for(i=0; i<test5_get->npages; i++){
    cprintf("\tindex: %d, value: %x\n\n", i, test5_get->pas[i]);
  }

  uint va1 = PGROUNDUP(myproc()->sz);
  cprintf("Adding to test1 : va -> %x\n", va1);
  int proc_add = add_proc((char *)va1, test1_get, myproc()->pid);
  if (!proc_add) {
    cprintf("Adding proc failed!\n");
  }

  int proc_idx = get_proc_idx(test1_get, myproc()->pid);
  cprintf("The proc pid : %d, va: %x\n", test1_get->procs[proc_idx].pid,
          test1_get->procs[proc_idx].va);

  uint * pas = alloc_physical_pages(2);
  cprintf("2 Physical pages allocated\n");
  for(int i=0; i<2; i++){
    cprintf("\tpa: %x\n", pas[i]);
  }

  void * va = map_shared_pages_to_proc(pas, myproc()->pgdir, myproc()->sz, 2);
  cprintf("\n\nSuccessfully mapped the PAs to VAs\n\n");
  unmap_shared_pages_to_proc(va, myproc()->pgdir, 2);
  cprintf("\n\nSuccessfully unmapped the PAs to VAs\n\n");

  dealloc_physical_pages(pas, 2);
  cprintf("2 Physical pages dedallocated\n");

  int proc_rem = remove_proc(test1_get, myproc()->pid);
  if (!proc_rem) {
    cprintf("Removing proc failed!\n");
  }

  int proc_idx_fail = get_proc_idx(test1_get, myproc()->pid);
  if (proc_idx_fail == -1) {
    cprintf("Get failed as expected!\n");
  }
  
  int success1 = remove_shared_vm("test1");
  int success2 = remove_shared_vm("test2");
  int success3 = remove_shared_vm("test3");
  int success4 = remove_shared_vm("test4");
  int success5 = remove_shared_vm("test5");
  if(!success1 || !success2 || !success3 || !success4 || !success5){
    cprintf("%s\n", "Failed to remove shared vm\n");
  }else{
    cprintf("%s\n", "Removed all the shared vms\n");
    kfree((char*)test1_pas);
    kfree((char*)test2_pas);
    kfree((char*)test3_pas);
    kfree((char*)test4_pas);
    kfree((char*)test5_pas);
  }
}

int sys_getsharedpage(void){
  char *key;
  int  npages;
  if(argstr(0, &key) < 0 || argint(1, &npages) < 0)
   return -1;
  struct proc * cur_proc = myproc();
  // cprintf("Key : %s, pages : %d, pid: %d, proc size: %d\n", key, npages, cur_proc->pid, cur_proc->sz);
  int va = (int)get_shared_pages(key, npages, cur_proc->pid, cur_proc->sz, cur_proc->pgdir);
  cur_proc->sz += (npages * PGSIZE);
  switchuvm(cur_proc);
  return va;
}

int sys_freesharedpage(void){
  char *key;
  if(argstr(0, &key) < 0)
   return -1;

  struct proc * cur_proc = myproc();
  // cprintf("Key : %s, pid: %d, proc size: %d\n", key, cur_proc->pid, cur_proc->sz);
  int npages = free_shared_pages(key, cur_proc->pid, cur_proc->pgdir);
  if(npages == -1){
    cprintf("No shared memory associated with this key : %s\n", key);
    return -1;
  }

  if(npages == -2){
    cprintf("Shared memory not associated with this process pid : %d\n", cur_proc->pid);
    return -2;
  }

  cur_proc->sz -= (npages * PGSIZE);
  switchuvm(cur_proc);
  return 0;
}