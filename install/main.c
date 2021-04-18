#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <kubridge.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

PSP_MODULE_INFO("MacSpoofer_Install", 0x800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf    pspDebugScreenPrintf

typedef struct BtcnfHeader
{
	u32 signature; // 0
	u32 devkit;		// 4
	u32 unknown[2];  // 8
	u32 modestart;  // 0x10
	int nmodes;  // 0x14
	u32 unknown2[2];  // 0x18
	u32 modulestart; // 0x20
	int nmodules;  // 0x24
	u32 unknown3[2]; // 0x28
	u32 modnamestart; // 0x30
	u32 modnameend; // 0x34
	u32 unknown4[2]; // 0x38
}  __attribute__((packed)) BtcnfHeader;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart; //
	int mode1;
	int mode2;
	int reserved[5];
} __attribute__((packed)) ModeEntry;

typedef struct ModuleEntry
{
	u32 stroffset;
	int reserved;
	u16 flags;
	u8 loadmode;
	u8 signcheck;
	int reserved2;
	u8  hash[0x10];
} __attribute__((packed)) ModuleEntry;

u8 g_buf[30000];

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);
	
	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	
	if (fd < 0)
	{
		return fd;
	}

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int ChangeBtcnfBuf(u8 *buf, int size)
{
	BtcnfHeader *header = (BtcnfHeader *)buf;
	ModuleEntry *modules, temp[2];
	char *names;
	int i, w = 0, nw = 0;

	if (header->signature != 0x0F803001)
	{
		return -1;
	}

	modules = (ModuleEntry *)((u32)header + header->modulestart);
	names = (char *)((u32)header + header->modnamestart);

	for (i = 0; i < header->nmodules; i++)
	{
		if (strcmp(names+modules[i].stroffset, "/kd/wlan.prx") == 0)
		{
			w = i;
			// Save wlan.prx and magpie/voyager
			memcpy(temp, modules+i, sizeof(temp));
		}

		else if (strcmp(names+modules[i].stroffset, "/kd/mediasync.prx") == 0)
		{
			nw = i+1;
			break;
		}
	}

	if (i == header->nmodules)
		return -2;

	if (w == 0)
		return -3;

	memmove(modules+w, modules+w+2, (nw-w-2) * sizeof(ModuleEntry));
	memcpy(modules+(nw-2), temp, sizeof(temp));
			
	return 0;
}

int ChangeBtcnfFile(char *file)
{
	printf("Patching %s... ", file);
	
	int size = ReadFile(file, g_buf, sizeof(g_buf));

	if (size <= 0)
	{
		printf("Cannot open it for reading.\n");
		return -4;
	}

	switch (ChangeBtcnfBuf(g_buf, size))
	{
		case -1:
			printf("Invalid BTCNF version, this program is not compatible with this firmware.\n");
			return -1;
		break;

		case -2:
			printf("WTF, mediasync not found.\n"); // Shouldn't happen...
			return -2;
		break;

		case -3:
			printf("Patch was already applied to this file.\n");
			return -3;
		break;
	}

	if (WriteFile(file, g_buf, size) != size)
	{
		printf("Cannot write back to the file\n");
		return -5;
	}

	printf(" done\n");
	return 0;
}

void Reassign()
{
	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(5000, "Error in unassign.\n");
	}

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		ErrorExit(5000, "Error in assign.\n");
	}
}

int main(void)
{  
	pspDebugScreenInit();

	Reassign();

	if (kuKernelGetModel() == PSP_MODEL_STANDARD)
	{
		ChangeBtcnfFile("flash0:/kd/pspbtjnf.bin");
		ChangeBtcnfFile("flash0:/kd/pspbtknf.bin");
		ChangeBtcnfFile("flash0:/kd/pspbtlnf.bin");
	}
	else
	{
		ChangeBtcnfFile("flash0:/kd/pspbtjnf_02g.bin");
		ChangeBtcnfFile("flash0:/kd/pspbtknf_02g.bin");
		ChangeBtcnfFile("flash0:/kd/pspbtlnf_02g.bin");
	}

	ErrorExit(5000, "\nFinished.\n\n");

    return 0;
}

