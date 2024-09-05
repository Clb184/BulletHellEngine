#ifndef TASKDRAWABLE2D_INCLUDED
#define TASKDRAWABLE2D_INCLUDED
#include "Task.h"
#include "HitCalc.h"
#include "Graphics.h"

#define TASK2D_MAX (256)

struct TaskDrawable2D : Task {
	glm::vec2 pos = { 0.0f, 0.0f };
	glm::vec2 size = { 32.0f, 32.0f };
	glm::vec2 scale = { 1.0f, 1.0f };
	float rotation = 0.0f;
	glm::vec4 uv = { 0.0f, 0.0f, 1.0f, 1.0f };
	uint32_t color = 0xffffffff;
};

struct TaskDrawable3D : Task {
	glm::vec3 pos;
	glm::mat4 transform;
};

struct TaskCollideableCircle : TaskDrawable2D {
	CollissionCircle co_shape;
	bool is_hit = false;
};

class Task2DManager : public CDrawableManager<TaskDrawable2D> {
public:
	Task2DManager();
	~Task2DManager();

	void Initialize() {}
	void Initialize(const SQChar* sq_list);
	void Move();
	void Draw();

	int GetItemCnt() const;
	Node<TaskDrawable2D>* CreateObject();
private:
};


inline bool Task2DMemberSetup(HSQUIRRELVM v) {
	bool member_set = TaskMemberSetup(v);
	sq_pushstring(v, _SC("texture"), -1);
	sq_pushnull(v);
	member_set &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return member_set;
}

static void Task2DInitialize(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {
	//pTask->texture_handle = SQGetObjectByName(v, _SC("texture"));
}

SQInteger Task2D_SetPos(HSQUIRRELVM v);
SQInteger Task2D_SetSize(HSQUIRRELVM v);
SQInteger Task2D_SetScale(HSQUIRRELVM v);
SQInteger Task2D_SetRotation(HSQUIRRELVM v);
SQInteger Task2D_SetUV(HSQUIRRELVM v);

SQInteger Collision_SetRadius(HSQUIRRELVM v);


static void Task2DListSetup0(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____fx0"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}
static void Task2DListSetup1(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____fx1"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}
static void Task2DListSetup2(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____fx2"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}

static void Task2DListSetup3(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____fx3"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}
static void Task2DListSetup4(HSQUIRRELVM v, Node<TaskDrawable2D>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____fx4"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}

template<Task2DManager *TaskList, void(*Task2DListSetup)(HSQUIRRELVM, Node<TaskDrawable2D>*) >
inline bool RegisterEffectClass(HSQUIRRELVM v, const char* name) {
	bool create_class = false;
	sq_pushstring(v, name, -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	Task2DMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<TaskDrawable2D, Task2DManager, TaskList, Task2DListSetup, Task2DInitialize>, "constructor");
	RegisterSQClassFunc(v, Task_Kill, "Kill");
	RegisterSQClassFunc(v, Task2D_SetPos, "SetPos");
	RegisterSQClassFunc(v, Task2D_SetSize, "SetSize");
	RegisterSQClassFunc(v, Task2D_SetScale, "SetScale");
	RegisterSQClassFunc(v, Task2D_SetUV, "SetUV");
	RegisterSQClassFunc(v, Task2D_SetRotation, "SetRotation");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif