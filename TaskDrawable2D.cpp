#include "TaskDrawable2D.h"
#include "GameMain.h"

Task2DManager::Task2DManager() :
	CDrawableManager<TaskDrawable2D>(TASK2D_MAX, &g_TaskDrawable2DPool)
{

}

Task2DManager::~Task2DManager()
{

}

void Task2DManager::Initialize(const SQChar* sq_list) {
	if (m_VM) {
		m_ArrayObj = SQCreateArray(m_VM, sq_list, TASK2D_MAX);
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
	if (!m_TaskList.GetSize())
		return;

	Node<TaskDrawable2D>* pInst;
	Iterator<TaskDrawable2D> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		TaskDrawable2D test = *pInst;

		it.MoveFront();
	}

}

int Task2DManager::GetItemCnt() const {
	return m_TaskList.GetSize();
}

Node<TaskDrawable2D>* Task2DManager::CreateObject() {
	return m_TaskList.AddFront();
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