#include "Task.h"

static CMemoryPool<Node<Task>> g_TaskPool;

SQInteger Task_Kill(HSQUIRRELVM v) {
	union {
		Task* pTask;
		SQUserPointer pData;
	}; 
	
	if (sq_gettop(v) == 1) {
		sq_getinstanceup(v, 1, &pData, NULL);
		pTask->is_delete = true;
		if(v == pTask->main_thread)
			return sq_suspendvm(v);
	}
	return 0;
}

TaskManager::TaskManager() :
CManagerBase<Task>(TASK_MAX, &g_TaskPool) {

}

TaskManager::~TaskManager() {

}

void TaskManager::Initialize() {
	if (m_VM) {
		m_ArrayObj = SQCreateArray(m_VM, _SC("____tsk"), TASK_MAX);
	}
}

void TaskManager::Move() {
	if (!m_TaskList.GetSize())
		return;

	Node<Task>* pInst;
	Iterator<Task> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		Task test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
		it.MoveFront();
	}
}

int TaskManager::GetItemCnt() const {
	return m_TaskList.GetSize();
}
