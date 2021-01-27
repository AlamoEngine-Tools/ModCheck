/*
** $Id: lundump.h,v 1.30 2003/04/07 20:34:20 lhf Exp $
** load pre-compiled Lua chunks
** See Copyright Notice in lua.h
*/

#ifndef lundump_h
#define lundump_h

#include "lobject.h"
#include "lzio.h"

/* load one chunk; from lundump.c */
Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff);

/* find byte order; from lundump.c */
int luaU_endianness (void);

/* dump one chunk; from ldump.c */
void luaU_dump (lua_State* L, const Proto* Main, lua_Chunkwriter w, void* data);

/* print one chunk; from print.c */
void luaU_print (const Proto* Main);

/* definitions for headers of binary files */
/*<customized>*/
#define	LUA_SIGNATURE	"\033Lup"	/* Binary files start with "<esc>Lup" for Petroglyph Lua files */
#define	VERSION		0x51		    /* Petroglyph LUA 5.0 files mask as 5.1 */
#define	VERSION0	0x51		    /* Petroglyph LUA 5.0 files mask as 5.1 */
/*</customized>*/

/* a multiple of PI for testing native format */
/* multiplying by 1E7 gives non-trivial integer values */
#define	TEST_NUMBER	((lua_Number)3.14159265358979323846E7)

#endif
