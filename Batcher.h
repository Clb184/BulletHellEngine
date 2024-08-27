#ifndef BATCHER_INCLUDED
#define BATCHER_INCLUDED

#include "Graphics.h"
#include "LinkedArrayList.h"

class Batcher;

struct SpriteInstance {
	union {
		struct {
			float PosX;
			float PosY;
		};
		glm::vec2 Pos;
	};

	union {
		FRect TexCoord = { 0.0f, 0.0f, 1.0f, 1.0f };
		glm::vec4 UV;
	};
	union {
		struct {
			float Width;
			float Height;
		};
		glm::vec2 Size = glm::vec2(32.0f);
	};
	float Rot = 0.0f;
	union {
		struct {
			float ScaleX;
			float ScaleY;
		};
		glm::vec2 Scale = glm::vec2(1.0f);
	};

	int Center = 0b0101;
	bool Active = true;

	UINT Color = 0xffffffff;

#ifdef BIT64
	Batcher* pBatcher;
#else
	Batcher* pBatcher;
	void* pPad = nullptr;
#endif
	float Padding[8];
};

extern CMemoryPool<Node<SpriteInstance>> g_SIPool;

class Batcher {
public:
	//Constructor and destructor
	Batcher(int cnt);
	~Batcher();

	//Change instance of batcher
	SpriteInstance** GetInstance(int id);
	Node<SpriteInstance>* GetFreeInstance();
	bool DeleteInstance(size_t idx);
	void SetInstance(int id, SpriteInstance* ref);

	//Draw x instances and update batch if necessary
	void Draw(int cnt);


private:

	bool m_bOnUpdate;
	uint32_t m_MaxCount;
	UINT* m_pIndices;

	SpriteInstance** m_pInstances;
	CDoubleLinkedArrayList<SpriteInstance> m_lInstances;
	Clb184::CVertexBuffer m_VBuffer;
	Clb184::CIndexBuffer m_IBuffer;
};


#endif