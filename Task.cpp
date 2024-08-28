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

SQInteger Collision_SetRadius(HSQUIRRELVM v) {
	union {
		TaskCollideableCircle* pHitbox;
		SQUserPointer pData;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getfloat(v, -1, &pHitbox->co_shape.r);
	return 0;
}



TaskManager::TaskManager() :
m_TaskList(TASK_MAX, &g_TaskPool) {

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

Task2DManager::Task2DManager() :
	m_TaskList(TASK2D_MAX, &g_TaskDrawable2DPool)
{

}

Task2DManager::~Task2DManager()
{

}

void Task2DManager::Initialize() {
	if (m_VM) {
		m_ArrayObj = SQCreateArray(m_VM, _SC("____tsk"), TASK2D_MAX);
	}
}

void Task2DManager::Move() {
	if (!m_TaskList.GetSize())
		return;

	Node<TaskDrawable2D>* pInst;
	Iterator<TaskDrawable2D> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		TaskDrawable2D test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
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