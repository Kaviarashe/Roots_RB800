#include <avr/io.h>

int EMU_StartTarget();

int StartEmu(int c, char** v)
{
	//HANDLE thread1 = CreateThread(NULL, 0, ThreadFuncTarget, NULL, 0, NULL);
	//HANDLE thread2 = CreateThread(NULL, 0, ThreadFuncCAN, NULL, 0, NULL);
	OSC.STATUS = 2;
	EMU_StartTarget();
	while (1);
	return 0;
}
