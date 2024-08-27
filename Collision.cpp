#include "Collision.h"

#include "HitCalc.inl"

SQInteger CollisionShape_Destructor(SQUserPointer col, SQInteger size) {
	if (col) delete(col);
	return SQ_OK;
}

template<class T, CollissionType kind>
SQInteger CollisionShape_Constructor(HSQUIRRELVM v) {
	if (sq_gettop(v) == 1) {
		T* pCol = new T;
		//sq_clone(v, )
		pCol->type = kind;
		sq_setinstanceup(v, -1, pCol);
		sq_setreleasehook(v, -1, CollisionShape_Destructor);
	}

	return 0;
}

SQInteger CollisionShape_SetPos(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		CollissionShape* pCol;
	};
	SQFloat x;
	SQFloat y;
	//if (sq_gettop(v) == 3 &&
	//	(sq_gettype(v, -1) | sq_gettype(v, -2)) == OT_INTEGER) {
	sq_getinstanceup(v, -3, &pData, NULL);
	sq_getfloat(v, -2, &x);
	sq_getfloat(v, -1, &y);
	pCol->x = x;
	pCol->y = y;
	//}

	return 0;
}

SQInteger CollisionRect_SetSize(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		CollissionRectRot* pCol;
	};
	SQFloat x;
	SQFloat y;
	//if (sq_gettop(v) == 3 &&
	//	(sq_gettype(v, -1) | sq_gettype(v, -2)) == OT_INTEGER) {
	sq_getinstanceup(v, -3, &pData, NULL);
	sq_getfloat(v, -2, &x);
	sq_getfloat(v, -1, &y);
	pCol->w = x;
	pCol->h = y;
	//}

	return 0;
}

SQInteger CollisionRectRot_SetDir(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		CollissionRectRot* pCol;
	};
	SQFloat d;
	//if (sq_gettop(v) == 3 &&
	//	(sq_gettype(v, -1) | sq_gettype(v, -2)) == OT_INTEGER) {
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getfloat(v, -1, &d);
	pCol->d = d;
	//}

	return 0;
}

SQInteger CollisionCircle_SetRadius(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		CollissionCircle* pCol;
	};
	SQFloat r;
	//if (sq_gettop(v) == 3 &&
	//	(sq_gettype(v, -1) | sq_gettype(v, -2)) == OT_INTEGER) {
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getfloat(v, -1, &r);
	pCol->r = r;
	//}

	return 0;
}

SQInteger CollisionCheck(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		CollissionShape* pShape1;
	};
	union {
		SQUserPointer pData2;
		CollissionShape* pShape2;
	};
	sq_getinstanceup(v, -2, &pData, NULL);
	sq_getinstanceup(v, -1, &pData2, NULL);
	switch (pShape1->type) {
	case COLLISION_RECTROT:
		switch (pShape2->type) {
		case COLLISION_RECTROT:
			sq_pushbool(v, hitCheckBoxes(*(CollissionRectRot*)pShape1, *(CollissionRectRot*)pShape2));
			break;
		case COLLISION_CIRCLE:
			sq_pushbool(v, hitCheckRectCircle(*(CollissionCircle*)pShape2, *(CollissionRectRot*)pShape1));
			break;
		default:
			sq_pushbool(v, false);
			break;
		}
		break;
	case COLLISION_CIRCLE:
		switch (pShape2->type) {
		case COLLISION_RECTROT:
			sq_pushbool(v, hitCheckRectCircle(*(CollissionCircle*)pShape1, *(CollissionRectRot*)pShape2));
			break;
		case COLLISION_CIRCLE:
			sq_pushbool(v, hitCheckCC(*(CollissionCircle*)pShape1, *(CollissionCircle*)pShape2));
			break;
		default:
			sq_pushbool(v, false);
			break;
		}
		break;
	default:
		sq_pushbool(v, false);
		break;
	}
	return 1;
}

SQInteger PushUserPointer(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
	};
	sq_getinstanceup(v, -1, &pData, NULL);
	sq_pushuserpointer(v, pData);
	return 1;
}

bool RegisterCollisionClass(HSQUIRRELVM v) {
	bool create_class;
	//sq_pushstring(v, "CollisionRect", -1);
	//create_class = SQ_SUCCEEDED(sq_newclass(v, SQFalse));
	//RegisterClassFunc(v, CollisionShape_Constructor<CollissionRect, COLLISION_RECT>, "constructor");
	//RegisterClassFunc(v, CollisionShape_SetPos, "SetPos");
	//RegisterClassFunc(v, CollisionRect_SetSize, "SetSize");
	//create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	sq_pushstring(v, _SC("CoRect"), -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, SQFalse));

	sq_pushstring(v, _SC("constructor"), -1);
	sq_newclosure(v, CollisionShape_Constructor<CollissionRectRot, COLLISION_RECTROT>, 0);
	sq_setnativeclosurename(v, -1, _SC("constructor"));
	SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	sq_pushstring(v, _SC("constructor"), -1);
	sq_newclosure(v, CollisionShape_Constructor<CollissionRectRot, COLLISION_RECTROT>, 0);
	sq_setnativeclosurename(v, -1, _SC("constructor"));
	SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	sq_pushstring(v, _SC("constructor"), -1);
	sq_newclosure(v, CollisionShape_Constructor<CollissionRectRot, COLLISION_RECTROT>, 0);
	sq_setnativeclosurename(v, -1, _SC("constructor"));
	SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	sq_pushstring(v, _SC("SetDir"), -1);
	sq_newclosure(v, CollisionShape_Constructor<CollissionRectRot, COLLISION_RECTROT>, 0);
	sq_setnativeclosurename(v, -1, _SC("SetDir"));
	SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	RegisterSQClassFunc(v, CollisionShape_SetPos, "SetPos");
	RegisterSQClassFunc(v, CollisionRect_SetSize, "SetSize");
	RegisterSQClassFunc(v, CollisionRectRot_SetDir, "SetDir");
	RegisterSQClassFunc(v, PushUserPointer, "GetUP");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	sq_pushstring(v, "CoCircle", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, SQFalse));
	RegisterSQClassFunc(v, CollisionShape_Constructor<CollissionCircle, COLLISION_CIRCLE>, "constructor");
	RegisterSQClassFunc(v, CollisionShape_SetPos, "SetPos");
	RegisterSQClassFunc(v, CollisionCircle_SetRadius, "SetRadius");
	RegisterSQClassFunc(v, PushUserPointer, "GetUP");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));

	RegisterSQFunc(v, CollisionCheck, "CoCheck");

	return create_class;
}