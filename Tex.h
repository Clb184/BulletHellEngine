#ifndef TEXTURESQ_INCLUDED
#define TEXTURESQ_INCLUDED
#include "Direct3D/Texture.h"
#include "SqHandlers.h"


SQInteger Texture_Constructor(HSQUIRRELVM v);
SQInteger Texture_Bind(HSQUIRRELVM v);

inline bool RegisterTextureClass(HSQUIRRELVM v) {
	bool create_class = true;
	sq_pushstring(v, _SC("Texture"), -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	RegisterSQClassFunc(v, Texture_Constructor, _SC("constructor"));
	RegisterSQClassFunc(v, Texture_Bind, _SC("Bind"));
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif