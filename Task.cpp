#include "Task.h"


static CMemoryPool<Node<Task>> g_TaskPool;
static CMemoryPool<Node<TaskDrawable2D>> g_TaskDrawable2DPool;
static CMemoryPool<Node<TaskDrawable3D>> g_TaskDrawable3DPool;


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
		//if(sq_getvmstate(pTask->main_thread) == SQ_VMSTATE_RUNNING) sq_suspendvm(pTask->main_thread);
	}
	return 0;
}

SQInteger Task2D_SetPos(HSQUIRRELVM v) {
	union {
		TaskDrawable2D* pTask;
		SQUserPointer pData;
	};
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getinstanceup(v, -1, &pData2, NULL);
	pTask->pos = pVec->obj;
	return 0;
}

SQInteger Task2D_SetSize(HSQUIRRELVM v) {
	union {
		TaskDrawable2D* pTask;
		SQUserPointer pData;
	};
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getinstanceup(v, -1, &pData2, NULL);
	pTask->size = pVec->obj;
	return 0;
}

SQInteger Task2D_SetScale(HSQUIRRELVM v) {
	union {
		TaskDrawable2D* pTask;
		SQUserPointer pData;
	};
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getinstanceup(v, -1, &pData2, NULL);
	pTask->scale = pVec->obj;
	return 0;
}

SQInteger Task2D_SetRotation(HSQUIRRELVM v) {
	union {
		TaskDrawable2D* pTask;
		SQUserPointer pData;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getfloat(v, -1, &pTask->rotation);
	return 0;
}

SQInteger Task2D_SetUV(HSQUIRRELVM v) {
	union {
		TaskDrawable2D* pTask;
		SQUserPointer pData;
	};
	union {
		LinalgType<glm::vec4>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getinstanceup(v, -1, &pData2, NULL);
	pTask->uv = pVec->obj;
	return 0;
}



TaskManager::TaskManager() :
m_TaskList(512, &g_TaskPool) {

}

TaskManager::~TaskManager() {

}

void TaskManager::Initialize()
{
}

void TaskManager::Move() {
	if (!m_TaskList.GetSize())
		return;

	Node<Task>* pInst;
	Iterator<Task> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		Task test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList);
		it.MoveFront();
	}
}

Task2DManager::Task2DManager() :
	m_TaskList(TASK2DMAX, &g_TaskDrawable2DPool)
{

}

Task2DManager::~Task2DManager()
{

}

void Task2DManager::Initialize() {

}

void Task2DManager::Move() {
	if (!m_TaskList.GetSize())
		return;

	Node<TaskDrawable2D>* pInst;
	Iterator<TaskDrawable2D> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		TaskDrawable2D test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList);
		it.MoveFront();
	}
}


void Task2DManager::Draw() {
	static Clb184::CD3DSprite spr;
	if (!m_TaskList.GetSize())
		return;

	Node<TaskDrawable2D>* pInst;
	Iterator<TaskDrawable2D> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		TaskDrawable2D test = *pInst;
		spr.SetPos(test.pos);
		spr.SetRotation(test.rotation);

		spr.Draw();
		it.MoveFront();
	}

}