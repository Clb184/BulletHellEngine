#ifndef GRAPHICS_INCLUDED
#define GRAPHICS_INCLUDED


#include <stdint.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ClbLibMain.h"

constexpr float HALF_PI = 3.14159f * 0.5f;

struct FRect {
	union {
		float left;
		float x;
	};
	union {
		float top;
		float y;
	};
	union {
		float right;
		float w;
	};
	union {
		float bottom;
		float h;
	};
};

#endif