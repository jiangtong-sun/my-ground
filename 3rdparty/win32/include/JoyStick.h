#ifndef _JOYSTICK_H
#define _JOYSTICK_H

extern "C"{
 int	JS4_Attach (int ID);
 int	JS4_IsWorking (int ID);
 int	JS4_GetAxesNums (int ID,int *Number);
 int	JS4_ReadValue (int ID,int *button,float *axes);
 int	JS4_Release (int ID);
}

#endif
