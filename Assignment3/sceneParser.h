#include <fstream>
#include <iostream>
#include <errno.h>
#include <vector>
#include "glm/glm.hpp"

struct SceneData
{
	glm::vec4 eye;         //position + shine
	glm::vec4 ambient;
	glm::ivec4 sizes;
	std::vector<glm::vec4> objects;      //center coordinates + radius / normal + d
	std::vector<glm::vec4> lights;        //position + cos(angle)
	std::vector<glm::vec4> directions;     //direction +  is directional 0.0/1.0
	std::vector<glm::vec4> colors;
	std::vector<glm::vec4> intensities;		   //light intensity
};

class SceneParser
{
private:

	static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
	{
		unsigned int result = start;
		while (result < length)
		{
			result++;
			if (str[result] == token)
				break;
		}

		return result;
	}

	static inline unsigned int ParseIndexValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return atoi(token.substr(start, end - start).c_str()) - 1;
	}

	static inline float ParseFloatValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return atof(token.substr(start, end - start).c_str());
	}
	glm::vec4 parseVec4(const std::string& line);
public:
	SceneParser(const std::string& fileName, SceneData* data);
};