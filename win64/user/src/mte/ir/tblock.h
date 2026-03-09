#include <ax_type.h>

#include "mte/cpu.h"

typedef struct _tblock_context{
	struct cpu_reg_map 	*map;
} tblock_context;

typedef struct _tblock{
	enum tblock_type : u8{
		TBLOCK_BIG,
		TBLOCK_SMALL,
	} type;
	void		*base;
	tblock_context	ctx;
} tblock;

