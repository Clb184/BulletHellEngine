#ifndef MEMORYPOOL_INCLUDED
#define MEMORYPOOL_INCLUDED

#include <stdlib.h>

template<class T>
struct BlockState {
	T* pBlock = nullptr;
	bool active = false;
	size_t size = 0;
};

template<class T>
class CMemoryPool {
public:
	CMemoryPool();
	~CMemoryPool();

	BlockState<T>* RequestData(size_t item_cnt);
	void FreeData();
private:
	BlockState<T>* AddNewBlock();
	bool Expand();
	void Cleanup();
private:
	BlockState<T>** m_pBlocks;
	size_t m_NumBlocks;
	size_t m_AllocBlocks;
};

template<class T>
CMemoryPool<T>::CMemoryPool() {
	const size_t start_data = 1;
	m_pBlocks = new BlockState<T>*[start_data];
	m_pBlocks[0] = new BlockState<T>;
	m_NumBlocks = 0;
	m_AllocBlocks = start_data;
}

template<class T>
CMemoryPool<T>::~CMemoryPool() {
	Cleanup();
}

template<class T>
BlockState<T>* CMemoryPool<T>::RequestData(size_t item_cnt) {
	BlockState<T>* pBlock = AddNewBlock();
	pBlock->pBlock = new T[item_cnt];
	pBlock->active = true;
	m_NumBlocks++;
	return pBlock;
}

template<class T>
void CMemoryPool<T>::FreeData() {
	size_t s = m_AllocBlocks;
	for (size_t i = 0; i < s;i++) {
		if (!m_pBlocks[i]->active) {
			if (m_pBlocks[i]->pBlock) {
				delete[] m_pBlocks[i]->pBlock;
				m_pBlocks[i]->pBlock = nullptr;
			}
		}
	}
}

template<class T>
BlockState<T>* CMemoryPool<T>::AddNewBlock() {
	size_t i = 0;
	for (i = 0; i < m_AllocBlocks; i++) {
		if (!m_pBlocks[i]->active) {
			if (m_pBlocks[i]->pBlock) {
				delete[] m_pBlocks[i]->pBlock;
			}
			return m_pBlocks[i];
		}
	}
	Expand();
	return m_pBlocks[i];
}

template<class T>
bool CMemoryPool<T>::Expand() {
	size_t new_size = m_AllocBlocks << 1;
	BlockState<T>** pBlocks = new BlockState<T>*[new_size];
	memcpy(pBlocks, m_pBlocks, sizeof(BlockState<T>*) * m_AllocBlocks);
	for (size_t i = m_AllocBlocks; i < new_size; i++) {
		pBlocks[i] = new BlockState<T>;
	}
	delete[] m_pBlocks;
	m_pBlocks = pBlocks;
	m_AllocBlocks = new_size;
	return true;
}

template<class T>
void CMemoryPool<T>::Cleanup() {
	for (size_t i = 0; i < m_AllocBlocks; i++) {
		try {
			if (m_pBlocks[i]->pBlock)
				delete[] m_pBlocks[i]->pBlock;
		}
		catch (...) {

		}

	}
	delete[] m_pBlocks;
}

#endif