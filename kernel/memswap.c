#include "memswap.h"
#include "defs.h"
#include "memlayout.h"
#include "riscv.h"

int alloc_mem_queue(struct mem_queue *m_queue)
{
  // 分配内存
  if ((m_queue->pages = (struct mem_entry_page *)kalloc()) == 0)
    return -1;
  struct mem_entry_page* p = m_queue->pages;
  for(int i=0;i<MEM_ENTRY_PAGE_NUM - 1;i++)
  {
    if((p->next=(struct mem_entry_page *)kalloc()) == 0)
      return-1;
    p = p->next;
  }
  p->next = 0;
  // 连接next指针
  struct mem_entry_page *pp = m_queue->pages;
  for (int i = 0; i < MEM_ENTRY_PAGE_NUM; i++)
  {
    for (int j = 0; j < MEM_ENTRY_NUM_PER_PAGE - 1; j++)
    {
      pp->entrys[j].next = &(pp->entrys[j + 1]);
    }
    if (i < MEM_ENTRY_PAGE_NUM - 1)
      pp->entrys[MEM_ENTRY_NUM_PER_PAGE - 1].next = &(pp->next->entrys[0]);
    else
      pp->entrys[MEM_ENTRY_NUM_PER_PAGE - 1].next = &(m_queue->pages->entrys[0]);
    pp = pp->next;
  }
  m_queue->tail = m_queue->head = &(m_queue->pages->entrys[0]);
  m_queue->num = 0;
  struct mem_entry* ppp = m_queue->pages->entrys;
  for(int i=0;i<MEM_ENTRY_NUM+1;i++)
  {
    ppp = ppp->next;
    if(ppp == m_queue->pages->entrys)
    {
      printf("%d OK!\n",i);
    }
  }
  return 0;
}
void dealloc_mem_queue(struct mem_queue *m_queue)
{
  struct mem_entry_page *p = m_queue->pages;
  while (p != 0)
  {
    struct mem_entry_page *next = p->next;
    kfree((void *)p);
    p = next;
  }
  m_queue->pages = 0;
  m_queue->head = 0;
  m_queue->tail = 0;
  m_queue->num = 0;
}

int alloc_swap_table(struct swap_table *s_table)
{
  // 分配内存
  struct swap_entry_page **p = &(s_table->pages);
  for (int i = 0; i < SWAP_ENTRY_PAGE_NUM; i++)
  {
    if ((*p = (struct swap_entry_page *)kalloc()) == 0)
      return -1;
    p = &((*p)->next);
  }
  (*p) = 0;

  // 初始化 entry
  for (struct swap_entry_page *pp = s_table->pages; pp; pp = pp->next)
  {
    for (int j = 0; j < SWAP_ENTRY_NUM_PER_PAGE; j++)
      pp->entrys[j].pte = SWAP_ENTRY_USABLE;
  }
  return 0;
}

void dealloc_swap_table(struct swap_table *s_table)
{
  struct swap_entry_page *p = s_table->pages;
  while (p != 0)
  {
    struct swap_entry_page *next = p->next;
    kfree((void *)p);
    p = next;
  }
  s_table->pages = 0;
}

struct mem_entry* pop_mem_queue(struct mem_queue *m_queue)
{
  struct mem_entry* res;
  while(1)
  {
    if (m_queue->head == m_queue->tail)
      panic("mem queue is empty!\n");
    res = m_queue->head;
    m_queue->head = m_queue->head->next;
    m_queue->num--;
    //printf("pop pte=%p\n",*(res->pte));
    if(*(res->pte) != 0 && *(res->pte) & PTE_V)
    {
      break;
    }
  }
  //printf("[POP] %p\n",*(res->pte));
  return res;
}
void push_mem_queue(struct mem_queue *m_queue, pte_t *pte)
{
  //printf("[PUSH] %d %p\n",m_queue->num+1, *pte);
  if (m_queue->tail->next == m_queue->head)
    panic("mem queue is full!\n");
  if(*pte == 0)
    panic("push  pte=0\n");
  m_queue->tail->pte = pte;
  m_queue->tail = m_queue->tail->next;
  m_queue->num++;
}

struct swap_entry* get_empty_swap_entry(struct swap_table *s_table, int *pos)
{
  int page_num=0;
  for(struct swap_entry_page* p = s_table->pages;p;p=p->next,page_num++)
  {
    for(int i=0;i<SWAP_ENTRY_NUM_PER_PAGE;i++)
    {
      if(p->entrys[i].pte == SWAP_ENTRY_USABLE || *(p->entrys[i].pte) == 0)
      {
        *pos = page_num*SWAP_ENTRY_NUM_PER_PAGE + i;
        return &(p->entrys[i]);
      }
    }
    if(p->next==0)
    { // 尝试为swap table分配内存
      if ((p->next = (struct swap_entry_page *)kalloc()) == 0)
        panic("No more memory to grow swap table!\n");
      for(int j=0;j<SWAP_ENTRY_NUM_PER_PAGE;j++)
      {
        p->next->entrys[j].pte = SWAP_ENTRY_USABLE;
      }
      p->next->next = 0;
    }
  }
  return 0;
}

struct swap_entry* get_pte_swap_entry(struct swap_table *s_table, pte_t *pte, int *pos)
{
  int page_num=0;
  for(struct swap_entry_page* p = s_table->pages;p;p=p->next,page_num++)
  {
    for(int i=0;i<SWAP_ENTRY_NUM_PER_PAGE;i++)
    {
      if(*(p->entrys[i].pte) == *pte)
      {
        *pos = page_num*SWAP_ENTRY_NUM_PER_PAGE + i;
        return &(p->entrys[i]);
      }
    }
  }
  return 0;
}
