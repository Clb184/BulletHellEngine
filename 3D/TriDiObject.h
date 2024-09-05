#ifndef TRIDIOBJECT_INCLUDED
#define TRIDIOBJECT_INCLUDED
#include "OBJModel.h"
#include "../Graphics.h"
#include "../SqHandlers.h"
#include <filesystem>

typedef std::map<std::filesystem::path, OBJModel*> _3DModelCache;

class TriDiObject {
public:
	//Constructor and Destructor
	TriDiObject();
	~TriDiObject();

	//Load model
	void LoadModel(std::filesystem::path file);



	//Drawing normal or instanced
	void Draw();
	void DrawInstanced(size_t cnt);

private:
	OBJModel* m_pTriDiModel = nullptr;

//For usage on this implementation with Direct3D11
private:
	//static _3DModelCache m_ModelCache;

};

SQInteger Model_Load(HSQUIRRELVM v);
SQInteger Model_Draw(HSQUIRRELVM v);
SQInteger Model_DrawInstanced(HSQUIRRELVM v);

inline bool RegisterModelClass(HSQUIRRELVM v) {
	SQClassFunctions funcs = {
		{Model_Load,			"Load"},
		{Model_Draw,			"Draw"},
		{Model_DrawInstanced,	"DrawInstanced"},
	};
	return CreateSQClassAndFunctions<TriDiObject>(v, "Model", funcs);
}

#endif