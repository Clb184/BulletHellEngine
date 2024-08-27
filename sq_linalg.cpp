#include "sq_linalg.h"


glm::vec2 CreateFullVec2(HSQUIRRELVM v) { return glm::vec2(SQGetFloat(v, -2), SQGetFloat(v, -1)); }
glm::vec3 CreateFullVec3(HSQUIRRELVM v) { return glm::vec3(SQGetFloat(v, -3), SQGetFloat(v, -2), SQGetFloat(v, -1)); }
glm::vec4 CreateFullVec4(HSQUIRRELVM v) { return glm::vec4(SQGetFloat(v, -4), SQGetFloat(v, -3), SQGetFloat(v, -2), SQGetFloat(v, -1)); }
