#include "nflua.h"
#include <string>
#include <fstream>
using std::string;
using std::ifstream;

string readFile(const string& filePath)
{
	ifstream ifs(filePath);
	string data;
	string line;
	while (std::getline(ifs, line))
	{
		data += line + "\n";
	}
	return data;
}


int main(int argc, char** argv)
{
	string scriptData = readFile("testcases.lua");
	NFLua::NFLuaHandler handler(scriptData);
	auto fun00 = handler.getFunc<void>("fun00");
	auto fun01 = handler.getFunc<lua_Number>("fun01");
	auto fun10 = handler.getFunc<void, lua_Number>("fun10");
	auto fun11 = handler.getFunc<lua_Number, lua_Number>("fun11");

	fun00();
	auto r01 = fun01();
	fun10(1);
	auto r11 = fun11(1);
		
	return 0;

}