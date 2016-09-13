//lua c api �ķ��b
#ifndef _DARK_CPP__LUA_CONTEXT__HPP_
#define _DARK_CPP__LUA_CONTEXT__HPP_

#include <lua/lua.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace dark
{
	namespace lua
	{
		typedef boost::shared_ptr<lua_State> l_spt;

		//lua ���\�� �h��
		class context_t
		{
		protected:
			l_spt _l;
			static void D(lua_State* l)
			{

			}
		public:
			//���� copy ���� ָ�
			context_t(bool openlibs = true)
			{
				lua_State* l = luaL_newstate();
				if(l)
				{
					_l = l_spt(l,lua_close);
					if(openlibs)
					{
						luaL_openlibs(l);
					}
				}
			}
			context_t(lua_State* l,bool close = true/*�Ƿ��������r ጷ�lua�h��*/)
			{
				if(close)
				{
					_l = l_spt(l,lua_close);
				}
				else
				{
					_l = l_spt(l,D);
				}
			}
			context_t(l_spt l)
			{
				_l = l;
			}
			context_t(const context_t& copy)
			{
				_l = copy._l;
			}
			inline context_t& operator=(const context_t& copy)
			{
				_l = copy._l;
				return *this;
			}
			inline operator bool()const
			{
				return _l;
			}
			inline operator l_spt()const
			{
				return _l;
			}
			inline operator lua_State*()const
			{
				return _l.get();
			}
			inline l_spt get_spt() const
			{
				return _l;
			}
			inline lua_State* get()const
			{
				return _l.get();
			}

			inline void reset()
			{
				_l.reset();
			}
		public:
			/******	�Uչ api	******/
			//�� ��ǰ �� ��ӡ���˜�ݔ��
			void dump_stdout()
			{
				int n = lua_gettop(_l.get());
				printf("ctx: top=%d, stack=[",n);
				for(int i=0;i<n;++i)
				{
					if(i)
					{
						printf(",");
					}
					dump_stdout(i+1);
				}
				puts("]");
			}
		protected:
			void dump_stdout(int n)
			{
				lua_State* l = _l.get();

				switch(lua_type(l,n))
				{
				case LUA_TNONE :
					std::cout<<"none";
					break;
				case LUA_TNIL:
					std::cout<<"nil";
					break;
				case LUA_TBOOLEAN:
					if(lua_toboolean(l,n))
					{
						std::cout<<"true";
					}
					else
					{
						std::cout<<"false";
					}
					break;
				case LUA_TLIGHTUSERDATA:
					std::cout<<"LUA_TLIGHTUSERDATA";
					break;

				case LUA_TNUMBER:
					lua_pushvalue(l,n);
					std::cout<<lua_tostring(l,-1);
					lua_pop(l,1);
					break;
				case LUA_TSTRING:
					std::cout<<"\""<<lua_tostring(l,n)<<"\"";
					break;
				case LUA_TTABLE:
					{
						std::cout<<"{";
						lua_pushnil(l);
						bool first = true;
						while(lua_next(l,n))
						{
							//�� ���Q �}�uһ�� ���⌢ index �D�Q�� string
							lua_pushvalue(l, -2);
							if(first)
							{
								first = false;
							}
							else
							{
								std::cout<<",";
							}
							bool is_str = lua_type(l,-1) == LUA_TSTRING;
							if(is_str)
							{
								std::cout<<"\"";
							}
							std::cout<<lua_tostring(l,-1);
							if(is_str)
							{
								std::cout<<"\"";
							}
							std::cout<<":";

							dump_stdout(-2);
						
							lua_pop(l, 2);
						}
						std::cout<<"}";
					}
					break;
				case LUA_TFUNCTION:
					std::cout<<"LUA_TFUNCTION";
					break;
				case LUA_TUSERDATA:
					std::cout<<"LUA_TUSERDATA";
					break;
				case LUA_TTHREAD:
					std::cout<<"LUA_TTHREAD";
					break;
				case LUA_NUMTAGS:
					std::cout<<"LUA_NUMTAGS";
					break;
				default:
					std::cout<<"unknow";
					break;
				}
			}


		public:
			/******	lua c api ����	******/
	
			/***	�� ����	***/
			//����
			inline void pop(int n=1)
			{
				lua_pop(_l.get(),n);
			}

			//�뗣
			inline void push_boolean(int b)
			{
				lua_pushboolean(_l.get(),b);
			}
			inline void push_boolean(bool b)
			{
				if(b)
				{
					lua_pushboolean(_l.get(),1);
				}
				else
				{
					lua_pushboolean(_l.get(),0);
				}
				
			}
			inline void push(lua_CFunction fn, int n)
			{
				lua_pushcclosure(_l.get(),fn,n);
			}
			inline void push(lua_CFunction fn)
			{ 
				lua_pushcfunction(_l.get(),fn);
			}
			inline void push_globaltable()
			{
				lua_pushglobaltable(_l.get());
			}
			inline void push_integer(lua_Integer n)
			{ 
				lua_pushinteger(_l.get(),n);
			}
			inline void push_lightuserdata (void *p)
			{
				lua_pushlightuserdata(_l.get(),p);
			}
			inline void push_nil()
			{
				lua_pushnil(_l.get());
			}
			inline void push_number(lua_Number n)
			{
				lua_pushnumber(_l.get(),n);
			}
			inline void push(const char *s)
			{
				lua_pushstring(_l.get(),s);
			}
			inline void push(const char *s,std::size_t len)
			{
				lua_pushlstring(_l.get(),s,len);
			}
			inline void push(const std::string& s)
			{
				lua_pushlstring(_l.get(),s.data(),s.size());
			}
			inline void push_thread()
			{
				lua_pushthread(_l.get());
				
			}
			//�}�u һ�ݔ������� �K�뗣
			inline void push_value(std::size_t idx)
			{
				lua_pushvalue(_l.get(),idx);
			}
			
			//�@ȡ ȫ��׃��/nil ����� ���� ��lua�̈́e
			inline int get_global(const char* name)
			{
				return lua_getglobal(_l.get(),name);
			}
			inline int get_global(const std::string& name)
			{
				return lua_getglobal(_l.get(),name.c_str());
			}
			//�� ��� �O�Þ� ȫ��׃�� �K����
			inline void set_global(const char* name)
			{
				lua_setglobal(_l.get(),name);
			}
			inline void set_global(const std::string& name)
			{
				lua_getglobal(_l.get(),name.c_str());
			}

			/******	���ؔ���	******/
			inline bool is_function(int idx)const
			{
				return lua_isfunction(_l.get(),idx);
			}
			inline bool is_table(int idx)const
			{
				return lua_istable(_l.get(),idx);
			}
			inline bool is_lightuserdata(int idx)const
			{
				return lua_islightuserdata(_l.get(),idx);
			}
			inline bool is_nil(int idx)const
			{
				return lua_isnil(_l.get(),idx);
			}
			inline bool is_boolean(int idx)const
			{
				return lua_isboolean(_l.get(),idx);
			}
			inline bool is_thread(int idx)const
			{
				return lua_isthread(_l.get(),idx);
			}
			inline bool is_none(int idx)const
			{
				return lua_isnone(_l.get(),idx);
			}


			/***	table ����	***/

			//���� һ�� table �K�뗣
			//narr �AӋ ��ʹ�õ� ���M��С �ą���ֵ
			//nrec �AӋ ��ʹ�õ� hash���С �ą���ֵ
			inline void createtable(int narr=0, int nrec=0)
			{
				lua_createtable(_l.get(),narr,nrec);
			}

			//�� ��� �O�õ� table �� key �K����
			inline void put_prop_string(int idx/*table λ��*/,const char* key)
			{
				lua_setfield(_l.get(),idx,key);
			}
			inline void set_prop_string(int idx/*table λ��*/,const std::string& key)
			{
				lua_setfield(_l.get(),idx,key.c_str());
			}
			//�� ��� �O�õ� table �� ָ��λ�� �K����
			inline void set_prop_index(int idx/*table λ��*/,int arr_idx)
			{
				lua_seti(_l.get(),idx,arr_idx);
			}
			//��table�� ����key ���ص� ��� �����ڷ��� nil
			inline void get_prop_string(int idx/*table λ��*/,const std::string& key)
			{
				lua_getfield(_l.get(),idx,key.c_str());
			}
			//��table�� ����arr_idx ���ص� ��� �����ڷ��� nil
			inline void get_prop_index(int idx/*table λ��*/,int arr_idx)
			{
				lua_geti(_l.get(),idx,arr_idx);
			}



			/***	����� ����	***/
			//���d �ַ��� ����lua�_�� �K������������ ����
			inline int l_loadstring(const char* s)
			{
				return luaL_loadstring(_l.get(),s);
			}
			inline int l_loadstring(const std::string& s)
			{
				return luaL_loadstring(_l.get(),s.c_str());
			}
			//���d �ļ� ����lua�_�� �K������������ ����
			inline int l_loadfile(const char* f)
			{
				return luaL_loadfile(_l.get(),f);
			}
			inline int l_loadfile(const std::string& f)
			{
				return luaL_loadfile(_l.get(),f.c_str());
			}

			/***	�{�� lua	***/
			/***	�� �뗣���� ֮�� ���� �������뗣 	***/
			inline void call(int nargs	/*��������*/ , int nresults/*����ֵ����*/)
			{
				lua_call(_l.get(),nargs,nresults);
			}
			inline bool pcall(int nargs	/*��������*/ , int nresults/*����ֵ����*/,int errfunc/*���е�һ�� �e�`���{����*/)
			{
				return lua_pcall(_l.get(),nargs,nresults,errfunc) == LUA_OK;
			}
		};


		//�� �����r ���� lua_pop ����
		class scoped_pop_t
			: boost::noncopyable
		{
		protected:
			lua_State* _l;
			int _n;
		public:
			scoped_pop_t(lua_State* l,int n)
			{
				_l = l;
				_n = n;
			}
			scoped_pop_t(context_t l,int n)
			{
				_l = l.get();
				_n = n;
			}
			~scoped_pop_t()
			{
				if(_n && _l)
				{
					lua_pop(_l,_n);
				}
			}

			scoped_pop_t& operator+=(int n)
			{
				_n += n;
				return *this;
			}
			void operator++()
			{
				++_n;
			}
			scoped_pop_t& operator-=(int n)
			{
				_n += n;
				return *this;
			}
			void operator--()
			{
				--_n;
			}

		};
	};
};
#endif	//_DARK_CPP__LUA_CONTEXT__HPP_