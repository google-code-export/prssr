#ifndef _SYNCLIST_H_
#define _SYNCLIST_H_

template<class TYPE, class ARG_TYPE>
class CSyncList : public CList<TYPE, ARG_TYPE> {
public:
	CSyncList(int nBlockSize = 10);
	virtual ~CSyncList();

	BOOL IsEmpty();
	int GetCount();

	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);

	TYPE RemoveHead();
	TYPE RemoveTail();

	// helper functions (note: O(n) speed)
	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL);
		// defaults to starting at the HEAD, return NULL if not found
	POSITION FindIndex(int nIndex);
		// get the 'nIndex'th element (may return NULL)

protected:
	CRITICAL_SECTION CS;
};

template<class TYPE, class ARG_TYPE>
CSyncList<TYPE, ARG_TYPE>::CSyncList(int nBlockSize/* = 10*/) : CList<TYPE, ARG_TYPE>(nBlockSize) {
	InitializeCriticalSection(&CS);
}

template<class TYPE, class ARG_TYPE>
CSyncList<TYPE, ARG_TYPE>::~CSyncList() {
	DeleteCriticalSection(&CS);
}

template<class TYPE, class ARG_TYPE>
inline BOOL CSyncList<TYPE, ARG_TYPE>::IsEmpty() {
	EnterCriticalSection(&CS);
	BOOL b = CList<TYPE, ARG_TYPE>::IsEmpty();
	LeaveCriticalSection(&CS);
	return b;
}

template<class TYPE, class ARG_TYPE>
inline int CSyncList<TYPE, ARG_TYPE>::GetCount() {
	EnterCriticalSection(&CS);
	int cnt = CList<TYPE, ARG_TYPE>::GetCount();
	LeaveCriticalSection(&CS);
	return cnt;
}

template<class TYPE, class ARG_TYPE>
POSITION CSyncList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement) {
	EnterCriticalSection(&CS);
	POSITION pos = CList<TYPE, ARG_TYPE>::AddHead(newElement);
	LeaveCriticalSection(&CS);
	return pos;
}

template<class TYPE, class ARG_TYPE>
POSITION CSyncList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement) {
	EnterCriticalSection(&CS);
	POSITION pos = CList<TYPE, ARG_TYPE>::AddTail(newElement);
	LeaveCriticalSection(&CS);
	return pos;
}

template<class TYPE, class ARG_TYPE>
TYPE CSyncList<TYPE, ARG_TYPE>::RemoveHead() {
	EnterCriticalSection(&CS);
	TYPE e = CList<TYPE, ARG_TYPE>::RemoveHead();
	LeaveCriticalSection(&CS);
	return e;
}

template<class TYPE, class ARG_TYPE>
TYPE CSyncList<TYPE, ARG_TYPE>::RemoveTail() {
	EnterCriticalSection(&CS);
	TYPE e = CList<TYPE, ARG_TYPE>::RemoveTail();
	LeaveCriticalSection(&CS);
	return e;
}

template<class TYPE, class ARG_TYPE>
POSITION CSyncList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter/* = NULL*/) {
	EnterCriticalSection(&CS);
	POSITION pos = CList<TYPE, ARG_TYPE>::Find(searchValue, startAfter);
	LeaveCriticalSection(&CS);
	return pos;
}

template<class TYPE, class ARG_TYPE>
POSITION CSyncList<TYPE, ARG_TYPE>::FindIndex(int nIndex) {
	EnterCriticalSection(&CS);
	POSITION pos = CList<TYPE, ARG_TYPE>::FindIndex(nIndex);
	LeaveCriticalSection(&CS);
	return pos;
}


#endif