/*shajf*/
#ifndef CH_LUA_H
#define CH_LUA_H
#include <string.h>
#include <stdlib.h>
#include <luajit-2.0/luajit.h>
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lualib.h>
#include <luajit-2.0/lauxlib.h>
#include "ch_constants.h"

#ifndef LUA_OK
#define LUA_OK 0
#endif

/*wrap type */

typedef enum {
	L_NUMBER_TYPE,
	L_STRING_TYPE,
	L_TABLE_TYPE,
	L_FUNCTION_TYPE,/*c function or lua function*/
	L_THREAD_TYPE,
	L_USERDATA_TYPE,
	L_NIL_TYPE,
	L_CPOINTER_TYPE,
	L_CFUNCTION_TYPE,
	L_BOOLEAN_TYPE,
	L_NONE_TYPE,
	L_NONEORNIL_TYPE,
	L_TYPE_N
}lua_type_e;

/****************wrap lua type check functions*****************/
static inline int
ch_check_type(lua_State *L,int index,int type)
{
	switch(type)
	{
		case L_NUMBER_TYPE:
		return lua_isnumber(L,index);

		case L_STRING_TYPE:
		return lua_isstring(L,index);

		case L_TABLE_TYPE:
		return lua_istable(L,index);

		case L_FUNCTION_TYPE:
		return lua_isfunction(L,index);

		case L_THREAD_TYPE:
		return lua_isthread(L,index);

		case L_USERDATA_TYPE:
		return lua_isuserdata(L,index);

		case L_NIL_TYPE:
		return lua_isnil(L,index);

		case L_CPOINTER_TYPE:
		return lua_islightuserdata(L,index);

		case L_CFUNCTION_TYPE:
		return lua_iscfunction(L,index);

		case L_BOOLEAN_TYPE:
		return lua_isboolean(L,index);
		
		case L_NONEORNIL_TYPE:
		return lua_isnoneornil(L,index);
		
		default:
		return 1;
	}//switch	
}

/*wrap function for stack*/
#define CH_LUA_STACK_TOP_INDEX	-1

#define ch_check_type_ontop(L,type) ch_check_type(L,CH_LUA_STACK_TOP_INDEX,type)

/*brief:Ensures that there are at least extra free stack slots 
 * in the stack. It returns false if it cannot fulfill the request,
 * because it would cause the stack to be larger than a fixed maximum
 * size (typically at least a few thousand elements) or because it cannot 
 * allocate memory for the new stack size. This function never shrinks 
 * the stack; 
 * if the stack is already larger than the new size, it is left unchanged
 * */
static inline int ch_lua_stack_check(lua_State *L,int need_size)
{
	if(need_size<0)
	{
		need_size=0;
	}

	if(lua_checkstack(L,need_size)==0)
	{
		return -1;
	}

	return 0;
}

static inline void ch_lua_stackpop(lua_State *L,int n)
{
	lua_pop(L,n);
}

static inline void ch_lua_stacksettop(lua_State *L,int new_index)
{
	lua_settop(L,new_index);
}

/*brief:clear stack*/
static inline void ch_lua_stack_clear(lua_State *L)
{
	lua_settop(L,0);
}

/*brief:Returns the index of the top element in the stack.
 * Because indices start at 1, this result is equal to 
 * the number of elements in the stack (and so 0 means an empty stack)*/
static inline int ch_lua_stack_get_elem_num(lua_State *L)
{
	return lua_gettop(L);
}

static inline int ch_lua_stack_push_true(lua_State *L)
{
	if(ch_lua_stack_check(L,sizeof(int))!=0)

		return -1;

	lua_pushboolean(L,1);
	return 0;
}

static inline int ch_lua_stack_push_false(lua_State *L)
{

	if(ch_lua_stack_check(L,sizeof(int))!=0)

		return -1;

	lua_pushboolean(L,0);
	return 0;
}

/*brief:Pushes a C function onto the stack. This function receives 
 * a pointer to a C function and pushes onto the stack a Lua value 
 * of type function that, when called, invokes the corresponding C 
 * function. Any function to be registered in Lua must follow the
 * correct protocol to receive its parameters and return its results 
 * */
static inline int ch_lua_stack_push_cfun(lua_State *L,lua_CFunction c_fun)
{
	
	if(ch_lua_stack_check(L,sizeof(void*))!=0)

		return -1;

	lua_pushcfunction(L,c_fun);
	return 0;
}

/*brief:Pushes a new C closure onto the stack. 
 * When a C function is created, it is possible 
 * to associate some values with it, thus creating a 
 * C closure these values are then accessible to the 
 * function whenever it is called. To associate values 
 * with a C function, first these values should be pushed 
 * onto the stack (when there are multiple values, the first 
 * value is pushed first). Then lua_pushcclosure is called to 
 * create and push the C function onto the stack, with the argument n
 * telling how many values should be associated with the function.
 * lua_pushcclosure also pops these values from the stack. 
 * */

static inline int ch_lua_stack_push_ccfun(lua_State *L,lua_CFunction c_fun,int num,int size_all)
{
	if(size_all<=0)
	{
		size_all=sizeof(void*)*16;
	}

	if(ch_lua_stack_check(L,size_all)!=0)

		return -1;

	lua_pushcclosure(L,c_fun,num);
	return 0;
}

/*brief:Pushes the string pointed to by s with size len onto the stack.
 * Lua makes (or reuses) an internal copy of the given string, 
 * so the memory at s can be freed or reused immediately after 
 * the function returns. The string can contain any binary data, 
 * including embedded zeros*/

static inline int ch_lua_stack_push_string(lua_State *L,const char *s,size_t len)
{
	if(s==NULL||len<=0)
	{
		return 0;
	}

	if(ch_lua_stack_check(L,len)!=0)

		return -1;

	lua_pushlstring(L,s,len);
	return 0;
}

/*brief:Push a nil value onto stack*/
#define ch_lua_stack_push_nil		lua_pushnil

/*brief:push double value onto stack*/

static inline int ch_lua_stack_push_double(lua_State *L,double n)
{

	if(ch_lua_stack_check(L,128)!=0)

		return -1;

	lua_pushnumber(L,n);
	return 0;
}


/*brief:Pushes the thread represented by L onto the stack.
 * Returns 1 if this thread is the main thread of its state*/
#define ch_lua_stack_push_thread	lua_pushthread 

/*brief:Pushes a copy of the element at the given valid index 
 * onto the stack*/

static inline int ch_lua_stack_push_value(lua_State *L,int index)
{

	if(ch_lua_stack_check(L,lua_objlen(L,index))!=0)
		return -1;

	lua_pushvalue(L,index);

	return 0;
}

/*brief:Pushes onto the stack a formatted string and returns 
 * a pointer to this string. It is similar to the C function 
 * sprintf, but has some important differences: 
 *You do not have to allocate space for the result: the result is a Lua
 *string and Lua takes care of memory allocation (and deallocation,
 *through garbage collection). 
 * The conversion specifiers are quite restricted. There are no flags, 
 * widths, or precisions. The conversion specifiers can only be 
 * '%%' (inserts a '%' in the string), '%s' (inserts a zero-terminated 
 * string, with no size restrictions), '%f' (inserts a lua_Number),
 * '%p' (inserts a pointer as a hexadecimal numeral), 
 * '%d' (inserts an int), and '%c' (inserts an int as a byte).
 * */
static inline const char *ch_lua_stack_push_fstring(lua_State *L,const char *fmt,...)
{
	va_list va_p;
	const char* res_p;
	va_start(va_p,fmt);
	res_p=lua_pushvfstring(L,fmt,va_p);
	va_end(va_p);
	return res_p;
}

static inline int ch_lua_stack_push_int(lua_State *L,int n)
{

	if(ch_lua_stack_check(L,64)!=0)

		return -1;

	lua_pushinteger(L,n);

	return 0;
}

/*brief:Pushes a light userdata onto the stack. 
 * Userdata represent C values in Lua. A light userdata
 * represents a pointer, a void*. It is a value (like a number):
 * you do not create it, it has no individual metatable, and it is
 * not collected (as it was never created). A light userdata is equal
 * to "any" light userdata with the same C address
 * */

static inline int ch_lua_stack_push_cpointer(lua_State *L,void *p)
{
	
	if(ch_lua_stack_check(L,sizeof(void*))!=0)

		return -1;

	lua_pushlightuserdata(L,p);

	return 0;
}

static inline int ch_lua_stack_pushnil(lua_State *L)
{
	lua_pushnil(L);
	return 0;
}

/*********************get fun**********************************/
static inline int ch_lua_stack_get_boolean(lua_State *L,int *val,int index)
{
	if(!ch_check_type(L,index,L_BOOLEAN_TYPE))
	{
		*val=0;
		return -1;
	}

	*val=lua_toboolean(L,index);
	return 0;
}

static inline int ch_lua_stack_get_int(lua_State *L,int *val,int index)
{
	if(!ch_check_type(L,index,L_NUMBER_TYPE))
	{
		*val=0;
		return -1;
	}

	*val=lua_tointeger(L,index);
	return 0;
}

static inline int ch_lua_stack_get_number(lua_State *L,double *val,int index)
{
	if(!ch_check_type(L,index,L_NUMBER_TYPE))
	{
		*val=0;
		return -1;
	}

	*val=lua_tonumber(L,index);
}

static inline int ch_lua_stack_get_string(lua_State *L,char ** val,size_t *len,int index)
{
	if(!ch_check_type(L,index,L_STRING_TYPE))
	{
		*val=NULL;
		*len=0;
		return -1;
	}

	*val=(char*)lua_tolstring(L,index,len);
	return 0;
}

static inline int ch_lua_stack_get_tostring(lua_State *L,char ** val,size_t *len,int index)
{

	*val=(char*)lua_tolstring(L,index,len);

	return 0;
}

static inline void* ch_lua_stack_get_cpointer(lua_State *L,int index)
{
	return (void*)lua_topointer(L,index);
}

static inline lua_CFunction ch_lua_stack_get_cfunction(lua_State *L,int index)
{
	return lua_tocfunction(L,index);
}

/*wrap function for table*/

/*brief:Creates a new empty table and pushes it onto the stack*/
static inline int ch_lua_create_table(lua_State *L)
{
	lua_newtable(L);
	if(ch_check_type_ontop(L,L_TABLE_TYPE))	
		return 0;
	
	return -1;
}

/*brief:Creates a new empty table and pushes it onto the stack.
 * Parameter narr is a hint for how many elements the table will
 * have as a sequence; parameter nrec is a hint for how many other
 * elements the table will have. Lua may use these hints to 
 * preallocate memory for the new table. This pre-allocation is 
 * useful for performance when you know in advance how many elements 
 * the table will have*/

static inline int ch_lua_create_fixtable(lua_State *L,int narr,int nrec)
{
	if(narr<=0||nrec<=0)
	{
		return ch_lua_create_table(L);
	}

	lua_createtable(L,narr,nrec);
	
	if(ch_check_type_ontop(L,L_TABLE_TYPE))	
		return 0;
	
	return -1;
	
}

static inline int check_and_put_key(lua_State *L,const char *key)
{

	if(!ch_check_type_ontop(L,L_TABLE_TYPE))
		return -1;
	/*push key*/
	if(ch_lua_stack_push_string(L,
				      key,
				      strlen(key))!=0)
	{
		return -1;
	}

	return 0;
}

static inline int check_and_put_key2(lua_State *L,const char *key,size_t len)
{

	if(!ch_check_type_ontop(L,L_TABLE_TYPE))
		return -1;
	/*push key*/
	if(ch_lua_stack_push_string(L,key,len)!=0)
	{
		return -1;
	}

	return 0;
}

static inline int ch_lua_table_put_int(lua_State *L,const char *key,long val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_int(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int
ch_lua_table_put_number(lua_State *L,const char *key,double val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_double(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_put_string(lua_State *L,const char *key,const char *val)
{
    if(key==NULL||strlen(key)==0)
        return -1;
    val = val==NULL||strlen(val)==0?"--":val;

	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_string(L,val,strlen(val))!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_put_lstring(lua_State *L,const char *key,const char *val,size_t val_len)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_string(L,val,val_len)!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_put_cfunction(lua_State *L,const char *key,lua_CFunction val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cfun(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_put_table(lua_State *L,const char *key)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	if(!ch_check_type(L,
			   CH_LUA_STACK_TOP_INDEX-1,
			   L_TABLE_TYPE))
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_put_cpointer(lua_State *L,const char *key,void *val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cpointer(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_settable(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int
ch_lua_table_set_int(lua_State *L,const char *key,int t_index,long val)
{
	if(!ch_check_type(L,t_index,L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_int(L,val)!=0)
		return -1;

	lua_setfield(L,t_index,key);

	return 0;
}

static inline int ch_lua_table_set_number(lua_State *L,const char *key,int t_index,double val)
{

	if(!ch_check_type(L,
			   t_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_double(L,val)!=0)
		return -1;

	lua_setfield(L,t_index,key);

	return 0;
}

static inline int ch_lua_table_set_string(lua_State *L,
		       const char *key,
		       int t_index,
		       const char *val,
               size_t val_len)
{
	if(!ch_check_type(L,
			   t_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_string(L,
				      val,
				      val_len)!=0)
		return -1;

	lua_setfield(L,t_index,key);
	
	return 0;
}

static inline int ch_lua_table_set_cfunction(lua_State *L,
		       const char *key,
		       int t_index,
		       lua_CFunction val)
{
	
	if(!ch_check_type(L,
			   t_index,
			   L_TABLE_TYPE))
		
		return -1;
	/*push val*/
	if(ch_lua_stack_push_cfun(L,val)!=0)
		return -1;

	lua_setfield(L,t_index,key);
	return 0;
}

static inline int ch_lua_table_set_cpointer(lua_State *L,
		       const char *key,
		       int t_index,
		       void *val)
{
	if(!ch_check_type(L,
			   t_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cpointer(L,val)!=0)
		return -1;

	lua_setfield(L,t_index,key);
	
	return 0;
}

static inline int ch_lua_table_set_nil(lua_State *L,
		       const char *key,
		       int t_index)
{

	if(!ch_check_type(L,
			   t_index,
			   L_TABLE_TYPE))
		return -1;

	ch_lua_stack_pushnil(L);


	lua_setfield(L,t_index,key);
	return 0;
}
/*********************quick access table************************/

static inline int ch_lua_table_rput_int(lua_State *L,
		       const char *key,
		       long val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_int(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_rput_number(lua_State *L,
		       const char *key,
		       double val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_double(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_rput_string(lua_State *L,
		       const char *key,
		       const char *val,
               size_t val_len)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_string(L,
				      val,
				      val_len)!=0)
		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_rput_cfunction(lua_State *L,
		       const char *key,
		       lua_CFunction val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cfun(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_rput_table(lua_State *L,
		       const char *key)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	if(!ch_check_type(L,
			   CH_LUA_STACK_TOP_INDEX-1,
			   L_TABLE_TYPE))

		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_table_rput_cpointer(lua_State *L,
		       const char *key,
		       void *val)
{
	if(check_and_put_key(L,key)!=0)
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cpointer(L,val)!=0)
		return -1;

	/*add key and val to table*/
	lua_rawset(L,CH_LUA_STACK_TOP_INDEX-2);
	return 0;
}

static inline int ch_lua_array_set_int(lua_State *L,
		       int a_index,
		       int e_index,
		       long val)
{
	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_int(L,val)!=0)
		return -1;

	lua_rawseti(L,a_index,e_index);

	return 0;
}

static inline int ch_lua_array_set_number(lua_State *L,
		       int a_index,
		       int e_index,
		       double val)
{

	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_double(L,val)!=0)
		return -1;

	lua_rawseti(L,a_index,e_index);

	return 0;
}

static inline int ch_lua_array_set_string(lua_State *L,
		       int a_index,
		       int e_index,
		       const char *val,
               size_t val_len)
{
	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_string(L,
				      val,
				      val_len)!=0)
		return -1;

	lua_rawseti(L,a_index,e_index);
	
	return 0;
}

static inline int ch_lua_array_set_cfunction(lua_State *L,
		       int a_index,
		       int e_index,
		       lua_CFunction val)
{
	
	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		
		return -1;
	/*push val*/
	if(ch_lua_stack_push_cfun(L,val)!=0)
		return -1;

	lua_rawseti(L,a_index,e_index);
	return 0;
}

static inline int ch_lua_array_set_cpointer(lua_State *L,
		       int a_index,
		       int e_index,
		       void *val)
{
	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		
		return -1;
	
	/*push val*/
	if(ch_lua_stack_push_cpointer(L,val)!=0)
		return -1;

	lua_rawseti(L,a_index,e_index);
	
	return 0;
}

static inline int ch_lua_array_set_nil(lua_State *L,
		       int a_index,
		       int e_index)
{

	if(!ch_check_type(L,
			   a_index,
			   L_TABLE_TYPE))
		return -1;

	ch_lua_stack_pushnil(L);


	lua_rawseti(L,a_index,e_index);
	return 0;
}

static inline int ch_lua_table_set_metatable(lua_State *L,int t_index)
{
	if(!ch_check_type(L,
			    CH_LUA_STACK_TOP_INDEX,
			    L_TABLE_TYPE)||\
	   !ch_check_type(L,
			    t_index,
			    L_TABLE_TYPE))
	{
		return -1;
	}

	lua_setmetatable(L,t_index);
	return 0;
}

/*************************get table's element*************************/
static inline int ch_lua_table_get(lua_State *L,
		   int t_index,
		   const char *key)
{
	if(!ch_check_type(L,t_index,L_TABLE_TYPE))
	{
		return -1;
	}

	if(ch_lua_stack_push_string(L,
			              key,
				      strlen(key))!=0)
	{
		return -1;
	}

	lua_gettable(L,t_index);
	return 0;
}

static inline int ch_lua_table_rget(lua_State *L,
		   int t_index,
		   const char *key)
{
	
	if(!ch_check_type(L,t_index,L_TABLE_TYPE))
	{
		return -1;
	}

	if(ch_lua_stack_push_string(L,
			              key,
				      strlen(key))!=0)
	{
		return -1;
	}

	lua_rawget(L,t_index);
	return 0;
}

static inline int ch_lua_array_get(lua_State *L,
		    int a_index,
		    int e_index)
{
	
	if(!ch_check_type(L,a_index,L_TABLE_TYPE))
	{
		return -1;
	}

	lua_rawgeti(L,a_index,e_index);

    return 0;
}

static inline int ch_lua_call(lua_State *L,int nargs,int nresults,int msgh)
{
	
	size_t len;
	char *err_msg;
	
	int rc=lua_pcall(L,nargs,nresults,msgh);

	if(rc!=LUA_OK)
	{
			
		ch_lua_stack_get_string(L,
					 &err_msg,
					 &len,
					 CH_LUA_STACK_TOP_INDEX);

		printf("Error:lua call failed %s\n",err_msg);
		return -1;	
	}
	return 0;
}

static inline int ch_lua_match_call(lua_State *L,int nargs){

	size_t len;
	char *err_msg;
	
	int rc=lua_pcall(L,nargs,1,0);

	if(rc!=LUA_OK)
	{
			
		ch_lua_stack_get_string(L,
					 &err_msg,
					 &len,
					 CH_LUA_STACK_TOP_INDEX);

		printf("Error:lua call failed %s\n",err_msg);
		return -1;	
	}

	return lua_tointeger(L,-1);
}

#endif /*CH_LUA_H*/
