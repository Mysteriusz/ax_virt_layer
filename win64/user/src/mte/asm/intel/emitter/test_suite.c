#include "test_suite.h"

enum i64_opcode add_opcodes[30];
i64_operand add_cases[30][I64_MAX_OP_COUNT];

void init_suite() {
        /* ---- ADD_R64_64 ---- */
        // ADD rax, rcx
        add_opcodes[0] = ADD_R64_64;
        add_cases[0][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rAX };
        add_cases[0][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rCX };

        // ADD rax, rbx
        add_opcodes[1] = ADD_R64_64;
        add_cases[1][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rAX };
        add_cases[1][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rBX };

        // ADD r10, rbx
        add_opcodes[2] = ADD_R64_64;
        add_cases[2][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_r10 };
        add_cases[2][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rBX };

        // ADD rax, r10
        add_opcodes[3] = ADD_R64_64;
        add_cases[3][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rAX };
        add_cases[3][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_r10 };

        // ADD r8, r9
        add_opcodes[4] = ADD_R64_64;
        add_cases[4][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_r8 };
        add_cases[4][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_r9 };

        // ADD rax, [rcx]
        add_opcodes[5] = ADD_R64_64;
        add_cases[5][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rAX };
        add_cases[5][1] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rCX };

        // ADD rax, [r10 + r9*8 + 0x1000]
        add_opcodes[6] = ADD_R64_64;
        add_cases[6][0] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_rAX };
        add_cases[6][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W64 }, .id = I64_r10,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_r9,
                        .scale_id = I64_SIB_8,
                        .disp     = 0x1000,
                }};

        // ADD r10, [rbx + rax*2 + 0xff]
        add_opcodes[7] = ADD_R64_64;
        add_cases[7][0] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_r10 };
        add_cases[7][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W64 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .scale_id = I64_SIB_2,
                        .disp     = 0xff,
                }};

        // ADD rax, [ecx]
        add_opcodes[8] = ADD_R64_64;
        add_cases[8][0] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rAX };
        add_cases[8][1] = (i64_operand){ .desc = { I64_MEM, W32 }, .id = I64_rCX };

        // ADD rax, [r10d + r9d*8 + 0x1000]
        add_opcodes[9] = ADD_R64_64;
        add_cases[9][0] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_rAX };
        add_cases[9][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W32 }, .id = I64_r10,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_r9,
                        .scale_id = I64_SIB_8,
                        .disp     = 0x1000,
                }};

        // ADD r10, [ebx + eax*2 + 0xff]
        add_opcodes[10] = ADD_R64_64;
        add_cases[10][0] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_r10 };
        add_cases[10][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W32 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .scale_id = I64_SIB_2,
                        .disp     = 0xff,
                }};

        /* ---- ADD_64_R64 ---- */
        // ADD [rax], rcx
        add_opcodes[11] = ADD_64_R64;
        add_cases[11][0] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rAX };
        add_cases[11][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rCX };

        // ADD [rdx], r15
        add_opcodes[12] = ADD_64_R64;
        add_cases[12][0] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rDX };
        add_cases[12][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_r15 };

        // ADD [r10 + r9*8 + 0x1000], rax
        add_opcodes[13] = ADD_64_R64;
        add_cases[13][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W64 }, .id = I64_r10,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_r9,
                        .scale_id = I64_SIB_8,
                        .disp     = 0x1000,
                }};
        add_cases[13][1] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_rAX };

        // ADD [rbx + rax*2 + 0xff], r10
        add_opcodes[14] = ADD_64_R64;
        add_cases[14][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W64 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .scale_id = I64_SIB_2,
                        .disp     = 0xff,
                }};
        add_cases[14][1] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_r10 };

        // ADD [ecx], rdx
        add_opcodes[15] = ADD_64_R64;
        add_cases[15][0] = (i64_operand){ .desc = { I64_MEM, W32 }, .id = I64_rCX };
        add_cases[15][1] = (i64_operand){ .desc = { I64_REG, W64 }, .id = I64_rDX };

        // ADD [r10d + r9d*8 + 0x1000], rax
        add_opcodes[16] = ADD_64_R64;
        add_cases[16][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W32 }, .id = I64_r10,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_r9,
                        .scale_id = I64_SIB_8,
                        .disp     = 0x1000,
                }};
        add_cases[16][1] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_rAX };

        // ADD [ebx + eax*2 + 0xff], r10
        add_opcodes[17] = ADD_64_R64;
        add_cases[17][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP32, W32 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .index_width = W32,
                        .scale_id = I64_SIB_2,
                        .disp     = 0xff,
                }};
        add_cases[17][1] = (i64_operand){ .desc = { I64_REG,              W64 }, .id = I64_r10 };

        /* ---- ADD_R8_8 ---- */
        // ADD al, cl
        add_opcodes[18] = ADD_R8_8;
        add_cases[18][0] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rAX };
        add_cases[18][1] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rCX };

        // ADD al, bl
        add_opcodes[19] = ADD_R8_8;
        add_cases[19][0] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rAX };
        add_cases[19][1] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rBX };

        // ADD cl, [rax]
        add_opcodes[20] = ADD_R8_8;
        add_cases[20][0] = (i64_operand){ .desc = { I64_REG, W8  }, .id = I64_rCX };
        add_cases[20][1] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rAX };

        // ADD cl, [rbx + rax*2 + 0x10]
        add_opcodes[21] = ADD_R8_8;
        add_cases[21][0] = (i64_operand){ .desc = { I64_REG,             W8  }, .id = I64_rCX };
        add_cases[21][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP8, W64 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .scale_id = I64_SIB_2,
                        .disp     = 0x10,
                }};

        // ADD cl, [ecx]
        add_opcodes[22] = ADD_R8_8;
        add_cases[22][0] = (i64_operand){ .desc = { I64_REG, W8  }, .id = I64_rCX };
        add_cases[22][1] = (i64_operand){ .desc = { I64_MEM, W32 }, .id = I64_rCX };

        // ADD cl, [ebx + eax*4 + 0x20]
        add_opcodes[23] = ADD_R8_8;
        add_cases[23][0] = (i64_operand){ .desc = { I64_REG,             W8  }, .id = I64_rCX };
        add_cases[23][1] = (i64_operand){ .desc = { I64_SIB | I64_DISP8, W32 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rAX,
                        .scale_id = I64_SIB_4,
                        .disp     = 0x20,
                }};

        /* ---- ADD_8_R8 ---- */
        // ADD [rax], cl
        add_opcodes[24] = ADD_8_R8;
        add_cases[24][0] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rAX };
        add_cases[24][1] = (i64_operand){ .desc = { I64_REG, W8  }, .id = I64_rCX };

        // ADD [rdx], bl
        add_opcodes[25] = ADD_8_R8;
        add_cases[25][0] = (i64_operand){ .desc = { I64_MEM, W64 }, .id = I64_rDX };
        add_cases[25][1] = (i64_operand){ .desc = { I64_REG, W8  }, .id = I64_rBX };

        // ADD bl, cl
        add_opcodes[26] = ADD_8_R8;
        add_cases[26][0] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rBX };
        add_cases[26][1] = (i64_operand){ .desc = { I64_REG, W8 }, .id = I64_rCX };

        // ADD [rbx + rcx*4 + 0x08], al
        add_opcodes[27] = ADD_8_R8;
        add_cases[27][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP8, W64 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rCX,
                        .scale_id = I64_SIB_4,
                        .disp     = 0x08,
                }};
        add_cases[27][1] = (i64_operand){ .desc = { I64_REG,             W8  }, .id = I64_rAX };

        // ADD [ecx], bl
        add_opcodes[28] = ADD_8_R8;
        add_cases[28][0] = (i64_operand){ .desc = { I64_MEM, W32 }, .id = I64_rCX };
        add_cases[28][1] = (i64_operand){ .desc = { I64_REG, W8  }, .id = I64_rBX };

        // ADD [ebx + ecx*4 + 0x08], al
        add_opcodes[29] = ADD_8_R8;
        add_cases[29][0] = (i64_operand){ .desc = { I64_SIB | I64_DISP8, W32 }, .id = I64_rBX,
                .value = *(u64*)&(struct i64_operand_mem){
                        .index_id = I64_rCX,
                        .scale_id = I64_SIB_4,
                        .disp     = 0x08,
                }};
        add_cases[29][1] = (i64_operand){ .desc = { I64_REG,             W8  }, .id = I64_rAX };
}
