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
	auto fun20 = handler.getFunc<void, lua_Number, lua_Number>("fun20");
	auto fun21 = handler.getFunc<lua_Number, lua_Number, lua_Number>("fun21");
	auto fun30 = handler.getFunc<void, lua_Number, lua_Number, lua_Number>("fun30");

	fun00();
	auto r01 = fun01();
	fun10(1);
	auto r11 = fun11(1);
	fun20(1, 2);
	auto r21 = fun21(1, 2);
	fun30(1, 2, 3);
		
	return 0;

}