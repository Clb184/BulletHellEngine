#ifndef LINKEDARRAYLIST_INCLUDED
#define LINKEDARRAYLIST_INCLUDED

#include "MemoryPool.h"
#include <stdlib.h>
#include <stdio.h>

template<class T>
struct Node : T {
    size_t id = -1;
    size_t active = false;
    Node* pPrev = nullptr;
    Node* pNext = nullptr;
};

template<class T>
struct NodeEx : Node<T> {
    bool IsHead = false;
};

template<class T>
class Iterator {
public:
    Iterator() {
        m_pCurNode = nullptr;
    }
    
    void Set(Node<T>* pNode){
        m_pCurNode = pNode;
    }

    bool MoveFront(){
        if(m_pCurNode){
            m_pCurNode = m_pCurNode->pNext;
            return true;
        }
        return false;
    }

    bool MoveBack(){
        if(m_pCurNode){
            m_pCurNode = m_pCurNode->pPrev;
            return true;
        }
        return false;
    }

    void FreeData(bool front){
        Node<T>* pThis = m_pCurNode;
        Node<T>* pWork;
        if(front && pThis && pThis->pNext){
            pWork = pThis->pNext;
            pWork->pPrev = pThis->pPrev;
            MoveFront();
            pThis->active = false;
        }
        else if (pThis && pThis->pPrev) {
            pWork = pThis->pPrev;
            pWork->pNext = pThis->pNext;
            MoveBack();
            pThis->active = false;
        }
    }

    Node<T>* GetData(){
        return this->m_pCurNode;
    }

    bool operator== (const Iterator<T>& it){
        return m_pCurNode == it.m_pCurNode;
    }

private:
    Node<T>* m_pCurNode;
};

template<class T>
class CDoubleLinkedArrayList{
private:

public:

public:
    CDoubleLinkedArrayList(size_t num_items);
    CDoubleLinkedArrayList(size_t num_items, CMemoryPool<Node<T>>* mem_pool);
    ~CDoubleLinkedArrayList();

    Iterator<T>* GetIterator();
    Iterator<T>* GetBackIterator();
    Iterator<T>* GetFrontIterator();
    Node<T>* AddFront(T item = T());
    Node<T>* AddBack(T item = T());
    bool DeleteFront();
    bool DeleteBack();
    bool DeleteFromIndex(size_t idx);

    size_t GetSize() const;

    Node<T>* FindFreeBlock();

private:
    NodeEx<T> m_pHeadNode;
    Node<T>* m_ArrayListData = nullptr;
    NodeEx<T> m_pLastNode;

    Iterator<T> m_pHead;
    Iterator<T> m_pLast;
    size_t m_ArraySize;
    size_t m_Index;
    size_t m_ListSize;
    size_t m_NumElements;
    Iterator<T> m_Iterator;
    BlockState<Node<T>>* m_pPool = nullptr;
};

template<class T>
CDoubleLinkedArrayList<T>::CDoubleLinkedArrayList(size_t num_items){
    m_pHead.Set(&m_pHeadNode);
    m_pHeadNode.IsHead = true;
    m_pHeadNode.pNext = &m_pLastNode;
    m_pLast.Set(&m_pLastNode);
    m_pLastNode.pPrev = &m_pHeadNode;
    m_ArraySize = num_items * sizeof(T);
    m_Index = 0;
    m_ListSize = 0;
    m_Iterator = m_pHead;
    m_NumElements = num_items;
    m_ArrayListData = new Node<T>[num_items];
    if(!(nullptr == m_ArrayListData)){
        for (size_t i = 0; i < num_items; i++) {
            m_ArrayListData[i].id = i;
        }
    }
}

template<class T>
CDoubleLinkedArrayList<T>::CDoubleLinkedArrayList(size_t num_items, CMemoryPool<Node<T>>* mem_pool) {
    m_pHead.Set(&m_pHeadNode);
    m_pHeadNode.IsHead = true;
    m_pHeadNode.pNext = &m_pLastNode;
    m_pLast.Set(&m_pLastNode);
    m_pLastNode.pPrev = &m_pHeadNode;
    m_ArraySize = num_items * sizeof(T);
    m_Index = 0;
    m_ListSize = 0;
    m_Iterator = m_pHead;
    m_NumElements = num_items;

    BlockState<Node<T>>* pBlock= mem_pool->RequestData(num_items);
    m_ArrayListData = pBlock->pBlock;
    m_pPool = pBlock;
    if (!(nullptr == m_ArrayListData)) {
        for (size_t i = 0; i < num_items; i++) {
            m_ArrayListData[i].id = i;
        }
    }
}

template<class T>
CDoubleLinkedArrayList<T>::~CDoubleLinkedArrayList(){
    if (m_ArrayListData) {
        size_t num_items = m_NumElements;
        for (size_t i = 0; i < num_items; i++) {
            m_ArrayListData[i].active = false;
        }
        if (m_pPool) {
            m_pPool->active = false;
        }
        else {
            delete[] m_ArrayListData;
        }
    }
}

template<class T>
Iterator<T>* CDoubleLinkedArrayList<T>::GetIterator(){
    return &m_Iterator;
}

template<class T>
Iterator<T>* CDoubleLinkedArrayList<T>::GetBackIterator(){
    return &m_pHead;
}

template<class T>
Iterator<T>* CDoubleLinkedArrayList<T>::GetFrontIterator(){
    return &m_pLast;
}

template<class T>
Node<T>* CDoubleLinkedArrayList<T>::AddFront(T item){
    Node<T>* tail = m_pLast.GetData();
    Node<T>* data = FindFreeBlock();
    if(data != nullptr){
        if(tail != nullptr){
            //tail->pNext = data;
        } else {
            m_pHead.Set(data);
        }
        memcpy(data, &item, sizeof(T));
        data->active = true;
        //data->obj = item;
        if (tail->pPrev)
            tail->pPrev->pNext = data;
        data->pPrev = tail->pPrev;
        tail->pPrev = data;
        data->pNext = tail;
        m_ListSize++;
        return data;
    }
    return nullptr;
}

template<class T>
Node<T>* CDoubleLinkedArrayList<T>::AddBack(T item){
    Node<T>* head = m_pHead.GetData();
    Node<T>* data = FindFreeBlock();
    if(data != nullptr){
        if(head != nullptr){
            //head->pPrev = data;
        } else {
            m_pLast.Set(data);
        }
        memcpy(data, &item, sizeof(T));
        data->active = true;
        //data->obj = item;
        if (head->pNext)
            head->pNext->pPrev = data;
        data->pNext = head->pNext;
        head->pNext = data;
        data->pPrev = head;
        m_ListSize++;
        return data;
    }
    return nullptr;
}

template<class T>
bool CDoubleLinkedArrayList<T>::DeleteFront(){
    Node<T>* it = m_pLast.GetData();
    if(it != nullptr){
        if(it->pPrev != nullptr){
            it->pPrev->pNext = nullptr;
            m_ListSize--;
        }
        it->active = false;
        return true;
    }
    return true;
}

template<class T>
bool CDoubleLinkedArrayList<T>::DeleteFromIndex(size_t idx) {
    if (idx < m_NumElements) {
        m_ArrayListData[idx].active = false;
        if (m_ArrayListData[idx].pNext)
            m_ArrayListData[idx].pNext->pPrev = m_ArrayListData[idx].pPrev;
        if (m_ArrayListData[idx].pPrev)
            m_ArrayListData[idx].pPrev->pNext = m_ArrayListData[idx].pNext;
        m_ListSize--;
        return true;
    }
    return false;
}

template<class T>
bool CDoubleLinkedArrayList<T>::DeleteBack(){
    Node<T>* it = m_pHead.GetData();
    if(it != nullptr){
        if(it->pNext != nullptr){
            it->pNext->pPrev = nullptr;
            m_ListSize--;
        }
        it->active = false;
        return true;
    }
    return true;
}

template<class T>
size_t CDoubleLinkedArrayList<T>::GetSize() const{
    return m_ListSize;
}

template<class T>
Node<T>* CDoubleLinkedArrayList<T>::FindFreeBlock(){
    size_t ListSize = m_NumElements;
    Node<T>* pLeft = &m_ArrayListData[0];
    Node<T>* pRight = &m_ArrayListData[ListSize - 1];
    if(ListSize % 2){
        size_t times = (ListSize + 1) >> 1;
        for(size_t i = 0; i < times; i++, pRight--, pLeft++){
            if(!pLeft->active)
                return pLeft;
            
            if(!pRight->active)
                return pRight;
        }
    } else {
        size_t times = ListSize >> 1;
        for(size_t i = 0; i < times; i++, pRight--, pLeft++){
            if(!pLeft->active)
                return pLeft;

            if(!pRight->active)
                return pRight;
            
        }
    }
    return nullptr;
}

#endif