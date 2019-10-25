
#include "svmm.h"
#include "defs.h"
#include "mmu.h"
#include "param.h"
#include "memlayout.h"

#define NULL_PROC_ID -1
#define NULL ((void*)0)

svm_t *head = NULL;
svm_t *tail = NULL;
uint count = 0;
void *start_addr = NULL;
void *end_addr = NULL;

// svm_t * add_shared_vm(char * key, uint npages, uint * pas){
//     svm_t * svm;
//     if(head == NULL){
//         start_addr = kalloc();
//         if(!start_addr){
//             // memory allocation failure
//             // Throw exception or return NULL
//             return NULL;
//         }
//         end_addr = start_addr + 4096;
//     }

//     svm = (svm_t *)(start_addr + (count * sizeof(svm_t)));
//     if(svm > end_addr - sizeof(svm_t)){
//         // memory will go out of page, cancel
//         // Throw exception or return NULL
//         return NULL;
//     }
//     svm->key = key;
//     svm->npages = npages;
//     svm->pas = pas;

//     if(tail!=NULL){
//         svm->prev = tail;
//         tail->next = svm;
//         tail=svm;
//     }

//     if(head==NULL){
//         head = svm;
//         if(tail == NULL){
//             tail = svm;
//         }
//     }

//     count++;
// }

static pte_t *
pgdirwalk(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

static int
pagesmap(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
  cprintf("Mapping VA : %x -> PA : %x\n", a, pa);
  for(;;){
    if((pte = pgdirwalk(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

int keycmp(const char *p, const char *q) {
  while (*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

svm_t *add_shared_vm(char *key, uint npages, uint *pas) {
  svm_t *svm;
  int idx = 0;
  svm = (svm_t *)kalloc();
  if (!svm) {
    // memory allocation failure
    // Throw exception or return NULL
    return NULL;
  }
  svm->key = key;
  svm->npages = npages;
  svm->pas = pas;
  svm->references = 0;
  svm->prev = NULL;
  svm->next = NULL;
  for (; idx < NPROC; idx++) {
    svm->procs[idx].pid = NULL_PROC_ID;
    svm->procs[idx].va = NULL;
  }

  if (tail != NULL) {
    svm->prev = tail;
    tail->next = svm;
    tail = svm;
  }

  if (head == NULL) {
    head = svm;
    if (tail == NULL) {
      tail = svm;
    }
  }

  count++;
  return svm;
}

svm_t *get_shared_vm(char *key) {
  if (head == NULL) {
    return NULL;
  }
  svm_t *cur = head;
  for (; cur != NULL; cur = cur->next) {
    if (keycmp(cur->key, key) == 0) {
      return cur;
    }
  }
  return NULL;
}

int remove_shared_vm(char *key) {
  svm_t *cur = get_shared_vm(key);

  if (cur == NULL) {
    return 0;
  }
  svm_t *prev = cur->prev;
  svm_t *next = cur->next;
  if (head == cur) {
    head = cur->next;
    if (head != NULL) {
      head->prev = NULL;
    }
  } else if (tail == cur) {
    tail = cur->prev;
    if (tail != NULL) {
      tail->next = NULL;
    }
  } else {
    if (prev != NULL) {
      prev->next = next;
    }

    if (next != NULL) {
      next->prev = prev;
    }
  }
  kfree((char *)cur->pas);
  kfree((char *)cur);
  count--;
  return 1;
}

int add_proc(void *va, svm_t *svm, int pid) {
  if (svm == NULL) {
    return 0;
  }

  int idx = get_proc_idx(svm, NULL_PROC_ID);
  if (idx == -1) {
    return 0;
  }
  svm->references += 1;
  svm->procs[idx].pid = pid;
  svm->procs[idx].va = va;
  return 1;
}

int get_proc_idx(svm_t *svm, int pid) {
  if (svm == NULL) {
    return 0;
  }
  int idx;
  for (idx = 0; idx < NPROC; idx++) {
    if (svm->procs[idx].pid == pid) {
      return idx;
    }
  }
  return -1;
}

int remove_proc(svm_t *svm, int pid) {
  if (svm == NULL) {
    return 0;
  }
  int idx = get_proc_idx(svm, pid);
  if (idx == -1) {
    return 0;
  }

  svm->references -= 1;
  svm->procs[idx].pid = NULL_PROC_ID;
  svm->procs[idx].va = NULL;
  return 1;
}

uint *alloc_physical_pages(uint npages) { 
    int pg_idx;
    uint * pa_store = (uint*)kalloc();
    if(!pa_store){
        cprintf("Unable to allocate memory to store physical addresses\n");
        return NULL;
    }
    uint * pa_str_ptr = pa_store;
    for(pg_idx=0; pg_idx<npages; pg_idx++){ 
        char *mem = kalloc();
        if(mem == 0){
            cprintf("alloc_physical_pages out of memory\n");
            dealloc_physical_pages(pa_store, pg_idx);
            return NULL;
        }
        memset(mem, 0, PGSIZE);
        *pa_str_ptr = V2P(mem);
        pa_str_ptr++;
    }
    return pa_store;
}

void dealloc_physical_pages(uint *pas, uint npages) { 
    int pg_idx;
    for(pg_idx=0; pg_idx<npages; pg_idx++){
        kfree(P2V(pas[pg_idx]));
    }
 }

void *map_shared_pages_to_proc(uint *pas, pde_t *pgdir, uint sz, uint npages) {
    uint va = PGROUNDUP(sz);
    uint va_idx = va;
    // uint newsz = sz + (PGSIZE * npages);
    for(int pg_idx=0; pg_idx<npages; pg_idx++){
        cprintf("Setting up PTE with VA : %x and PA : %x\n", va_idx, pas[pg_idx]);
        if(pagesmap(pgdir, (char*)va_idx, PGSIZE, pas[pg_idx], PTE_W|PTE_U) < 0){
          cprintf("mappages out of memory (2)\n");
          dealloc_physical_pages(pas, npages);
          return 0;
        }
        va_idx += PGSIZE;
    }
    return (char*)va;
}
void unmap_shared_pages_to_proc(void *va, pde_t *pgdir, uint npages) {
    pte_t *pte;
    char *va_idx = (char*)va;
    for(int pg_idx=0; pg_idx<npages; pg_idx++){
        cprintf("Unmapping PTE with VA : %x\n", va_idx);
        pte = pgdirwalk(pgdir, va_idx, 0);
        if(!pte){
            panic("unmap");
        }
        *pte = 0;
        va_idx += PGSIZE;
    }
}

void *get_shared_pages(char *key, uint npages, int pid, uint proc_sz,
                       pde_t *pgdir) {
  svm_t *svm;
  uint *pas;
  svm = get_shared_vm(key);
  if (svm != NULL) {
    int proc_idx = get_proc_idx(svm, pid);
    if (proc_idx != -1) {
      return svm->procs[proc_idx].va;
    }
    pas = svm->pas;
    cprintf("Getting new pages\n");
    for(int pg_idx=0;pg_idx<npages;pg_idx++){
        cprintf("\t%x\n", pas[pg_idx]);
    }
  } else {
    pas = alloc_physical_pages(npages);
    cprintf("Allocated new pages\n");
    for(int pg_idx=0;pg_idx<npages;pg_idx++){
        cprintf("\t%x\n", pas[pg_idx]);
    }
  }
  char *va = (char *)map_shared_pages_to_proc(pas, pgdir, proc_sz, npages);
  if (va == NULL) {
    // mapping failed, return null
    return NULL;
  }
  if(svm == NULL){
    svm = add_shared_vm(key, npages, pas);
  }
  add_proc(va, svm, pid);

  cprintf("References : %d\n", svm->references);
  return (void *)va;
}

int free_shared_pages(char *key, int pid, pde_t *pgdir) {
  svm_t *svm = get_shared_vm(key);
  if (svm == NULL) {
    // throw error saying its not allocated
    return -1;
  }

  int svm_proc_idx = get_proc_idx(svm, pid);
  if (svm_proc_idx == -1) {
    // throw error saying the shared memory was never allocated for this process
    return -2;
  }

  uint npages = svm->npages;
  char * va = svm->procs[svm_proc_idx].va;
  remove_proc(svm, pid);
  if (svm->references == 0) {
    dealloc_physical_pages(svm->pas, npages);
    remove_shared_vm(key);
  }
  unmap_shared_pages_to_proc(va, pgdir, npages);
  return npages;
}