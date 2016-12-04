/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef linkedlist_h
#define linkedlist_h

#include <stdio.h>
#include <stdint.h>

template<class T> class linkedlistitem
{
public:
  linkedlistitem(T i)
  {
    m_item = i;
    m_pNext = NULL;
    m_pPrev = NULL;
  }
  
  ~linkedlistitem()
  {
    m_pNext = NULL;
    m_pPrev = NULL;
  }

  T m_item;
  linkedlistitem* m_pNext;
  linkedlistitem* m_pPrev;
};

template<class T> class linkedlist
{
  public:
    linkedlist()
    {
      m_u16Count = 0;
      m_pHead = NULL;
      m_pTail = NULL;
      m_bLastWasNext = true;
    }

    ~linkedlist()
    {
        while(count())
            removeHead();
    }

  linkedlistitem<T>* find(T item)
  {
    linkedlistitem<T>* pCur = m_pHead;
    while(pCur)
    {
      if (pCur->m_item == item)
        return pCur;

      pCur = pCur->m_pNext;
    }

    return NULL;
  }
  
  linkedlistitem<T>* findpointers(T item) // TODO revisit this!!
  {
    linkedlistitem<T>* pCur = m_pHead;
    while(pCur)
    {
      if ((*pCur->m_item) == (*item))
        return pCur;

      pCur = pCur->m_pNext;
    }

    return NULL;
  }

  void addHead(T item)
  {
    linkedlistitem<T>* pCur = find(item);

    if(pCur)
      return;

    pCur = new linkedlistitem<T>(item);

    if (!m_pHead)
    {
      m_pHead = m_pTail = pCur;
    }
    else
    {
      m_pHead->m_pPrev  = pCur;
      pCur->m_pNext     = m_pHead;
      
      m_pHead           = pCur;
    }

    m_u16Count++;
  }

  void addTail(T item)
  {
    linkedlistitem<T>* pCur = find(item);

    if(pCur)
      return;

    pCur = new linkedlistitem<T>(item);

    if (!m_pTail)
    {
      m_pHead = m_pTail = pCur;
    }
    else
    {
      m_pTail->m_pNext  = pCur;
      pCur->m_pPrev     = m_pTail;
      
      m_pTail           = pCur;
    }

    m_u16Count++;
  }

  T removeHead()
  {
    m_bLastWasNext = true;
    if (!m_pHead)
      return NULL;

    T ret = m_pHead->m_item;
    remove(m_pHead);
    
    return ret;
  }

  T removeTail()
  {
    m_bLastWasNext = false;
    if (!m_pHead)
      return NULL;

    T ret = m_pTail->m_item;
    remove(m_pTail);
    
    return ret;
  }

  void removeCurIterator()
  {
    if (!m_pIterator)
      return;

    remove(m_pIterator);
  }
  void remove(linkedlistitem<T>* pCur)
  {
    if (!pCur)
        return;

    if (m_pIterator == pCur)
      m_pIterator = m_bLastWasNext ? pCur->m_pNext : pCur->m_pPrev;

    m_u16Count--;

    if (pCur == m_pHead && pCur == m_pTail)
    {
        m_u16Count = 0;
        m_pHead = m_pTail = m_pIterator = NULL;
        delete pCur;
    }
    else if (pCur == m_pHead)
    {
      m_pHead = m_pHead->m_pNext;
      m_pHead->m_pPrev = NULL;
      delete pCur; 
    }
    else if (pCur == m_pTail)
    {
      m_pTail = m_pTail->m_pPrev;
      m_pTail->m_pNext = NULL;
      delete pCur; 
    }
    else
    {
        pCur->m_pPrev->m_pNext = pCur->m_pNext;
        pCur->m_pNext->m_pPrev = pCur->m_pPrev;
        delete pCur;
    }

  }

  bool first()
  {
    if (!m_pHead)
    {
      m_pIterator = NULL;
      return false;
    }

    m_pIterator = m_pHead;
    return true;
  }

  bool last()
  {
    if (!m_pTail)
    {
      m_pIterator = NULL;
      return false;
    }
    
    m_pIterator = m_pTail;
    return true;
  }

  linkedlistitem<T>* next()
  {
   
    m_bLastWasNext = true;

    if (!m_pIterator)
        return 0;

    //T ret = m_pIterator->m_item;
    linkedlistitem<T>* ret = m_pIterator;
    m_pIterator = m_pIterator->m_pNext;

    return ret;
  }

  linkedlistitem<T>* prev()
  {
    m_bLastWasNext = false;

    if (!m_pIterator)
        return 0;

    //T ret = m_pIterator->m_item;
    linkedlistitem<T>* ret = m_pIterator;
    m_pIterator = m_pIterator->m_pPrev;

    return ret;
  }

  void insert(linkedlistitem<T>* pInsert, T item)
      {  insertAfter(pInsert, item); }
      
  void insertAfter(linkedlistitem<T>* pInsert, T item)
  {
    if (!pInsert)
    {
        addHead(item);
        return;
    }
        
    linkedlistitem<T>* pNew = new linkedlistitem<T>(item);
  
    pNew->m_pPrev = pInsert;
    pNew->m_pNext = pInsert->m_pNext;
    
    pInsert->m_pNext = pNew;
    
    if (pNew->m_pNext)
        pNew->m_pNext->m_pPrev = pNew;
        
    m_u16Count++;
 }

  void insertBefore(linkedlistitem<T>* pInsert, T item)
  {
    if (!pInsert)
    {
      addHead(item);
      return;
    }
        
    linkedlistitem<T>* pNew = new linkedlistitem<T>(item);
  
    pNew->m_pNext = pInsert;
    pNew->m_pPrev = pInsert->m_pPrev;
    
    pInsert->m_pPrev = pNew;
    
    pNew->m_pPrev->m_pNext = pNew;
    m_u16Count++;
  }
      
  bool end()
  {
    return m_pIterator == NULL;
  }


  uint16_t count()
  {
    return m_u16Count;
  }

  linkedlistitem<T>* getHead() //take note
    { return m_pHead; }

  private:
    uint16_t m_u16Count;
    linkedlistitem<T>* m_pHead;
    linkedlistitem<T>* m_pTail;
    linkedlistitem<T>* m_pIterator;
    bool m_bLastWasNext;
};
#if 0
typedef struct _tagbob
{
int a;
int b;
bool operator==(const _tagbob rhs) { return a == rhs.a && b == rhs.b;}
} bob;

int main(void)
{
    
  linkedlist<bob *> lsbob; 
  bob *pbob = new bob;
  pbob->a=1; pbob->b=2;
  lsbob.addHead(pbob);

  pbob = new bob;
  pbob->a=3; pbob->b=4;
  lsbob.addHead(pbob);

  pbob = new bob;
  pbob->a=5; pbob->b=6;
  lsbob.addHead(pbob);

  linkedlist<int> ls; 
  ls.addHead(1);
  ls.addHead(2);
  ls.addHead(3);
  ls.addHead(4);
  ls.addHead(5);
  ls.addHead(6);

  linkedlistitem<int> *pintItem;
  ls.last();
  while(!ls.end())
  {
    pintItem = ls.prev();
    int test = pintItem->m_item;
    printf("cur %d\n", test);
  }
  printf("\n");
  linkedlistitem<int>*pintfind =  ls.find(3);
  if (pintfind)
  {
    printf("Found %d\n", pintfind->m_item);
  }
  else
   printf("Error could not find\n");


  printf("\n");
  while(ls.count())
  {
    int test = ls.removeHead();
    printf("cur %d  count %d\n", test, ls.count());

  }
  printf("\n");

  bob bobfind;
  bobfind.a = 3;
  bobfind.b = 4;
  if (lsbob.findpointers(&bobfind ))
    printf("found %d, %d\n", bobfind.a, bobfind.b);
  else
    printf("NOT found %d, %d\n", bobfind.a, bobfind.b);

  bobfind.a = 3;
  bobfind.a = 5;
  if (lsbob.findpointers(&bobfind ))
    printf("found %d, %d\n", bobfind.a, bobfind.b);
  else
    printf("NOT found %d, %d\n", bobfind.a, bobfind.b);

  while(lsbob.count())
  {
    bob *pcurbob = lsbob.removeHead();
    printf("cur a=%d b=%d  count %d\n", pcurbob->a, pcurbob->b, ls.count());
  }
  printf("\n");

  return 0;
}
#endif

#endif // #ifndef linkedlist_h


