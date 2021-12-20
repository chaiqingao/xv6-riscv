#include "types.h"
typedef uint64 pte_t;
typedef uint64 *pagetable_t;

//mem queue
#define MEM_ENTRY_NUM_PER_PAGE  255
#define MEM_ENTRY_PAGE_NUM      20
#define MEM_ENTRY_NUM (MEM_ENTRY_NUM_PER_PAGE * MEM_ENTRY_PAGE_NUM - 1)
//swap table
#define SWAP_ENTRY_NUM_PER_PAGE 511
#define SWAP_ENTRY_PAGE_NUM     40
//swap file
#define SWAP_FILE_PAGE          256
#define SWAP_FILE_SIZE          (SWAP_FILE_PAGE*4096)
#define SWAP_FILE_NUM           10
#define SWAP_ENTRY_USABLE       ((unsigned long int*)0xffffffffffffffffUL)

// 记录在内存中的文件队列（循环队列）
struct mem_entry{// 16 Byte
  pte_t* pte;
  struct mem_entry* next;
};
struct mem_entry_page{// 4088 Byte
  struct mem_entry_page* next;
  struct mem_entry entrys[MEM_ENTRY_NUM_PER_PAGE];
};

struct mem_queue{
  struct mem_entry_page* pages;
  struct mem_entry* head;
  struct mem_entry* tail;
  int num;
};

// 记录在外存中的文件
struct swap_entry{
  pte_t* pte;
};
struct swap_entry_page{// 4096 Byte
  struct swap_entry_page* next;
  struct swap_entry entrys[SWAP_ENTRY_NUM_PER_PAGE];
};

struct swap_table{
  struct swap_entry_page* pages;
};
