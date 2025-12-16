#include "arch.h"

int main(){
	struct cpu *cpu_0 = axmalloc(sizeof(struct cpu));
	cpu_init(cpu_0);

	return 0;
}

