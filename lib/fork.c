// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW         0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
  void *addr = (void*)utf->utf_fault_va;
  uint32_t err = utf->utf_err;
  int r;

  // Check that the faulting access was (1) a write, and (2) to a
  // copy-on-write page.  If not, panic.
  // Hint:
  //   Use the read-only page table mappings at uvpt
  //   (see <inc/memlayout.h>).

  // LAB 4: Your code here.
  if (!(uvpt[(uint32_t)addr / PGSIZE] & PTE_COW) || !(err & FEC_WR)) panic("pgfault error");

  // Allocate a new page, map it at a temporary location (PFTEMP),
  // copy the data from the old page to the new page, then move the new
  // page to the old page's address.
  // Hint:
  //   You should make three system calls.

  // LAB 4: Your code here.

  //alloc new page at a temporary location
  if (sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W)) panic("pgfault error");

  //copy the content of the old page to the new page
  addr = ROUNDDOWN(addr, PGSIZE);
  memcpy(PFTEMP, addr, PGSIZE);

  //move the new page to the old page's address
  if (sys_page_map(0, PFTEMP, 0, addr, PTE_P | PTE_U | PTE_W)) panic("pgfault error");

  //unmap the temporary page. we now have a writable page
  if (sys_page_unmap(0, PFTEMP)) panic("pgfault error");

  //panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
  int r;

  // LAB 4: Your code here.
  void* page_addr = (void*) (pn * PGSIZE);

  if ((*(uvpt + pn)) & (PTE_W || PTE_COW)){
    //Make both pages (in the parent and the child envs) copy on write
    if (sys_page_map(0, page_addr, envid, page_addr, PTE_P | PTE_U | PTE_COW)) panic("duppage error");
    if (sys_page_map(0, page_addr, 0, page_addr, PTE_P | PTE_U | PTE_COW)) panic("duppage error");
  } else {
    //If the page is read only, child envs will naturally point to the same page
    if (sys_page_map(0, page_addr, envid, page_addr, PTE_P | PTE_U)) panic("duppage error");
  }

  //panic("duppage not implemented");
  return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
  // LAB 4: Your code here.

  // Set up our page fault handler appropriately
  set_pgfault_handler(pgfault);

  // Create a child and handles errors
  envid_t envID = sys_exofork();
  if (envID < 0) panic("Could not create child");

  // Copy our address space and page fault handler setup to the child
  if (envID == 0){
    //we're in the child env: we need to fix "thisenv"
    envid_t childID = sys_getenvid();
    uint32_t index = ENVX(childID);
    thisenv = envs + index;
  } else {
    //we're in the parent env, we need to copy our address space and page fault handler setup to the child
    unsigned pn;
    for (pn = 0; pn < USTACKTOP / PGSIZE; pn++) {
      //only call "duppage" for accessible pages
      unsigned pgdir_index = PDX(pn * PGSIZE);
      if (!(uvpd[pgdir_index] & PTE_P)) continue;
      if (!(uvpt[pn] & (PTE_P | PTE_U))) continue;

      duppage(envID, pn);
    }

    //allocate page for the exception stack in the child env
    if (sys_page_alloc(envID, (void *)(UXSTACKTOP - PGSIZE), PTE_P | PTE_U | PTE_W)) panic("Fork error");

    //setup page fault handler in the child env
    if (sys_env_set_pgfault_upcall(envID, thisenv->env_pgfault_upcall)) panic("Fork error");

    //mark the child as runnable
    if (sys_env_set_status(envID, ENV_RUNNABLE)) panic("Fork error");
  }

  return envID;
  //panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
  panic("sfork not implemented");
  return -E_INVAL;
}
