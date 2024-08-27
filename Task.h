#ifndef TASK_INCLUDED
#define TASK_INCLUDED

#include "SqHandlers.h"
#include "sq_linalg.h"
#include "ClbLibMain.h"
#include "Graphics.h"
#include "CManagerBase.h"
#include "LinkedArrayList.h"
#include "MemoryPool.h"

#define TASK2DMAX (256)

struct VMInstance {
	HSQUIRRELVM thread = nullptr;
	bool is_newcall = false;
};

struct Task {
	bool is_delete = false;
	bool is_maincreated = false;
	uint16_t create_flags = 0x0;
	uint32_t flags = 0x00;
	//HSQUIRRELVM main = nullptr;
	HSQUIRRELVM main_thread = nullptr;
	HSQUIRRELVM threads[16] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	Task* parent = nullptr;
	Task* child[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	SQClosure* interrupt;
	SQInteger count;
};

struct TaskDrawable2D : Task {
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec2 scale;
	float rotation;
	glm::vec4 uv;
};

struct TaskDrawable3D : Task {
	glm::vec3 pos;
	glm::mat4 transform;
};

static SQInteger TaskInit(HSQUIRRELVM v) {
	int t = sq_gettop(v);
	int numparams = t - 3;
	PrintStack(v);
	sq_bindenv(v, 2);
	sq_pushroottable(v);
	PrintStack(v);
	int i = 0;
	while (i++ < numparams) {
		sq_move(v, v, 2 + i);
	}
	PrintStack(v);
	sq_call(v, numparams + 1, SQFalse, SQTrue);
	return 0;
}

static SQInteger DummyRelease(SQUserPointer, SQInteger) {
	return SQ_OK;
}

template<class T>
static SQInteger ListRelease(SQUserPointer pData, SQInteger) {
	return SQ_OK;
}

template <class T, Node<T>* (*generator)(), SQRELEASEHOOK des>
static SQInteger Task_Constructor(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Node<T>* pTask;
	};
	SQInteger top = sq_gettop(v);
	SQInteger numparams;
	HSQOBJECT obj;

	if (top >= 2 && (sq_gettype(v, 2) == OT_CLOSURE)) {
		pTask = generator();
		if (!pTask) return 0;
		sq_getstackobj(v, 1, &obj);
		sq_setinstanceup(v, 1, pData);
		sq_setreleasehook(v, 1, des);
		numparams = top - 2;
		sq_pushstring(v, _SC("____task"), -1);
		pTask->main_thread = sq_newthread(v, top + 2 + 15);
		pTask->is_delete = false;
		//PrintStack(v);
		//sq_pushroottable(v2);
		//sq_pushstring(v2, _SC("TaskInit"), -1);
		//sq_get(v2, -2);
		sq_move(pTask->main_thread, v, 2);
		sq_move(pTask->main_thread, v, 1);
		sq_bindenv(pTask->main_thread, 1);
		sq_pushroottable(pTask->main_thread);
		int i = 0;
		while (i++ < numparams) {
			sq_move(pTask->main_thread, v, 2 + i);
		}
		//sq_move(v2, v, 1);
		//PrintStack(pTask->main_thread);
		sq_call(pTask->main_thread, numparams + 1, SQFalse, SQTrue);
		//sq_call(v2, numparams + 1 + 1 + 1, SQFalse, SQTrue);
		//pTask->main_thread = std::move(v2);
		sq_set(v, 1);
		//sq_pop(v, 1);
		sq_pushroottable(v);
		sq_pushstring(v, _SC("____enm"), -1);
		sq_get(v, -2);
		sq_move(v, v, 1);
		sq_arrayinsert(v, -2, pTask->id);
		sq_pop(v, 2);
	}
	return 0;
}

SQInteger Task_InterruptMain(HSQUIRRELVM v);
SQInteger Task_AsyncTask(HSQUIRRELVM v);
SQInteger Task_GetParent(HSQUIRRELVM v);
SQInteger Task_GetChild(HSQUIRRELVM v);
SQInteger Task_Kill(HSQUIRRELVM v);

SQInteger Task2D_SetPos(HSQUIRRELVM v);
SQInteger Task2D_SetSize(HSQUIRRELVM v);
SQInteger Task2D_SetScale(HSQUIRRELVM v);
SQInteger Task2D_SetRotation(HSQUIRRELVM v);
SQInteger Task2D_SetUV(HSQUIRRELVM v);

template <class T>
inline void MoveTask(HSQUIRRELVM v, Node<T>* pTask, CDoubleLinkedArrayList<T>* pList) {
	bool ret = false;
	if (pTask->main_thread && 0) {
		if (!pTask->is_maincreated) {
			SQInteger n;
			sq_getinteger(pTask->main_thread, -1, &n);
			sq_pop(pTask->main_thread, 1);
			sq_call(pTask->main_thread, n + 1, SQFalse, SQTrue);
			pTask->is_maincreated = true;
			ret = true;
		}
	}
	if (pTask->main_thread) {
		if (sq_getvmstate(pTask->main_thread) == SQ_VMSTATE_SUSPENDED && !pTask->is_delete) {
			sq_wakeupvm(pTask->main_thread, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		//assert(!pTask->is_delete && (sq_getvmstate(pTask->main_thread) != SQ_VMSTATE_IDLE));
		if (sq_getvmstate(pTask->main_thread) == SQ_VMSTATE_IDLE || pTask->is_delete) {
			//pTask->main_thread = nullptr;
			sq_pushroottable(v);
			sq_pushstring(v, _SC("____enm"), -1);
			sq_get(v, -2);
			sq_pushnull(v);
			sq_arrayinsert(v, -2, pTask->id);
			sq_pop(v, 2);
			pList->DeleteFromIndex(pTask->id);
			return;
		}
	}

	for (auto& v : pTask->threads) {
		if (!v) continue;
		if (sq_getvmstate(v) == SQ_VMSTATE_SUSPENDED) {
			sq_wakeupvm(v, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		if (sq_getvmstate(v) == SQ_VMSTATE_IDLE)
			sq_close(v);
	}
}

extern CMemoryPool<Node<Task>> g_TaskPool;
extern CMemoryPool<Node<TaskDrawable2D>> g_TaskDrawable2DPool;
extern CMemoryPool<Node<TaskDrawable3D>> g_TaskDrawable3DPool;

class TaskManager : public CManagerBase {
public:
	TaskManager();
	~TaskManager();

	void Initialize();
	void Move();
	void Draw();

private:
	CDoubleLinkedArrayList<Task> m_TaskList;
};

class Task2DManager : public CManagerBase {
public:
	Task2DManager();
	~Task2DManager();

	void Initialize();
	void Move();
	void Draw();

private:
	CDoubleLinkedArrayList<TaskDrawable2D> m_TaskList;
};

#endif