#ifndef SQHANDLERS_INCLUDED
#define SQHANDLERS_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <cstdio>
#include <vector>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>

#pragma comment(lib, "squirrel.lib")
#pragma comment(lib, "sqstdlib.lib")


SQInteger GetCnt(SQUserPointer file); 

//Compiler error handler
void SQCompilerErrorHandler(HSQUIRRELVM vm, const SQChar* Desc, const SQChar* Src, SQInteger line, SQInteger col);
SQInteger SQErrorFunction(HSQUIRRELVM vm);
static SQInteger PrintCallStack(HSQUIRRELVM v) {
	int i = 1;
	std::string msg;
	SQStackInfos sqstack;
	sq_getprintfunc(v)(v, "", msg.c_str());
	while (SQ_SUCCEEDED(sq_stackinfos(v, i, &sqstack))) {
		msg += ((sqstack.funcname) ? std::string(sqstack.funcname) : std::string("Anonymous function")) +
			"@" + std::string(sqstack.source) + ":" + std::to_string(sqstack.line) + "\n";
		i++;
	}
	sq_getprintfunc(v)(v, "---------CALL STACK---------\n"
		                  "%s", msg.c_str());
	return 0;
}

//Compile script
bool CompileSQScript(const char* FileName, HSQUIRRELVM vm);
bool CompileMemSQScript(const char* Src, HSQUIRRELVM vm);
SQInteger CompileIncludedFile(HSQUIRRELVM v);

//Print function (provisional)
void PrintFn(HSQUIRRELVM vm, const SQChar* message, ...);

//Registers a no parameter function
void RegisterSQFunc(HSQUIRRELVM v, SQFUNCTION func, const char* funcName);
inline void RegisterSQClassFunc(HSQUIRRELVM v, SQFUNCTION func, const char* funcName);

struct SquirrelFunc {
	SQFUNCTION func;
	const char* name;
};

typedef std::vector<SquirrelFunc> SQClassFunctions;

//inline bool CreateSQClassAndFunctions(HSQUIRRELVM v, const char* ClassName, const SQClassFunctions& functable, SQFUNCTION constructor);

//Executes a function with no parameters
inline void CallNPSQFunc(HSQUIRRELVM v, const char* funcName);




//Implementation
void CallNPSQFunc(HSQUIRRELVM v, const char* funcName) {
#ifdef _DEBUG
	sq_pushroottable(v);
	sq_pushstring(v, _SC(funcName), -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		sq_call(v, 1, SQFalse, SQTrue);
		sq_pop(v, 2);
	}
	else {
		sq_pop(v, 1);
		std::string err = "Error while calling function " + std::string(funcName);
		MessageBoxA(NULL, err.c_str(), "Squirrel Error", MB_ICONERROR);
	}
#else
	sq_pushroottable(v);
	sq_pushstring(v, _SC(funcName), -1);
	sq_get(v, -2);
	sq_pushroottable(v);
	sq_call(v, 1, SQFalse, SQTrue);
	sq_pop(v, 2);
#endif
}

inline SQObjectType SQGetType(HSQUIRRELVM v, SQInteger index) {
	return sq_gettype(v, index);
}

inline size_t SQGetArgc(HSQUIRRELVM v) {
	return sq_gettop(v);
}

inline float SQGetInteger(HSQUIRRELVM v, SQInteger index) {
	SQInteger i = -1.0;
	sq_getinteger(v, index, &i);
	return i;
}

inline float SQGetFloat(HSQUIRRELVM v, SQInteger index) {
	SQFloat f = -1.0;
	sq_getfloat(v, index, &f);
	return f;
}

inline void PrintStack(HSQUIRRELVM v) {
	HSQOBJECT obj;
	const SQChar* pStr;
	printf("---------STACK---------\n");
	for (int i = 1; i <= sq_gettop(v); i++) {
		SQObjectType t = sq_gettype(v, -i);
		printf("-%d: type: ", i);
		sq_getstackobj(v, -i, &obj);
		switch (t) {
		case SQObjectType::OT_NULL:
			printf("NULL"); break;
		case SQObjectType::OT_INTEGER:
			printf("INTEGER %d", obj._unVal.nInteger); break;
		case SQObjectType::OT_FLOAT:
			printf("FLOAT %f", obj._unVal.fFloat); break;
		case SQObjectType::OT_BOOL:
			printf("BOOL %d", obj._unVal.nInteger); break;
		case SQObjectType::OT_STRING:
			sq_getstring(v, -i, &pStr);
			printf("STRING %s", pStr); break;
		case SQObjectType::OT_TABLE:
			printf("TABLE %p", obj._unVal.pTable); break;
		case SQObjectType::OT_ARRAY:
			printf("ARRAY %p", obj._unVal.pArray); break;
		case SQObjectType::OT_USERDATA:
			printf("USERDATA %p", obj._unVal.pUserData); break;
		case SQObjectType::OT_CLOSURE:
			printf("CLOSURE %p", obj._unVal.pClosure); break;
		case SQObjectType::OT_NATIVECLOSURE:
			printf("NATIVECLOSURE %p"); break;
		case SQObjectType::OT_GENERATOR:
			printf("GENERATOR %p"); break;
		case SQObjectType::OT_USERPOINTER:
			printf("USERPOINTER"); break;
		case SQObjectType::OT_THREAD:
			printf("THREAD %p", obj._unVal.pThread); break;
		case SQObjectType::OT_FUNCPROTO:
			printf("FUNCPROTO"); break;
		case SQObjectType::OT_CLASS:
			printf("CLASS %p", obj._unVal.pClass); break;
		case SQObjectType::OT_INSTANCE:
			printf("INSTANCE %p", obj._unVal.pInstance); break;
		case SQObjectType::OT_WEAKREF:
			printf("WEAKREF"); break;
		case SQObjectType::OT_OUTER:
			printf("OUTER"); break;
		}
		printf("\n");
	}
}

#define SQExPrint(v, x) x; PrintStack(v);

void RegisterSQClassFunc(HSQUIRRELVM v, SQFUNCTION func, const char* funcName) {
	bool slot_create;
	sq_pushstring(v, funcName, -1);
	sq_newclosure(v, func, 0);
	sq_setnativeclosurename(v, -1, funcName);
	slot_create = SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
}

template<class T>
SQInteger DefaultSQClassDestructor(SQUserPointer pKbd, SQInteger size) {
	T* pInstance = (T*)pKbd;
	if (pInstance) delete pInstance;
	return SQ_OK;
}

template<class T>
SQInteger DefaultSQClassConstructor(HSQUIRRELVM v) {
	if (sq_gettop(v) == 1) {
		T* pKbd = new T();
		sq_setinstanceup(v, -1, pKbd);
		sq_setreleasehook(v, -1, DefaultSQClassDestructor<T>);
	}
	return 0;
}


template<class T, T* (*func)(HSQUIRRELVM), SQInteger (*dest)(SQUserPointer, SQInteger) = DefaultSQClassDestructor<T>>
SQInteger SQClassConstructor(HSQUIRRELVM v) {
	if (sq_gettop(v) >= 1) {
		T* pClass = func(v);
		sq_setinstanceup(v, -1, pClass);
		sq_setreleasehook(v, -1, dest);
	}
	return 0;
}

//void(*constructor)(HSQUIRRELVM, T*) SQClassConstructorFunc<T>;

template<class T, T*(*constructor)(HSQUIRRELVM) = nullptr, bool autocreate = true>
bool CreateSQClassAndFunctions(HSQUIRRELVM v, const char* ClassName, const SQClassFunctions& functable) {
	bool create_class;
	sq_pushstring(v, ClassName, -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, SQFalse));
	if (constructor == nullptr && autocreate) {
		//User implements and adds itself the constructor if autocreate is disabled
		RegisterSQClassFunc(v, DefaultSQClassConstructor<T>, "constructor");
	}
	else {
		RegisterSQClassFunc(v, SQClassConstructor<T,constructor>, "constructor");
	}
	for (auto& c : functable) {
		RegisterSQClassFunc(v, c.func, c.name);
	}
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif