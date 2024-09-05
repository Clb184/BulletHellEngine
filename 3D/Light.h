#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED
#include "..\Graphics.h"

class Light {
public:/*
	Light() {}
	Light(const Light&) = default;
	Light& operator=(const Light&) = default;
	*/
	Light();

	void setup(glm::vec4 inCol, glm::vec3 inpos) {
		this->m_Color = inCol;
		this->m_Pos = inpos;
		//this->m_VShader.LoadFromFile(L"Shader/default3DVS.hlsl", "vs_4_0", "main");
		//this->m_PShader.LoadFromFile(L"Shader/default3DPS.hlsl", "ps_4_0", "main");
	}

	inline void chgCol(int i) {
		float fact = 0.0;
		//fact = (kbd.isKeyPressed(kbd.Add)) ? 0.05 : (kbd.isKeyPressed(kbd.Subtract) ? -0.05 : 0.0);
		switch (i) {
		case 0: this->m_Color.r += fact; break;
		case 1: this->m_Color.g += fact; break;
		case 2: this->m_Color.b += fact; break;
		case 3: this->m_Color.a += fact; break;
		}
	}

	void move(glm::vec3 pos) {
		this->m_Pos = pos;
	}
	/*
	void draw(Camera cam) {
		sf::Shader::bind(&m_Shader);
		vertLight[0] = m_Pos.x;
		vertLight[1] = m_Pos.y;
		vertLight[2] = m_Pos.z;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (GLfloat*)vertLight);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGL), (GLfloat*)vertLight + 3);
		glEnableVertexAttribArray(1);
		model = glm::mat4(1.0);

		model = glm::translate(model, m_Pos);

		m_Shader.setUniform("camMat", sf::Glsl::Mat4(glm::value_ptr(cam.camMat)));
		m_Shader.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));

		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(5.0);
		glDrawArrays(GL_POINTS, 0, GLsizei(1));
		glPointSize(1.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	*/

	glm::vec4 getColor();
	glm::vec3 getPos();

private:
	//GLfloat vertLight[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f ,0.0f };
	glm::mat4 m_Model = {};
	Clb184::CVertexShader m_VShader;
	Clb184::CPixelShader m_PShader;
	glm::vec4 m_Color;
	glm::vec3 m_Pos;
};
#endif