#ifndef COLLISION_INCLUDED
#define COLLISION_INCLUDED

#include "SquirrelMain.h"



SQInteger CollisionShape_SetPos(HSQUIRRELVM v);
SQInteger CollisionRect_SetSize(HSQUIRRELVM v);
SQInteger CollisionRectRot_SetDir(HSQUIRRELVM v);
SQInteger CollisionCircle_SetRadius(HSQUIRRELVM v);
SQInteger CollisionCheck(HSQUIRRELVM v);

bool RegisterCollisionClass(HSQUIRRELVM v);


#endif