#include "mandelbrot.h"
#include <iostream>
#include "GL/glew.h"

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

mandelbrot::mandelbrot() : Scene()
{
	counter = 1;
	power = 2;
	oldPower = 2;
	scale = 2.5;
	oldPosx = 0.5;
	oldPosy = 0.5;
	offsetx = 0;
	offsety = 0;
	color = 100;
}

//mandelbrot::mandelbrot(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void mandelbrot::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/mandelbrotShader");
	AddTexture("../res/textures/pal.png", 1);
	//TextureDesine(840, 840);

	AddMaterial(texIDs,slots, 1);
	
	AddShape(Plane, -1, TRIANGLES);
	SetShapeShader(0, 1);
}

void mandelbrot::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
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
		s->SetUniformMat4f("MVP", glm::mat4(1));
		s->SetUniformMat4f("Normal", glm::mat4(1));
	}
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	s->SetUniform1f("offsetx", offsetx);
	s->SetUniform1f("offsety", offsety);
	if (oldPower < power)
	{
		oldPower += 0.1*scale;
		if (oldPower > power)
			oldPower = power;
	}
	if (oldPower > power)
	{
		oldPower -= 0.1*scale;
		if (oldPower < power)
			oldPower = power;
	}
	s->SetUniform1f("power", oldPower);
	s->SetUniform1f("scale", scale);
	s->SetUniform1f("colorUpdate", color);
	s->Unbind();
}

void mandelbrot::UpdatePosition(float xpos,  float ypos)
{
	
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y = 1 - ypos / viewport[3]; 
	offsetx += (x- oldPosx)*scale;
	offsety +=  (y - oldPosy)*scale;
	oldPosx = x;
	oldPosy = y;
}

void mandelbrot::release(float x, float y)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldPosx = x / viewport[2];
	oldPosy = 1-y / viewport[3];
}

void mandelbrot::UpdatePower(bool up)
{
	if (up)
		power++;
	else if (power > 2)
		power--;

}
void mandelbrot::updateScale(float factor)
{
	if(factor>0)
		scale -= 0.25*scale*factor;
	if (factor < 0)
		scale /= 0.75;
	std::cout << "scale: " << scale << std::endl;
}
void mandelbrot::WhenRotate()
{
	//std::cout << "x "<<x<<", y "<<y<<std::endl;
	
}

void mandelbrot::WhenTranslate()
{
}

void mandelbrot::updateColor(bool left)
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

void mandelbrot::Motion()
{
	if(isActive)
	{
	}
}

unsigned int mandelbrot::TextureDesine(int width, int height)
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

mandelbrot::~mandelbrot(void)
{

}
