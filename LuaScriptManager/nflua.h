#pragma once


#include <string>
#include <stdexcept>
#include <functional>
#include <lua.hpp>
#include "nfmacro.h"

namespace NFLua
{
	NF_RUNTIMEEXCEPTION(ExceptionLuaInitializeFailed);
	NF_RUNTIMEEXCEPTION(ExceptionLuaFunctionNotValid);
	NF_RUNTIMEEXCEPTION(ExceptionLuaFuncionCallFailed);

	namespace _NFLuaInternal
	{
		using std::string;
		NF_INLINE void push_value(lua_State* luavm, lua_Integer i)
		{
			lua_pushinteger(luavm, i);
		}
		NF_INLINE void push_value(lua_State* luavm, lua_Number n)
		{
			lua_pushnumber(luavm, n);
		}
		NF_INLINE void push_value(lua_State* luavm, bool b)
		{
			lua_pushboolean(luavm, b);
		}
		NF_INLINE void push_value(lua_State* luavm, const string& s)
		{
			lua_pushstring(luavm, s.c_str());
		}

		template <typename T>
		struct value_extractor
		{
		};

		template <>
		struct value_extractor<lua_Integer>
		{
			static lua_Integer get(lua_State* vm)
			{
				lua_Integer val = lua_tointeger(vm, -1);
				lua_pop(vm, 1);
				return val;
			}
		};

		template <>
		struct value_extractor<lua_Number>
		{
			static lua_Number get(lua_State* vm)
			{
				lua_Number val = lua_tonumber(vm, -1);
				lua_pop(vm, 1);
				return val;
			}
		};

		template <>
		struct value_extractor<bool>
		{
			static bool get(lua_State* vm)
			{
				bool val = lua_toboolean(vm, -1);
				lua_pop(vm, 1);
				return val;
			}
		};

		template <>
		struct value_extractor<std::string>
		{
			static std::string get(lua_State* vm)
			{
				std::string val = lua_tostring(vm, -1);
				lua_pop(vm, 1);
				return val;
			}
		};

		template <class _Head, class... _Tail>
		NF_INLINE void push_multiple_value(lua_State* vm, const _Head& hd, const _Tail&... tl)
		{
			push_value(vm, hd);
			if constexpr (sizeof...(_Tail) != 0)
				push_multiple_value(vm, tl...);
		}



/*
		template <class _Head>
		NF_INLINE void push_multiple_value(lua_State* vm, const _Head& hd)
		{
			push_value(vm, hd);
		}*/

		NF_INLINE void push_multiple_value(lua_State* vm)
		{
			// Do nothing
		}

	}

	using std::string;

	template <class _Ret> class NFLuaFunction;

	class NFLuaFunctionBase
	{
	private:
		lua_State* vm_;
		int funcIndex_;


		template <class _Ret> friend class NFLuaFunction;

		void call(int args, int results)
		{
			int status = lua_pcall(vm_, args, results, 0);
			if (status /* != 0 */)
			{
				// Case: Call FAILED
				string error = lua_tostring(vm_, -1);
				lua_pop(vm_, 1);
				throw ExceptionLuaFuncionCallFailed(error);
			}
		}

	public:
		NFLuaFunctionBase(lua_State* vm, const string& func)
			: vm_(vm)
		{
			lua_getfield(vm_, LUA_GLOBALSINDEX, func.c_str());
			// Check Function
			if (!lua_isfunction(vm_, -1))
			{
				lua_pop(vm_, 1);
				throw ExceptionLuaFunctionNotValid(func);
			}
			funcIndex_ = luaL_ref(vm_, LUA_REGISTRYINDEX);
		}

		NFLuaFunctionBase(const NFLuaFunctionBase& func)
			: vm_(func.vm_)
		{
			// Copy from registry ref
			lua_rawgeti(vm_, LUA_REGISTRYINDEX, func.funcIndex_);
			funcIndex_ = luaL_ref(vm_, LUA_REGISTRYINDEX);
		}
		~NFLuaFunctionBase()
		{
			luaL_unref(vm_, LUA_REGISTRYINDEX, funcIndex_);
		}

		NFLuaFunctionBase& operator=(const NFLuaFunctionBase& func)
		{
			if (this != &func)
			{
				vm_ = func.vm_;
				lua_rawgeti(vm_, LUA_REGISTRYINDEX, func.funcIndex_);
				funcIndex_ = luaL_ref(vm_, LUA_REGISTRYINDEX);
			}
			return *this;
		}
	};

	template <class _Ret>
	class NFLuaFunction : public NFLuaFunctionBase
	{
	public:
		NFLuaFunction(lua_State* vm, const string& func)
			:NFLuaFunctionBase(vm, func)
		{}

		template<class ..._Args>
		_Ret operator()(const _Args&... args)
		{
			lua_rawgeti(vm_, LUA_REGISTRYINDEX, funcIndex_);
			_NFLuaInternal::push_multiple_value(vm_, args...);
			constexpr auto argNum = sizeof...(_Args);
			call(argNum, 1);
			return _NFLuaInternal::value_extractor<_Ret>::get(vm_);
		}
	};
	template <>
	class NFLuaFunction<void> : public NFLuaFunctionBase
	{
	public:
		NFLuaFunction(lua_State* vm, const string& func)
			:NFLuaFunctionBase(vm, func)
		{}

		template<class ..._Args>
		void operator()(const _Args&... args)
		{
			lua_rawgeti(vm_, LUA_REGISTRYINDEX, funcIndex_);
			_NFLuaInternal::push_multiple_value(vm_, args...);
			call(sizeof...(_Args), 0);
		}
	};
/*
	template <>
	class NFLuaFunction<void> : public NFLuaFunctionBase
	{
	public:
		NFLuaFunction(lua_State* vm, const string& func)
			:NFLuaFunctionBase(vm, func)
		{}

		void operator()()
		{
			lua_rawgeti(vm_, LUA_REGISTRYINDEX, funcIndex_);
			call(0, 0);
		}
	};*/

	class NFLuaHandler
	{
	private:
		lua_State* vm;
	public:
		NFLuaHandler(const string& code)
			:vm(::luaL_newstate())
		{
			if (!vm) throw ExceptionLuaInitializeFailed();
			::luaopen_base(vm);
			::luaL_loadstring(vm, code.c_str());
			::lua_pcall(vm, 0, 0, 0);
		}
		template<class _Ret, class... _Args>
		std::function<_Ret(_Args...)> getFunc(const string& func)
		{
			return NFLuaFunction<_Ret>(vm, func);
		}

/*
		template<class _Ret = void>
		std::function<_Ret(void)> getFunc(const string& func)
		{
			return NFLuaFunction<_Ret>(vm, func);
		}*/
	};

}