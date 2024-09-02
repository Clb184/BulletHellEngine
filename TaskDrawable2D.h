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

extern CMemoryPool<Node<TaskDrawable2D>> g_TaskDrawable2DPool;
extern CMemoryPool<Node<TaskDrawable3D>> g_TaskDrawable3DPool;


struct TaskCollideableCircle : TaskDrawable2D {
	CollissionCircle co_shape;
	bool is_hit = false;
};

class Task2DManager : public CDrawableManager<TaskDrawable2D> {
public:
	Task2DManager();
	~Task2DManager();

	void Initialize();
	void Move();
	void Draw();

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

#endif