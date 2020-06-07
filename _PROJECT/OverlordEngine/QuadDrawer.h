#pragma once
#include "Singleton.h"

class QuadMaterial;

struct QuadRect
{
	int x;
	int y;
	int width;
	int height;

	bool operator!=(const QuadRect& other) const
	{
		return (x != other.x) || (y != other.y) || (width != other.width) || (height != other.height);
	}

	QuadRect& operator=(const QuadRect& other)
	{
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}
};

class QuadDrawer final: public Singleton<QuadDrawer>
{
public:
	virtual ~QuadDrawer();

	QuadDrawer(const QuadDrawer& other) = delete;
	QuadDrawer(QuadDrawer&& other) noexcept = delete;
	QuadDrawer& operator=(const QuadDrawer& other) = delete;
	QuadDrawer& operator=(QuadDrawer&& other) noexcept = delete;

	//Functions
	void Initialize(ID3D11Device* pDevice);

	//DRAW SRV ONLY (DEFAULT MATERIAL)
	void Draw(const GameContext& gameContext, ID3D11ShaderResourceView* pSRV) { Draw(gameContext, pSRV, { -1,-1,-1,-1 }, false); }
	void Draw(const GameContext& gameContext, ID3D11ShaderResourceView* pSRV, QuadRect dim, bool useRect = true);

	//DRAW CUSTOM MATERIAL
	void Draw(const GameContext& gameContext, Material* pMaterial) { Draw(gameContext, pMaterial, { -1,-1,-1,-1}, false); }
	void Draw(const GameContext& gameContext, Material* pMaterial, QuadRect dim, bool useRect = true);

private:
	friend class Singleton<QuadDrawer>;
	QuadDrawer();

	static const int CUSTOM_QUAD = 0; //QUAD 0
	static const int FULLSCREEN_QUAD = 1; //QUAD 1

	bool m_IsInitialized = false;

	ID3D11Buffer* m_pVertexBuffer = nullptr;

	QuadRect m_PreviousDimensions = {-1,-1,-1,-1};

	float m_ScreenWidth = 0.0f, m_ScreenHeight = 0.0f;
	float m_HalfScreenWidth = 0.0f, m_HalfScreenHeight = 0.0f;

	QuadMaterial* m_pDefaultMaterial = nullptr;
};

