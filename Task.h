#ifndef TASK_INCLUDED
#define TASK_INCLUDED

#include "SqHandlers.h"
#include "sq_linalg.h"
#include "ClbLibMain.h"
#include "CManagerBase.h"
#include "CDrawableManager.h"
#include "LinkedArrayList.h"
#include "MemoryPool.h"

#define TASK_MAX (512)

struct Task {
	bool is_delete = false;
	//bool is_maincreated = false;
	//uint16_t create_flags = 0x0;
	uint32_t flags = 0x00;
	HSQUIRRELVM main_thread = nullptr;
	HSQUIRRELVM threads[16] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	Task* parent = nullptr;
	Task* child[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	SQClosure* interrupt;
	SQInteger count;
};


static SQInteger DummyRelease(SQUserPointer, SQInteger) {
	return SQ_OK;
}

inline bool TaskMemberSetup(HSQUIRRELVM v) {
	bool member_set = true;
	sq_pushstring(v, _SC("____task"), -1);
	sq_pushnull(v);
	member_set &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return member_set;
}


/*
Setup a task, avoiding inheritance or at least trying so
On each class register for tasks, you enter the following:
-The class (Well, struct) for the Task
-The class generator
-The list setup (You need to push the root table and manage yourself)
-The member initializer (and caching? if you want, in this case, the instance is pushed before)
*/
template <class T, class L, L* list, void (*list_setup)(HSQUIRRELVM, Node<T>*), void (*initializer)(HSQUIRRELVM, Node<T>*)>
static SQInteger Task_Constructor(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Node<T>* pTask;
	};
	SQInteger top = sq_gettop(v);
	SQInteger numparams;

	if (top >= 2 && (sq_gettype(v, 2) == OT_CLOSURE)) {
		pTask = list->CreateObject();
		if (!pTask) return 0;
		sq_setinstanceup(v, 1, pData);
		sq_setreleasehook(v, 1, DummyRelease);
		numparams = top - 2;
		sq_pushstring(v, _SC("____task"), -1);
		pTask->main_thread = sq_newthread(v, top + 2 + 15);
		pTask->is_delete = false;
		pTask->active = true;
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
		sq_call(pTask->main_thread, numparams + 1, SQFalse, SQTrue);
		sq_set(v, 1);
		list_setup(v, pTask);
		sq_move(v, v, 1);
		initializer(v, pTask);
		sq_pop(v, 1);

	}
	return 0;
}

SQInteger Task_InterruptMain(HSQUIRRELVM v);
SQInteger Task_AsyncTask(HSQUIRRELVM v);
SQInteger Task_GetParent(HSQUIRRELVM v);
SQInteger Task_GetChild(HSQUIRRELVM v);
SQInteger Task_Kill(HSQUIRRELVM v);


template <class T>
inline void MoveTask(HSQUIRRELVM v, Node<T>* pTask, CDoubleLinkedArrayList<T>* pList, HSQOBJECT arrayobj) {
	bool ret = false;
	if (pTask->main_thread) {
		if (sq_getvmstate(pTask->main_thread) == SQ_VMSTATE_SUSPENDED && !pTask->is_delete) {
			sq_wakeupvm(pTask->main_thread, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		//assert(!pTask->is_delete && (sq_getvmstate(pTask->main_thread) != SQ_VMSTATE_IDLE));
		if (sq_getvmstate(pTask->main_thread) == SQ_VMSTATE_IDLE || pTask->is_delete) {
			//pTask->main_thread = nullptr;
			sq_pushroottable(v);
			sq_pushobject(v, arrayobj);
			sq_pushinteger(v, pTask->id);
			sq_pushnull(v);
			sq_set(v, -3);
			sq_pop(v, 2);
			pList->DeleteFromIndex(pTask->id);
			return;
		}
	}

	//for (auto& v : pTask->threads) {
	//	if (!v) continue;
	//	if (sq_getvmstate(v) == SQ_VMSTATE_SUSPENDED) {
	//		sq_wakeupvm(v, SQFalse, SQFalse, SQTrue, SQFalse);
	//	}
	//	if (sq_getvmstate(v) == SQ_VMSTATE_IDLE)
	//		sq_close(v);
	//}
}

extern CMemoryPool<Node<Task>> g_TaskPool;

class TaskManager : public CManagerBase<Task> {
public:
	TaskManager();
	~TaskManager();

	void Initialize();
	void Move();
	int GetItemCnt() const;
private:
};
#endif