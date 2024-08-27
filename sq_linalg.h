/*
*		=================================================
		Linear Algebra from GLM to Squirrel 3.1 by Clb184
		=================================================
	Includes:
	-Vectors
	-Matrices
	-Quaternions

	-Any vector operation including multiplication by matrices, dot product and cross product
	-Matrix multiplication
	-Quat multiplication

*/
#ifndef SQLINALG_INCLUDED
#define SQLINALG_INCLUDED
#include "SqHandlers.h"
#include "Graphics.h"

enum LINALG_TYPES {
	LINALG_FLOAT,
	LINALG_VEC2,
	LINALG_VEC3,
	LINALG_VEC4,
	LINALG_MAT2,
	LINALG_MAT3,
	LINALG_MAT4,
	LINALG_QUAT
};

template<class T>
struct LinalgType {
	LINALG_TYPES type;
	T obj;
};

template<class T>
LinalgType<T>* GetLinalgType(HSQUIRRELVM v, SQInteger index) {
	union {
		SQUserPointer pData = nullptr;
		HSQOBJECT pObj;
	};

	bool on_succeed = true;
	if (SQ_FAILED(sq_getuserdata(v, index, &pData, nullptr)))
		if (SQ_FAILED(sq_getuserpointer(v, index, &pData)))
			if (SQ_FAILED(sq_getinstanceup(v, index, &pData, nullptr)))
				on_succeed = false;
	return (LinalgType<T>*)pData;
}

template<class T>
SQInteger VecDestructor(SQUserPointer pKbd, SQInteger size) {
	LinalgType<T>* pInstance = (LinalgType<T>*)pKbd;
	if (pInstance) delete pInstance;
	return SQ_OK;
}

glm::vec2 CreateFullVec2(HSQUIRRELVM v);
glm::vec3 CreateFullVec3(HSQUIRRELVM v);
glm::vec4 CreateFullVec4(HSQUIRRELVM v);

//Vec2
template <class Vec, LINALG_TYPES num, Vec (*VecFunc)(HSQUIRRELVM)>
SQInteger RegisterVec(HSQUIRRELVM v) {
	size_t argc = SQGetArgc(v);
	LinalgType<Vec>* vec = nullptr;
	LinalgType<Vec>* t = nullptr;

	switch (argc - 1) {
	case 0:
		vec = new LinalgType<Vec>{ num, Vec() };
		break;
	case 1:
		switch (SQGetType(v, -1)) {
		case OT_INTEGER:
		case OT_FLOAT:
			vec = new LinalgType<Vec>{ num, Vec(SQGetFloat(v, -1)) };
			break;
		case OT_INSTANCE:
			t = GetLinalgType<Vec>(v, -1);
			if (t->type == LINALG_VEC2) {
				vec = new LinalgType<Vec>{ num, t->obj };
			}
			break;
		}
		break;
	case num + 1:
		vec = new LinalgType<Vec>{ num, VecFunc(v)};
		break;
	default:
		sq_pushnull(v);
		return 1;
		break;
	}
	sq_setinstanceup(v, -static_cast<int>(argc), vec);
	sq_setreleasehook(v, -static_cast<int>(argc), VecDestructor<Vec>);
	return 1;
}

template<class T, T (*func)(const T&, const T&)>
SQInteger VecOperation(HSQUIRRELVM v) {
	size_t argc = SQGetArgc(v);
	SQInteger kind = 0;
	float f2;
	union {
		LinalgType<T>* t = nullptr;
		SQUserPointer	pData;
	};
	union {
		LinalgType<T>* t2 = nullptr;
		SQUserPointer	pData2;
	};

	T vec;

	sq_getinstanceup(v, -2, &pData, NULL);
	switch (SQGetType(v, -1)) {
	case OT_INTEGER:
	case OT_FLOAT:
		f2 = SQGetFloat(v, -1);
		vec = T(f2);
		break;
	case OT_INSTANCE:
		sq_getinstanceup(v, -1, &pData2, NULL);
		vec = t2->obj;
		break;
	}
	HSQOBJECT sq;
	union {
		LinalgType<T>* t3 = nullptr;
		SQUserPointer	pData3;
	};
	sq_getclass(v, -2);
	sq_getstackobj(v, -1, &sq);
	sq_pop(v, 3);
	sq_pushobject(v, sq);
	sq_createinstance(v, -1);
	t3 = new LinalgType<T>;
	sq_setinstanceup(v, -1, pData3);
	sq_setreleasehook(v, -1, VecDestructor<T>);
	t3->obj = func(t->obj, vec);
	return 1;
}

template <class T>
T OpAdd(const T& v1, const T& v2) { return v1 + v2; }

template <class T>
T OpSub(const T& v1, const T& v2) { return v1 - v2; }

template <class T>
T OpMul(const T& v1, const T& v2) { return v1 * v2; }

template <class T>
T OpDiv(const T& v1, const T& v2) { return v1 / v2; }

template <class T>
SQInteger DotProduct(HSQUIRRELVM v) {
	union {
		LinalgType<T>* t = nullptr;
		SQUserPointer	pData;
	};
	union {
		LinalgType<T>* t2 = nullptr;
		SQUserPointer	pData2;
	};
	sq_getinstanceup(v, -2, &pData, nullptr);
	sq_getinstanceup(v, -1, &pData2, nullptr);
	SQFloat f = glm::dot(t->obj, t2->obj);
	sq_pop(v, 2);
	sq_pushfloat(v, f);
	return 1;
}

static SQInteger CrossProduct(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec3>* i3;
		SQUserPointer	pData;
	};
	union {
		LinalgType<glm::vec3>* o3;
		SQUserPointer	pData2;
	};
	sq_getinstanceup(v, -2, &pData, nullptr);
	sq_getinstanceup(v, -1, &pData2, nullptr);
	if(i3->type == LINALG_VEC3)
		i3->obj = glm::cross(i3->obj, o3->obj);

	return 0;
}

static SQInteger Vec_X(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec2>* vec;
		SQUserPointer	pData;
	};
	size_t argc = SQGetArgc(v);
	sq_getinstanceup(v, -static_cast<int>(argc), &pData, nullptr);
	switch (argc - 1) {
	case 0:
		sq_pop(v, 1);
		sq_pushfloat(v,	vec->obj.x);
		return 1;
	case 1:
		sq_getfloat(v, -1, &vec->obj.x);
		return 0;
	}
	return 0;
}

static SQInteger Vec_Y(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec2>* vec;
		SQUserPointer	pData;
	};
	size_t argc = SQGetArgc(v);
	sq_getinstanceup(v, -static_cast<int>(argc), &pData, nullptr);
	switch (argc - 1) {
	case 0:
		sq_pop(v, 1);
		sq_pushfloat(v, vec->obj.y);
		return 1;
	case 1:
		sq_getfloat(v, -1, &vec->obj.y);
		return 0;
	}
	return 0;
}

static SQInteger Vec_Z(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec3>* vec;
		SQUserPointer	pData;
	};
	size_t argc = SQGetArgc(v);
	sq_getinstanceup(v, -static_cast<int>(argc), &pData, nullptr);
	switch (argc - 1) {
	case 0:
		sq_pop(v, 1);
		sq_pushfloat(v, vec->obj.z);
		return 1;
	case 1:
		sq_getfloat(v, -1, &vec->obj.z);
		return 0;
	}
	return 0;
}

static SQInteger Vec_W(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec4>* vec;
		SQUserPointer	pData;
	};
	size_t argc = SQGetArgc(v);
	sq_getinstanceup(v, -static_cast<int>(argc), &pData, nullptr);
	switch (argc - 1) {
	case 0:
		sq_pop(v, 1);
		sq_pushfloat(v, vec->obj.w);
		return 1;
	case 1:
		sq_getfloat(v, -1, &vec->obj.w);
		return 0;
	}
	return 0;
}

inline bool RegisterLinearAlgebraClass(HSQUIRRELVM v) {
	bool ret = false;
	SQClassFunctions vec2funcs = {
		{RegisterVec<glm::vec2, LINALG_VEC2, CreateFullVec2>, "constructor"},
		{VecOperation <glm::vec2, OpAdd<glm::vec2>> , "_add"},
		{VecOperation <glm::vec2, OpSub<glm::vec2>> , "_sub"},
		{VecOperation <glm::vec2, OpMul<glm::vec2>> , "_mul"},
		{VecOperation <glm::vec2, OpDiv<glm::vec2>> , "_div"},
		{DotProduct<glm::vec2> , "dot"},
		{Vec_X, "x"},
		{Vec_Y, "y"},
	};

	SQClassFunctions vec3funcs = {
		{RegisterVec<glm::vec3, LINALG_VEC3, CreateFullVec3>, "constructor"},
		{VecOperation <glm::vec3, OpAdd<glm::vec3>> , "_add"},
		{VecOperation <glm::vec3, OpSub<glm::vec3>> , "_sub"},
		{VecOperation <glm::vec3, OpMul<glm::vec3>> , "_mul"},
		{VecOperation <glm::vec3, OpDiv<glm::vec3>> , "_div"},
		{DotProduct<glm::vec3> , "dot"},
		{CrossProduct, "cross"},
		{Vec_X, "x"},
		{Vec_Y, "y"},
		{Vec_Z, "z"},
	};

	SQClassFunctions vec4funcs = {
		{RegisterVec<glm::vec4, LINALG_VEC4, CreateFullVec4>, "constructor"},
		{VecOperation <glm::vec4, OpAdd<glm::vec4>> , "_add"},
		{VecOperation <glm::vec4, OpSub<glm::vec4>> , "_sub"},
		{VecOperation <glm::vec4, OpMul<glm::vec4>> , "_mul"},
		{VecOperation <glm::vec4, OpDiv<glm::vec4>> , "_div"},
		{DotProduct<glm::vec4> , "dot"},
		{Vec_X, "x"},
		{Vec_Y, "y"},
		{Vec_Z, "z"},
		{Vec_W, "w"},
	};

	return
	   CreateSQClassAndFunctions<LinalgType<glm::vec2>, nullptr, false>(v, "vec2t", vec2funcs)
	&& CreateSQClassAndFunctions<LinalgType<glm::vec3>, nullptr, false>(v, "vec3t", vec3funcs)
	&& CreateSQClassAndFunctions<LinalgType<glm::vec4>, nullptr, false>(v, "vec4t", vec4funcs);
}
#endif