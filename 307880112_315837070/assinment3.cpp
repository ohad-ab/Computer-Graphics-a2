#include "assinment3.h"
#include <iostream>
#include "GL/glew.h"
#include "sceneParser.h"

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

assinment3::assinment3() : Scene()
{
	data = new SceneData();
	counter = 1;
	power = 2;
	oldPower = 2;
	scale = 2.5;
	zoom = 0;
	oldPosx = 1.3;
	oldPosy = 1.5;
	offsetx = 0;
	offsety = 0;
	color = 100;
	pickedSphere = glm::vec3(0, 0, 0);
	pickedIndex = -1;
}

//mandelbrot::mandelbrot(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void assinment3::Init()
{	
	SceneParser("../res/scenes/scene.txt", data);
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	AddTexture("../res/textures/pal.png", 1);
	//TextureDesine(840, 840);

	AddMaterial(texIDs,slots, 1);
	
	AddShape(Plane, -1, TRIANGLES);
	SetShapeShader(0, 1);
}

void assinment3::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx != 1)
	{
		s->SetUniformMat4f("MVP", MVP);
		s->SetUniformMat4f("Normal", Model);
	}
	else
	{
		s->SetUniformMat4f("MVP", MVP);
		s->SetUniformMat4f("Normal", Model);
	}
	const glm::vec4 eye(data->eye.x+offsetx, data->eye.y+offsety, data->eye.z+ zoom, data->eye.w);
	const glm::vec4 ambient(data->ambient.x, data->ambient.y, data->ambient.z, data->ambient.w);
	const glm::vec4 sizes((float)data->sizes.x, (float)data->sizes.y, (float)data->sizes.z, (float)data->sizes.w);
	s->SetUniform4fv("ambient", &ambient, 1);
	s->SetUniform4fv("eye", &eye, 1);
	s->SetUniform4i("sizes", sizes.x, sizes.y, sizes.z, sizes.w);
	s->SetUniform4fv("objects", &(data->objects[0]), 20);
	s->SetUniform4fv("lightPosition", &(data->lights[0]), 10);
	s->SetUniform4fv("lightsDirection", &(data->directions[0]), 10);
	s->SetUniform4fv("objColors", &(data->colors[0]), 20);
	s->SetUniform4fv("lightsIntensity", &(data->intensities[0]), 10);
	s->SetUniform1f("dis", data->eye.z);
	s->Unbind();
}

void assinment3::UpdatePosition(float xpos, float ypos)
{

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y = 1 - ypos / viewport[3];
	glm::vec3 eye(data->eye.x + offsetx, data->eye.y + offsety, data->eye.z + zoom);
	glm::vec3 p = findPoint(oldPosx, oldPosy);
	glm::vec3 v = p - eye;
	glm::vec3 nv = glm::normalize(v);
	float dist = 1000.0;
	glm::vec4 sphereInter;
	glm::vec3 spherexyz;
	if (pickedSphere == glm::vec3(0, 0, 0))
	{
		for (int i = 0; i < data->sizes.x; i++)
		{
			if (data->objects[i].w > 0)
			{
				sphereInter = sphereIntersection(eye, nv, data->objects[i]);
				spherexyz = glm::vec3(sphereInter.x, sphereInter.y, sphereInter.z);
				if (sphereInter != glm::vec4(0, 0, 0, 0) && glm::distance(spherexyz, eye) < dist)
				{
					dist = glm::distance(spherexyz, eye);
					pickedIndex = i;
					pickedSphere = spherexyz;

				}
			}

		}
	}

	if(pickedSphere != glm::vec3(0,0,0))
	{
		glm::vec3 newp = findPoint(x, y);

		float tx = (glm::length(pickedSphere - eye)*(newp.x-p.x)) / glm::length(p - eye);
		float ty = (glm::length(pickedSphere - eye)*(newp.y - p.y)) / glm::length(p - eye);
			
		data->objects[pickedIndex].x += tx;
		data->objects[pickedIndex].y += ty;

	}
	
	oldPosx = x;
	oldPosy = y;
}

glm::vec3 assinment3::findPoint(float x, float y)
{
	glm::vec3 eye(data->eye.x + offsetx, data->eye.y + offsety, data->eye.z + zoom);
	glm::vec3 vTo = glm::vec3(0, 0, -1);
	glm::vec3 vUp = glm::vec3(0, -1, 0);
	glm::vec3 vRight = glm::normalize(glm::cross(vUp, vTo));
	glm::vec3 vUp1 = glm::normalize(glm::cross(vRight, vTo));
	glm::vec3 pc = eye + data->eye.z * vTo;
	glm::vec3 p = pc + 2 * (x - 0.5f)*vRight + 2 * (y - 0.5f)*vUp1;
	
	return p;
}

glm::vec4 assinment3::sphereIntersection(glm::vec3 p0, glm::vec3 v, glm::vec4 sphere)
{
	glm::vec3 O = glm::vec3(sphere.x, sphere.y, sphere.z);
	float radius = sphere.w;
	float radius2 = pow(radius, 2);
	glm::vec3 L = O - p0;
	float tm = glm::dot(L, v);
	float d = pow(glm::length(L), 2) - pow(tm, 2);
	if (d > radius2)
		return glm::vec4(0, 0, 0, 0);
	float th = sqrt(radius2 - d);
	float t = tm - th;
	if (t < 0)
		t = tm + th;
	if (t > 0)
		return glm::vec4((p0 + t * v), 1);
	else
		return glm::vec4(0, 0, 0, 0);
}

void assinment3::release(float x, float y)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldPosx = x / viewport[2];
	oldPosy = 1-y / viewport[3];
	pickedSphere = glm::vec3(0, 0, 0);
	pickedIndex = -1;
}

void assinment3::UpdatePosition2(float xpos, float ypos)
{

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y = 1 - ypos / viewport[3];
	offsetx -= (x - oldPosx)*scale;
	offsety -= (y - oldPosy)*scale;
	oldPosx = x;
	oldPosy = y;
}

void assinment3::release2(float x, float y)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldPosx = x / viewport[2];
	oldPosy = 1 - y / viewport[3];
}

void assinment3::moveCamera(int key)
{
	/*if (key == 1)
		offsety += 0.5;
	if (key == 2)
		offsety -= 0.5;
	if (key == 3)
		offsetx -= 0.5;
	if (key == 4)
		offsetx +=0.5;*/

}
void assinment3::updateScale(float factor)
{
	if(factor>0)
		zoom -= 0.5*factor;
	if (factor < 0)
		zoom += 0.5;
	std::cout << "scale: " << scale << std::endl;
}
void assinment3::WhenRotate()
{
	//std::cout << "x "<<x<<", y "<<y<<std::endl;
	
}

void assinment3::WhenTranslate()
{
}

void assinment3::updateColor(bool left)
{
	if (left) {
		if (color > 2) {
			color = ceil(color / 2);
			//printf("%f\n", color);
		}
	}
	else
		color = color * 2;
}

void assinment3::Motion()
{
	if(isActive)
	{
	}
}

unsigned int assinment3::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	/*for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] = (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}*/

	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

assinment3::~assinment3(void)
{

}
