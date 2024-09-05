#ifndef MATERIAL_INCLUDED
#define MATERIAL_INCLUDED
#include "..\Graphics.h"
#include <filesystem>
#include <vector>

class ModelMaterial {
public:
	void loadMaterial(const std::string&);
	Clb184::CTexture& getTexture();
private:
	Clb184::CTexture m_MainTex;
	std::vector<Clb184::CTexture> m_ExtraTextures;
};
#endif