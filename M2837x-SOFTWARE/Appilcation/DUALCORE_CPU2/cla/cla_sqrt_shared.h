#ifndef _CLA_SQRT_SHARED_H_
#define _CLA_SQRT_SHARED_H_
#endif

#include "F2837xD_Cla_defines.h"
#include <stdint.h>

void CLA_configClaMemory(void);
void CLA_initCpu1Cla1(void);

//Task 1 (C) 变量
extern float fVal;
extern float fResult;

//Task 2 (C) 变量

//Task 3 (C) 变量

//Task 4 (C) 变量

//Task 5 (C) 变量

//Task 6 (C) 变量

//Task 7 (C) 变量

//Task 8 (C) 变量

//Common (C) 变量

__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

__interrupt void cla1Isr1();
__interrupt void cla1Isr2();
__interrupt void cla1Isr3();
__interrupt void cla1Isr4();
__interrupt void cla1Isr5();
__interrupt void cla1Isr6();
__interrupt void cla1Isr7();
__interrupt void cla1Isr8();


