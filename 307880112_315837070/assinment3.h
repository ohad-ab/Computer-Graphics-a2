#pragma once
#include "scene.h"

class assinment3 : public Scene
{
public:
	
	assinment3();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	
	unsigned int TextureDesine(int width, int height);
	~assinment3(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
	void UpdatePosition2( float xpos, float ypos);
	void moveCamera(int key);
	void updateScale(float factor);
	void release(float x, float y);
	void release2(float x, float y);
	void updateColor(bool left);
	glm::vec4 sphereIntersection(glm::vec3 p0, glm::vec3 v, glm::vec4 sphere);
	glm::vec3 findPoint(float x, float y);
private:
	struct SceneData* data;
	unsigned int counter;
	unsigned int tmp;
	float x, y, oldPosx, oldPosy, offsetx, offsety;
	float power;
	float oldPower;
	float scale;
	float zoom;
	float color;
	glm::vec3 pickedSphere;
	int pickedIndex;
};

