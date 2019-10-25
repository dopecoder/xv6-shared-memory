#include "types.h"

typedef struct {
  int pid;
  char *va;
} svm_proc_list_t;

typedef struct svm {
  char *key;
  uint *pas;
  uint npages;
  uint references;
  svm_proc_list_t procs[64];
  struct svm *next;
  struct svm *prev;
} svm_t;

// extern svm_t * head;
// extern svm_t * tail;
// extern uint count;
// extern void * start_addr;

svm_t *add_shared_vm(char *key, uint npages, uint *pas);
svm_t *get_shared_vm(char *key);
int remove_shared_vm(char *key);
int add_proc(void *va, svm_t *svm, int pid);
int get_proc_idx(svm_t *svm, int pid);
int remove_proc(svm_t *svm, int pid);

uint *alloc_physical_pages(uint npages);
void dealloc_physical_pages(uint *pas, uint npages);

void *map_shared_pages_to_proc(uint *pas, pde_t *pgdir, uint sz, uint npages);
void unmap_shared_pages_to_proc(void *va, pde_t *pgdir, uint npages);

void *get_shared_pages(char *key, uint npages, int pid, uint proc_sz,
                       pde_t *pgdir);
int free_shared_pages(char *key, int pid, pde_t *pgdir);