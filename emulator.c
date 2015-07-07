#include "emulator.h"
#include "8080opcodes.h"


/* Function for unwritten opcodes */
void incomplete(State8080 * state)
{
	
	disassemble8080(state->memory, state->pc);
	printf("Error: Unimplemented instruction \n");
	exit(1);
}

void generate_interrupt(State8080* state, int interrupt_num)
{
	if(state-> int_enable)
	{
		/* Push the PC on the stack and set pc to the corresponding memory location */
		
		state->memory[state->sp-1] = (state->pc >> 8) & 0xFF; ;
		state->memory[state->sp-2] = (state->pc &  0xFF);
		state->sp -= 2;
	
		/* Jump to vector location */
		state->pc =  8 * interrupt_num;
	}

	
}

void emulate8080(State8080 * state, int num_cycles)
{
	int cycle_count = 0;

	while( cycle_count < num_cycles)
	{

		/* Get the current instruction */
		unsigned char * opcode = &state->memory[state->pc];
#ifdef DEBUG 
		printf(" %04x: ", state->pc);
		disassemble8080( state->memory, state->pc);
#endif
		uint16_t adr, answer;
		int temp;
		uint8_t flags;
		switch(opcode[0])
		{
			/* NOP */
			case 0x00: break;
			/* LXI B, word */
			case 0x01:
				   /* Load immediate B <- byte 3, C <- byte 2*/
				   state->c = opcode[1];
				   state->b = opcode[2];
				   state->pc += 2;	/* 3 byte instruction */
				   break;

			/* INX B*/
			case 0x03:
				   if(++state->c == 0) 	/* IF overflow */
					  state->b++;
				   break;
			
			/* INR B*/
			case 0x04:
				   INR(state->b);
				   break;

			/* DCR B*/
			case 0x05:
				   DCR(state->b);
				   break;

			/* MVI B, byte*/
			case 0x06:
				   MVI(state->b);

			/* DAD B */
			case 0x09:
				   /* HL = HL + BC */
				   adr = (state->h << 8) | state->l;
				   answer = (state->b << 8) | state->c;
				   temp = answer + adr;
				   state->cc.cy = (temp > 0xFFFF); 
				   state->h = (temp >> 8) & 0xFF;
				   state->l = temp & 0xFF;
				   break;


			/* LDAX B*/
			case 0x0a:
				   adr = (state->b << 8) | state->c ;
				   state->a = state->memory[adr];
				   break;

			/* INR C*/
			case 0x0c:
				   INR(state->c);
				   break;

			/* DCR C*/
			case 0x0d:
				   DCR(state->c);
				   break;

			/* MVI C, byte */
			case 0x0e:
				   MVI(state->c);
			
			/* RRC */
			case 0x0f:
				   state->cc.cy = ((state->a & 0x80) != 0);
				   state->a >>= 1;
				   state->a |= (state->cc.cy << 7);
				   break;

			/* LXI D, word */
			case 0x11:
				   state->d = opcode[2];
				   state->e = opcode[1];
				   state->pc += 2;
				   break;

			/* INX D*/
			case 0x13:
				   if(++state->e == 0) 	/* IF overflow */
					  state->d++;
				   break;

			/* INR D*/
			case 0x14:
				   INR(state->d);
				   break;

			/* DCR D*/
			case 0x15:
				   DCR(state->d);
				   break;

			/* MVI D, byte */
			case 0x16:
				   MVI(state->d);

			/* DAD D */
			case 0x19:
				   /* HL = HL + DE */
				   adr = (state->h << 8) | state->l;
				   answer = (state->d << 8) | state->e;
				   temp = answer + adr;
				   state->cc.cy = (temp > 0xFFFF); 
				   state->h = (temp >> 8) & 0xFF;
				   state->l = temp & 0xFF;
				   break;

			/* LDAX D*/
			case 0x1a:
				   adr = (state->d << 8) | state->e ;
				   state->a = state->memory[adr];
				   break;

			/* INR E*/
			case 0x1c:
				   INR(state->e);
				   break;

			/* DCR E*/
			case 0x1d:
				   DCR(state->e);
				   break;

			/* MVI E, byte */
			case 0x1e:
				  MVI(state->e);
			
			/* LXI H, word */
			case 0x21:
				  state->h = opcode[2];
				  state->l = opcode[1];
				  state->pc+= 2;
				  break;

			/* INX H*/
			case 0x23:
				   if(++state->l == 0) 	/* IF overflow */
					  state->h++;
				   break;

			/* INR H*/
			case 0x24:
				   INR(state->h);
				   break;

			/* DCR H*/
			case 0x25:
				   DCR(state->h);
				   break;

			/* MVI H, byte */
			case 0x26:
				   MVI(state->h);

			/* DAD H */
			case 0x29:
				   /* HL = HL + HL */
				   adr = (state->h << 8) | state->l;
				   answer = (state->h << 8) | state->l;
				   temp = answer + adr;
				   state->cc.cy = (temp > 0xFFFF); 
				   state->h = (temp >> 8) & 0xFF;
				   state->l = temp & 0xFF;
				   break;

			/* INR L*/
			case 0x2c:
				   INR(state->l);
				   break;

			/* DCR L*/
			case 0x2d:
				   DCR(state->l);
				   break;

			/* MVI L, byte */
			case 0x2e:
				MVI(state->l);		

			/* LXI SP, word */
			case 0x31:
				   state->sp = (opcode[2] << 8)| opcode[1];
				   state->pc += 2;
				   break;

			/* STA adr */
			case 0x32:
				   adr = (opcode[2] << 8) | opcode[1] ; 
				   state->memory[adr] = state->a ;
				   state->pc+= 2;
				   break;

			/* INX SP*/
			case 0x33:
				   state->sp++;
				   break;

			/* INR M*/
			case 0x34:
				   adr = (state->h << 8) | state->l;
				   INR(state->memory[adr]);
				   break;

			/* DCR M*/
			case 0x35:
				   adr = (state->h << 8) | state->l;
				   DCR(state->memory[adr]);
				   break;

			/* MVI M, byte */
			case 0x36:
				adr = (state->h << 8) | state->l;
				state->memory[adr] = opcode[1];	
				state->pc++;
				break;

			/* STC */
			case 0x37:
				state->cc.cy = 1;
				break;

			/* DAD SP */
			case 0x39:
				   /* HL = HL + DE */
				   adr = (state->h << 8) | state->l;
				   answer = state->sp; 
				   temp = answer + adr;
				   state->cc.cy = (temp > 0xFFFF); 
				   state->h = (temp >> 8) & 0xFF;
				   state->l = temp & 0xFF;
				   break;

			/* LDA adr */
			case 0x3a:
				   adr = (opcode[2] << 8) | opcode[1];
				   state->a = state->memory[adr];
				   state->pc += 2;
				   break;

			/* DCR A*/
			case 0x3d:
				DCR(state->a);
				break;

			/* MVI A, byte */
			case 0x3e:
				MVI(state->a);
				
	/************ MOV INSTRUCTIONS ********************/
			case 0x40:
				MOV(state->b, state->b);

			case 0x41:
				MOV(state->b, state->c);	

			case 0x42:
				MOV(state->b, state->d);

			case 0x43:
				MOV(state->b, state->e);	

			case 0x44:
				MOV(state->b, state->h);

			case 0x45:
				MOV(state->b, state->l);	

			case 0x46:
				MOV_TO_M(state->b);

			case 0x47:
				MOV(state->b, state->a);

			case 0x48:
				MOV(state->c, state->b);	

			case 0x49:
				MOV(state->c, state->c);

			case 0x4a:
				MOV(state->c, state->d);	

			case 0x4b:
				MOV(state->c, state->e);

			case 0x4c:
				MOV(state->c, state->h);	

			case 0x4d:
				MOV(state->c, state->l);

			case 0x4e:
				MOV_TO_M(state->c);

			case 0x4f:
				MOV(state->c, state->a);	

			case 0x50:
				MOV(state->d, state->b);

			case 0x51:
				MOV(state->d, state->c);	

			case 0x52:
				MOV(state->d, state->d);

			case 0x53:
				MOV(state->d, state->e);	

			case 0x54:
				MOV(state->d, state->h);

			case 0x55:
				MOV(state->d, state->l);	

			case 0x56:
				MOV_TO_M(state->d);

			case 0x57:
				MOV(state->d, state->a);

			case 0x58:
				MOV(state->e, state->b);	

			case 0x59:
				MOV(state->e, state->c);

			case 0x5a:
				MOV(state->e, state->d);	

			case 0x5b:
				MOV(state->e, state->e);

			case 0x5c:
				MOV(state->e, state->h);	

			case 0x5d:
				MOV(state->e, state->l);

			case 0x5e:
				MOV_TO_M(state->e);

			case 0x5f:
				MOV(state->e, state->a);	

			case 0x60:
				MOV(state->h, state->b);

			case 0x61:
				MOV(state->h, state->c);	

			case 0x62:
				MOV(state->h, state->d);

			case 0x63:
				MOV(state->h, state->e);	

			case 0x64:
				MOV(state->h, state->h);

			case 0x65:
				MOV(state->h, state->l);	

			case 0x66:
				MOV_TO_M(state->h);

			case 0x67:
				MOV(state->h, state->a);

			case 0x68:
				MOV(state->l, state->b);	

			case 0x69:
				MOV(state->l, state->c);

			case 0x6a:
				MOV(state->l, state->d);	

			case 0x6b:
				MOV(state->l, state->e);

			case 0x6c:
				MOV(state->l, state->h);	

			case 0x6d:
				MOV(state->l, state->l);

			case 0x6e:
				MOV_TO_M(state->l);

			case 0x6f:
				MOV(state->l, state->a);	

			case 0x70:
				MOV_FROM_M(state->b);

			case 0x71:
				MOV_FROM_M(state->c);

			case 0x72:
				MOV_FROM_M(state->d);

			case 0x73:
				MOV_FROM_M(state->e);

			case 0x74:
				MOV_FROM_M(state->h);

			case 0x75:
				MOV_FROM_M(state->l);

			case 0x77:
				MOV_FROM_M(state->a);

			case 0x78:
				MOV(state->a, state->b);	

			case 0x79:
				MOV(state->a, state->c);

			case 0x7a:
				MOV(state->a, state->d);	

			case 0x7b:
				MOV(state->a, state->e);

			case 0x7c:
				MOV(state->a, state->h);	

			case 0x7d:
				MOV(state->a, state->l);

			case 0x7e:
				MOV_TO_M(state->a);

			case 0x7f:
				MOV(state->a, state->a);	

	/************************** END OF MOV INSTRUCTIONS *****************/

			/* ANA B*/
			case 0xa0:
				ANA(state->b);
				break;

			/* ANA C*/
			case 0xa1:
				ANA(state->c);
				break;

			/* ANA D*/
			case 0xa2:
				ANA(state->d);
				break;

			/* ANA E*/
			case 0xa3:
				ANA(state->e);
				break;

			/* ANA H*/
			case 0xa4:
				ANA(state->h);
				break;

			/* ANA L*/
			case 0xa5:
				ANA(state->l);
				break;

			/* ANA M*/
			case 0xa6:
				ANA(state->memory[(state->h << 8) |state->l]);
				break;

			/* ANA A*/
			case 0xa7:
				ANA(state->a);
				break;

			/* XRA B*/
			case 0xa8:
				XRA(state->b);
				break;

			/* XRA C*/
			case 0xa9:
				XRA(state->c);
				break;

			/* XRA D*/
			case 0xaa:
				XRA(state->d);
				break;

			/* XRA E*/
			case 0xab:
				XRA(state->e);
				break;

			/* XRA H*/
			case 0xac:
				XRA(state->h);
				break;

			/* XRA L*/
			case 0xad:
				XRA(state->l);
				break;

			/* XRA M*/
			case 0xae:
				XRA(state->memory[(state->h << 8) |state->l]);
				break;

			/* XRA A*/
			case 0xaf:
				XRA(state->a);
				break;

			/* POP B */
			case 0xc1:
				state->c = state->memory[state->sp];
				state->b = state->memory[state->sp + 1];
				state->sp += 2;
				break;

			/* JNZ adr */
			case 0xc2:
				if(!state->cc.z)
				{
				    JMP();
				}
				else
				    state->pc += 2;
				break;

			/* JMP adr */
			case 0xc3:
				JMP();
				break; 

			/* PUSH B*/
			case 0xc5:
				state->memory[state->sp-2] = state->c;
				state->memory[state->sp-1] = state->b;
				state->sp -= 2;
				break;

			/* ADI byte*/
			case 0xc6:
				answer = (uint16_t)state->a + (uint16_t)opcode[1];
				state->cc.z = (answer == 0);
				state->cc.s = (answer & 0x80) != 0;
				state->cc.p = parity(answer);
				state->cc.cy = (answer > 0xFF);
				state->a = answer & 0xFF;
				state->pc++;
				break;

			/* RZ */
			case 0xc8:
				if(state->cc.z)
				{
					RET();
				}
				break;
				

			/* RET */
			case 0xc9:
				RET();
				break;

			/* JZ adr */
			case 0xca:
				if(state->cc.z)
				{
					JMP();
				}
				else
					state->pc += 2;
				break;
				
				
			/* Call adr */
			case 0xcd:
				CALL();
				break;

			/* POP D */
			case 0xd1:
				state->e = state->memory[state->sp];
				state->d = state->memory[state->sp + 1];
				state->sp += 2;
				break;

			/*JNC adr */
			case 0xd2:
				if(!state->cc.cy)
				{
					JMP();
				}
				else
					state->pc+=2;

			/* OUT byte*/
			case 0xd3:
				if(port8080_out != NULL)
					port8080_out(opcode[1], state);
				else
				{
					printf("PORT OUT NOT DEFINED");
					exit(-1);
				}
				break;

			/* PUSH D*/
			case 0xd5:
				state->memory[state->sp-2] = state->e;
				state->memory[state->sp-1] = state->d;
				state->sp -= 2;
				break;

			/* RC */
			case 0xd8:
				if(state->cc.cy)
				{
					RET();
				}
				break;

			/*JC adr */
			case 0xda:
				if(state->cc.cy)
				{
					JMP();
				}
				else
					state->pc+=2;
				break;

			/* IN byte*/
			case 0xdb:
				if(port8080_in != NULL)
					port8080_in(opcode[1], state);
				else
				{
					printf("PORT IN NOT assigned");
					exit(-1);
				}
				break;

			/* POP H */
			case 0xe1:
				state->l = state->memory[state->sp];
				state->h = state->memory[state->sp + 1];
				state->sp += 2;
				break;

			/* PUSH H*/
			case 0xe5:
				state->memory[state->sp-2] = state->l;
				state->memory[state->sp-1] = state->h;
				state->sp -= 2;
				break;

			/* ANI byte*/
			case 0xe6:
				state->a &= opcode[1];
				/* Flags */
				state->cc.z = (state->a == 0);
				state->cc.s = ((state->a & 0x80) != 0);
				state->cc.p = parity(state->a);
				state->cc.cy = 0;
				state->pc++;
				break;

			/* XCHG */
			case 0xeb:
				/* H <-> D; L <->E */
				answer = state->h;
				state->h = state->d;
				state->d = answer;

				answer = state->l;
				state->l = state->e;
				state->e = answer;
				break;

			/* POP PSW */
			case 0xf1:
				flags = state->memory[state->sp];
				state->cc.s = ((flags & 0x80) != 0);
				state->cc.z = ((flags & 0x40) != 0);
				state->cc.ac = ((flags & 0x10) != 0);
				state->cc.p = ((flags & 0x04) != 0);
				state->cc.cy = ((flags & 0x01) != 0);
				state->a = state->memory[state->sp + 1];
				state->sp += 2;
				break;

			/* DI */
			case 0xf3:
				state->int_enable = 0;
				break;

			/* PUSH PSW*/
			case 0xf5:
				flags = 2;
				flags = (state->cc.s << 7) | (state->cc.z << 6);
				flags |= (state->cc.ac << 4) | (state->cc.p <<2 ) | (state->cc.cy << 0);
				state->memory[state->sp-2] = flags;
				state->memory[state->sp-1] = state->a;
				state->sp -= 2;
				break;
			
			/* EI */
			case 0xfb:
				state->int_enable = 1;
				break;

			/* CPI byte*/
			case 0xfe:
				answer = (uint16_t)state->a - (uint16_t)opcode[1];
				/* Set flags */
				state->cc.z = ((answer & 0xFF) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer);
				state->cc.cy = (state->a < opcode[1]);
				state->pc++;
				break;

			default: incomplete(state); break;
		}

		/* Increment pc */
		state->pc++;

		cycle_count += cycles_count[opcode[0]];
	}

}


/* Returns 1 for even number of 1s and 0 for odd */
uint8_t parity(uint16_t number)
{
	uint8_t ret = 1;
	while(number > 0)
	{
		if( number & 1)
			ret ^= 1;
		number >>= 1;
	}
	return ret;
}

