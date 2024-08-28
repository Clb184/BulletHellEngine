#include "SqHandlers.h"

SQInteger GetCnt(SQUserPointer file) {
	int ret;
	char c;
	if ((ret = fread(&c, sizeof(c), 1, (FILE*)file) > 0))
		return c;
	return 0;
}

void SQCompilerErrorHandler(HSQUIRRELVM vm, const SQChar* Desc, const SQChar* Src, SQInteger line, SQInteger col) {
	std::string estr;

	estr =
		"Error in line " + std::to_string(line) + ", column " + std::to_string(col) + " - " + std::string((const char*)Src) + "\n" +
		"Desc: " + std::string((const char*)Desc);
	std::wstring msg = std::wstring(estr.begin(), estr.end());

	MessageBox(NULL, estr.c_str(),TEXT("Squirrel Error"), MB_ICONERROR);
}


HSQOBJECT SQCreateArray(HSQUIRRELVM v, const SQChar* arrayname, int num_elements) {
	HSQOBJECT ret = {};
	sq_pushstring(v, arrayname, -1);
	sq_newarray(v, num_elements);
	sq_newslot(v, -3, SQFalse);
	sq_pushstring(v, arrayname, -1);
	sq_get(v, -2);
	sq_getstackobj(v, -1, &ret);
	sq_pop(v, 1);
	return ret;
}

HSQOBJECT SQGetObjectByName(HSQUIRRELVM v, const SQChar* name) {
	HSQOBJECT ret = {};
	sq_pushstring(v, name, -1);
	sq_get(v, -2);
	sq_getstackobj(v, -1, &ret);
	sq_pop(v, 1);
	return ret;
}

SQInteger SQErrorFunction(HSQUIRRELVM vm) {
	if (sq_gettop(vm) >= 1) {
		SQStackInfos sqstack;
		const SQChar* error_message = 0;
		std::string msg;
		if (SQ_SUCCEEDED(sq_getstring(vm, 2, &error_message))) {
			msg =
				"Error: \"" + std::string(error_message) + "\"\n" +
				"---------------------------------------------------\n" +
				"                       STACK                       \n" +
				"---------------------------------------------------\n" +
				">>>";
			int i = 1;
			while (SQ_SUCCEEDED(sq_stackinfos(vm, i, &sqstack))) {
				msg += ((sqstack.funcname) ? std::string(sqstack.funcname) : std::string("Anonymous function")) +
					"@" + std::string(sqstack.source) + ":" + std::to_string(sqstack.line) + "\n";
				i++;
			}
			//PrintStack(vm);
			MessageBoxA(NULL, msg.c_str(), "Squirrel Runtime Error", MB_ICONEXCLAMATION);
		}
	}
	return SQ_OK;
}

bool CompileSQScript(const char* FileName, HSQUIRRELVM vm) {
	bool is_compiled = false;
	FILE* fp = fopen(FileName, "rb");
	if (fp) {
		is_compiled = SQ_SUCCEEDED(sq_compile(vm, GetCnt, fp, _SC(FileName), 1));

		/*
		if (is_compiled)
			;// MessageBox(NULL, TEXT("Script compiled succesfuly."), TEXT("Squirrel Info"), MB_ICONINFORMATION);
		else
			;// MessageBox(NULL, TEXT("Couldn't compile script."), TEXT("Squirrel Info"), MB_ICONERROR);
			*/
		fclose(fp);


		sq_pushroottable(vm);
		sq_call(vm, 1, SQFalse, SQTrue);
		sq_pop(vm, -1);
		return is_compiled;
	}
	return false;
}

bool CompileMemSQScript(const char* Src, HSQUIRRELVM vm) {
	bool is_compiled = false;
	if (Src) {
		is_compiled = SQ_SUCCEEDED(sq_compilebuffer(vm, Src, strlen(Src), _SC("inline"), SQTrue));

		if (is_compiled) {
			//MessageBox(NULL, TEXT("Script compiled succesfuly."), TEXT("Squirrel Info"), MB_ICONINFORMATION);
			sq_pushroottable(vm);
			sq_call(vm, 1, SQFalse, SQTrue);
			sq_pop(vm, -1);
		}
		else
			;// MessageBox(NULL, TEXT("Couldn't compile script."), TEXT("Squirrel Info"), MB_ICONERROR);

		return is_compiled;
	}
	return false;
}

void PrintFn(HSQUIRRELVM vm, const SQChar* message, ...) {
	char buffer[1024] = {};
	va_list marker;
	va_start(marker, message);
	vsnprintf(buffer, 1024, (const char*)message, marker);
	va_end(marker);
	printf("%s", buffer);
	//MessageBoxA(NULL, buffer, "Squirrel Info", MB_ICONINFORMATION);
}

void RegisterSQFunc(HSQUIRRELVM v, SQFUNCTION func, const char* funcName) {
	sq_pushstring(v, funcName, -1);
	sq_newclosure(v, func, 0);
	sq_newslot(v, -3, SQFalse);
}

SQInteger CompileIncludedFile(HSQUIRRELVM v) {
	SQInteger argc = sq_gettop(v);
	const SQChar* str = nullptr;
	if (sq_gettype(v, -1) == OT_STRING) {

		sq_getstring(v, -1, &str);
		sq_pop(v, -1);
		CompileSQScript(str, v);
	}
	return 0;
}
