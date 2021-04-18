#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <systemctrl.h>

#include <string.h>

#include "libertas.h"

PSP_MODULE_INFO("MacSpoofer_Module", 0x1006, 1, 0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

STMOD_HANDLER previous;
u8 mac[6];

int (* ReverseCommandEndianess)(LIBERTAS_COMMAND *cmd, int size);

int ReverseCommandEndianessPatched(LIBERTAS_COMMAND *cmd, int size)
{
	// Note: not only host commands are sent to this function

#if 0
	printf("Libertas command 0x%04X\n", cmd->CmdCode);
#endif
	
	if (cmd->CmdCode == LIBERTAS_CMD_GET_HW_SPEC)
	{
		// Check the size to verify it is really the HW_SPEC host command
		
		if (cmd->Size == (sizeof(LIBERTAS_GET_HW_SPEC_COMMAND) + 8))
		{
			memcpy(cmd->hwspec.MacAddr, mac, 6);
		}		
	}
	
	return ReverseCommandEndianess(cmd, size);
}

int OnModuleStart(SceModule2 *module)
{
	if (strcmp(module->modname, "sceWlan_Driver") == 0)
	{
		int i, inst = 0;

		// Find the function. The code may break in future firmwares.
		for (i = 0; i < module->text_size; i += 4)
		{
			u32 *addr = (u32 *)(module->text_addr+i);

			if (addr[0] == 0x00053083 &&
				addr[1] == 0x00061880 &&
				addr[7] == 0x7c0618e0)
			{
				ReverseCommandEndianess = (void *)addr;
				MAKE_CALL((u32)&inst, (u32)addr);
				
				sceKernelDcacheWritebackAll();
				sceKernelIcacheClearAll();

				break;
			}
		}

		if (inst != 0)
		{						
			for (i = 0; i < module->text_size; i += 4)
			{
				if (*(u32 *)(module->text_addr+i) == inst)
				{					
					MAKE_CALL(module->text_addr+i, ReverseCommandEndianessPatched);
				}
			}

			sceKernelDcacheWritebackAll();
			sceKernelIcacheClearAll();
		}
	}
	
	return previous(module);
}

static void upper_case(char *str, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] -= ('a' - 'A');
		}
	}
}

int GetHexNumber(char c)
{
	int res = -1;

	if (c >= '0' && c <= '9')
	{
		res = c-'0';
	}
	else if (c >= 'A' && c <= 'F')
	{
		res = c-'A' + 10;
	}

	return res;
}

int module_start(SceSize args, void *argp)
{
	u8 buf[17];
	int i;
	SceUID fd;
	u8 random[20];
	
	fd = sceIoOpen("ms0:/seplugins/mac.txt", PSP_O_RDONLY, 0);

	if (fd < 0)
	{
		return 1;
	}

	int read = sceIoRead(fd, buf, sizeof(buf));
	sceIoClose(fd);

	if (read < sizeof(buf))
		return 1;

	upper_case((char *)buf, 15);

	for (i = 0; i < 6; i++)
	{
		if (buf[3*i] == 'R' && buf[3*i+1] == 'R')
		{
			if (sceDdrdbPrngen(random) >= 0)
				mac[i] = random[i];
			else
				mac[i] = 0xDA;
		}
		else
		{
			int a, b;
			
			a = GetHexNumber(buf[3*i]);
			b = GetHexNumber(buf[3*i+1]);
			
			if (a == -1 || b == -1)
				return 1;

			mac[i] = (a << 4) | b;
		}

		if (i != 5 && buf[3*i+2] != ':')
			return 1;
	}
	
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	
	return 0;
}



