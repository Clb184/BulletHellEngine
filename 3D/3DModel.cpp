//#include "3DModel.h"
/*
void parseTex(std::map<std::string, sf::Image>& img, std::map<std::string, Texture>& tex)  {
	 for (std::map<std::string, sf::Image>::iterator it = img.begin(); it != img.end(); ++it) {
		 Texture ttex;
		 const int w = it->second.getSize().x;
		 const int h = it->second.getSize().y;
		 //ttex.pTexBytes = (char*)malloc(w*h*4);
		 ttex.pTexBytes = (char*)it->second.getPixelsPtr();
		 //memcpy(ttex.pTexBytes, it->second.getPixelsPtr(), w * h * 4);
		 ttex.w = w;
		 ttex.h = h;
		 tex.insert({ it->first, ttex });
	 }
}*/
