/*
 * hbhk_pwm.h
 *
 *  Created on: 2017年2月24日
 *      Author: liyulin
 */
#ifndef DRV_HBHK_PWM_H_
#define DRV_HBHK_PWM_H_

//typedef unsigned int        Uint16;

void Pie_Register_Pwm();
void Int_Pwm_Enable();
void Init_Pwm();
void InitEPwm1Example();
void InitEPwm2Example();
void InitEPwm3Example();
void InitEPwm4Example();

#define EPWM1_TIMER_TBPRD  2000  // Period register
#define EPWM1_MAX_CMPA     1950
#define EPWM1_MIN_CMPA       50
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB       50

#define EPWM2_TIMER_TBPRD  2000  // Period register
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

#define EPWM4_TIMER_TBPRD  2000
#define EPWM4_MAX_CMPA      950
#define EPWM4_MIN_CMPA       50
#define EPWM4_MAX_CMPB     1950
#define EPWM4_MIN_CMPB     1050

#define EPWM_CMP_UP           1
#define EPWM_CMP_DOWN         0

typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    unsigned int EPwm_CMPA_Direction;
    unsigned int EPwm_CMPB_Direction;
    unsigned int EPwmTimerIntCount;
    unsigned int EPwmMaxCMPA;
    unsigned int EPwmMinCMPA;
    unsigned int EPwmMaxCMPB;
    unsigned int EPwmMinCMPB;
}EPWM_INFO;

void update_compare(EPWM_INFO *epwm_info);
//结构体，用于配置PWM参数

//中断服务
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);
__interrupt void epwm4_isr(void);
#endif /* DRV_HBHK_PWM_H_ */
