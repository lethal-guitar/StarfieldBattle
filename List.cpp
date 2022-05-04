#include "List.h"

void List::Add(void* NewEntry)
{
  Knoten* NewKnoten = new Knoten(NewEntry);

  if (nullptr == Start)
  {
    Start = End = NewKnoten;
  }
  else
  {
    End->SetNext(NewKnoten);
    NewKnoten->SetPrev(End);
    End = NewKnoten;
  }
}

void List::Delete(void)
{
  if (nullptr != End)
  {
    while (End->GetPrev())
    {
      End = End->GetPrev();
      delete End->GetNext();
      End->SetNext(nullptr);
    }
  }
}

BOOL List::NextKnoten(void)
{
  if (nullptr != CurrentKnoten)
  {
    if (nullptr != CurrentKnoten->GetNext())
    {
      CurrentKnoten = CurrentKnoten->GetNext();
      return TRUE;
    }
  }

  return FALSE;
}

BOOL List::PrevKnoten(void)
{
  if (nullptr != CurrentKnoten)
  {
    if (nullptr != CurrentKnoten->GetPrev())
    {
      CurrentKnoten = CurrentKnoten->GetPrev();
      return TRUE;
    }
  }

  return FALSE;
}

void List::DeleteKnoten(void)
{
  if (nullptr != CurrentKnoten)
  {
    if (CurrentKnoten == End)
    {
      CurrentKnoten = End->GetPrev();
      delete End;
      End = CurrentKnoten;

      if (End == nullptr)
      {
        Start = nullptr;
      }
      else
      {
        End->SetNext(nullptr);
      }
    }
    else if (CurrentKnoten == Start)
    {
      CurrentKnoten = Start->GetNext();
      delete Start;
      Start = CurrentKnoten;

      if (Start == nullptr)
      {
        End = nullptr;
      }
      else
      {
        Start->SetPrev(nullptr);
      }
    }
    else
    {
      Knoten* p = CurrentKnoten;

      CurrentKnoten = CurrentKnoten->GetPrev();

      CurrentKnoten->SetNext(p->GetNext());
      p->GetNext()->SetPrev(CurrentKnoten);
      delete p;
    }
  }
}

void List::ReplaceCurrent(void* NewEntry)
{
  if (nullptr != CurrentKnoten)
  {
    if (CurrentKnoten == End)
    {
      CurrentKnoten = End->GetPrev();
      delete End;

      End = new Knoten(NewEntry);
      End->SetNext(nullptr);
      End->SetPrev(CurrentKnoten);
    }
    else if (CurrentKnoten == Start)
    {
      CurrentKnoten = Start->GetNext();
      delete Start;

      Start = new Knoten(NewEntry);
      Start->SetNext(CurrentKnoten);
      Start->SetPrev(nullptr);
    }
    else
    {
      Knoten* pTmp = CurrentKnoten;

      CurrentKnoten = CurrentKnoten->GetPrev();
      Knoten* pNewKn = new Knoten(NewEntry);

      pNewKn->SetPrev(CurrentKnoten);
      pNewKn->SetNext(pTmp->GetNext());

      CurrentKnoten->SetNext(pNewKn);
      pNewKn->GetNext()->SetPrev(pNewKn);

      CurrentKnoten = pNewKn;

      delete pTmp;
    }
  }
}
