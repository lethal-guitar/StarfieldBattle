// --- List.h ---
//
// This file contains declarations for the "List" class used in this project.

#ifndef LIST_H
#define LIST_H

#include <Windows.h>

class Knoten
{
  void* m_Entry;

  Knoten* m_Next;

  Knoten* m_Prev;

public:
  Knoten(void)
  {
    m_Entry = nullptr;
    m_Next = m_Prev = nullptr;
  }
  Knoten(void* Entry)
  {
    m_Entry = Entry;
    m_Next = m_Prev = nullptr;
  }

  ~Knoten()
  {
    if (nullptr != m_Entry)
    {
      delete m_Entry;
    }
  }

  void* GetEntry(void)
  {
    if (nullptr != m_Entry)
    {
      return m_Entry;
    }
    else
    {
      return nullptr;
    }
  }

  void SetNext(Knoten* Next) { m_Next = Next; }
  Knoten* GetNext(void) { return m_Next; }

  void SetPrev(Knoten* Prev) { m_Prev = Prev; }
  Knoten* GetPrev(void) { return m_Prev; }
};

class List
{
  Knoten* End;
  Knoten* Start;
  Knoten* CurrentKnoten;

public:
  List(void) { Start = End = CurrentKnoten = nullptr; }
  ~List() { Delete(); }

  void Add(void* NewEntry);

  void Delete(void);

  BOOL NextKnoten(void);
  BOOL PrevKnoten(void);
  void FirstKnoten(void) { CurrentKnoten = Start; }
  void LastKnoten(void) { CurrentKnoten = End; }

  void ReplaceCurrent(void* NewEntry);

  Knoten* GetCurrent(void) { return CurrentKnoten; }
  void DeleteKnoten(void);
};

#endif
