#pragma once
#ifdef MYTEST
#include<glm/glm.hpp>
#include <yaml-cpp/yaml.h>
using namespace YAML;
struct __v
{
	glm::vec3 p;
	glm::vec2 uv;
	glm::vec4 color;
};
void yamlTest()
{

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "m_vert_shader";
	out << YAML::Value << "../../vert";
	out << YAML::Key << "m_frag_shader";
	out << YAML::Value << "../../frag";


	out << YAML::Key << "textureParams";
	//out << YAML::Value << YAML::BeginSeq << "Sasha" << "Malia" << YAML::EndSeq;
	//out << YAML::Value << "../../texture1";
	out << YAML::BeginMap;
	out << YAML::Key << "_BaseMap";
	out << YAML::Value << "../../texture1";
	out << YAML::EndMap;

	out << YAML::Key << "vec1Params";
	//out << YAML::Value << YAML::BeginSeq << "Sasha" << "Malia" << YAML::EndSeq;
	//out << YAML::Value << "../../texture1";
	out << YAML::BeginMap;
	out << YAML::Key << "_Test";
	out << YAML::Value << "2";
	out << YAML::EndMap;
	out << YAML::Key << "vec4Params";
	out << YAML::BeginMap;
	out << YAML::Key << "_Color";
	out << YAML::Value << YAML::BeginSeq << 1 << 1 << 1 << 1 << YAML::EndSeq;
	out << YAML::EndMap;
	out << YAML::EndMap;
	/*ofstream myfile;
	myfile.open("yamltest.yaml", std::ios::trunc | std::ios::binary);
	std::string s(out.c_str());
	if (myfile.is_open())
	{
		auto size = out.size();
		myfile.write(out.c_str(), out.size());
		myfile.close();
	}

	cout << out.c_str() << endl;*/
}
class TestB
{
public:
	int b = 4;
	TestB() = default;
	TestB(const TestB& obj)
	{
		std::cout << "copy test b" << std::endl;
	}
	~TestB()
	{
		b = -1;
		std::cout << "dispose test b" << std::endl;
	}
};
class TestA
{
public:
	TestB* a;
	TestA(const int x1)
	{
		x = x1;
	}
	void setData(TestB* b)
	{
		a = b;
		std::cout << a << std::endl;
	}
	 int x = 4;
	void setDatax( int& x1)
	{
		x = x1;
	}
};

#endif

