#include "Light.h"

Light::Light() : m_Color(1.0)  {

}

glm::vec4 Light::getColor() {
	return this->m_Color;
}

glm::vec3 Light::getPos() {
	return this->m_Pos;
}