#pragma once
#include "scene.h"

class mandelbrot : public Scene
{
public:
	
	mandelbrot();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	
	unsigned int TextureDesine(int width, int height);
	~mandelbrot(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
	void UpdatePower(bool up);
	void updateScale(float factor);
	void release(float x, float y);
	void updateColor(bool left);
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y, oldPosx, oldPosy, offsetx, offsety;
	float power;
	float oldPower;
	float scale;
	float color;
};

