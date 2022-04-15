#include "List.h"

void List::Add(Knoten* NewKnoten)
{
	if (NULL == Start)
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

void List::Add(void* NewEntry)
{
	Knoten* NewKnoten = new Knoten(NewEntry);

	if (NULL == Start)
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
	if (NULL != End)
	{
		while(End->GetPrev())
		{
			End = End->GetPrev();
			delete End->GetNext();
			End->SetNext(NULL);
		}
	}
}

BOOL List::NextKnoten(void)
{
	if (NULL != CurrentKnoten)
	{
		if (NULL != CurrentKnoten->GetNext())
		{
			CurrentKnoten = CurrentKnoten->GetNext();
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL List::PrevKnoten(void)
{
	if (NULL != CurrentKnoten)
	{
		if (NULL != CurrentKnoten->GetPrev())
		{
			CurrentKnoten = CurrentKnoten->GetPrev();
			return TRUE;
		}
	}
	
	return FALSE;
}

void List::DeleteKnoten(void)
{
	if (NULL != CurrentKnoten)
	{
		if (CurrentKnoten == End)
		{
			CurrentKnoten = End->GetPrev();
			delete End;
			End = CurrentKnoten;

			if ( End == NULL)
			{
				Start = NULL;
			}
			else
			{
				End->SetNext(NULL);
			}
		}
		else if (CurrentKnoten == Start)
		{
			CurrentKnoten = Start->GetNext();
			delete Start;
			Start = CurrentKnoten;

			if ( Start == NULL)
			{
				End = NULL;
			}
			else
			{
				Start->SetPrev(NULL);
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
	if (NULL != CurrentKnoten)
	{
		if (CurrentKnoten == End)
		{
			CurrentKnoten = End->GetPrev();
			delete End;

			End = new Knoten(NewEntry);
			End->SetNext(NULL);
			End->SetPrev(CurrentKnoten);
		}
		else if (CurrentKnoten == Start)
		{
			CurrentKnoten = Start->GetNext();
			delete Start;
			
			Start = new Knoten(NewEntry);
			Start->SetNext(CurrentKnoten);
			Start->SetPrev(NULL);
		}
		else
		{
			Knoten* pTmp   = CurrentKnoten;
		
			CurrentKnoten  = CurrentKnoten->GetPrev();
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