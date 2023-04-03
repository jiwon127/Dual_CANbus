#include <stm32f105xc.h>
#include <stdint.h>

#include "GPIOconfig.h"
#include "GlobalVariable.h"

#define CAN_01B0                0xA0C00D84  // 141801b0 << 3 | 0x01 << 2
#define CAN_04B0                0xA0C02584  // 141804b0 << 3 | 0x01 << 2

// ���� �̸� �ٲܰ�.
extern CAN_DATA globalCANdata[2];

void CAN1config(void);
void CAN2config(void);

void CAN1_filterconfig(int msgboxnumber, int id);  
void CAN1_filterconfig2(void);

void CAN_TxMessage2(int mailboxnumber, unsigned int id );

//**************** gpio pin config ****************// 
// PB12 : CAN2Rx, PB13 : CAN2Tx - CRH
// PB8  : CAN1Rx, PB9 : CAN1Tx - CRH
// Rx = 0100 (reset state)
//Tx = 1011
// PB8, PB9 - Rx, Tx remapping - MAPR
void CAN_config()
{
    RCC->APB1ENR |= 0x06000000;
    // CAN1, CAN2 enable

    GPIOB->CRH = 0x44B444B4;
    AFIO->MAPR |= 0x00004000; // remapping CAN1, CAN2 in 176page

    //**************** gpio pin config ****************// 
    CAN1config();
    CAN2config();

    NVIC->ISER[0] = (1 << 20);
    //NVIC->IP[63] = 19;
    //NVIC->ISER[1] = 0x80000000;

    CAN1->MCR &= 0xFFFFFFFE;
    CAN2->MCR &= 0xFFFFFFFE;
    // MCR. INRQ clear
}

void CAN1config(void)
{ 
    CAN1->MCR = (CAN1->MCR & (unsigned long)0xFFFFFFFD) | 0x01;
    // INRQ set : init mode on
    // SLEEP clear : sleep mode off

#if (debugging == 1)
    CAN1->BTR = 0x00010004; // 1000 kpbs // ts1 = 2, ts2 = 1, seg = 1;
#else
    CAN1->BTR = 0x00140004; // 500 kbps // ts1 = 5, ts2 = 2, seg = 1;
#endif

#if (debugging == 1)
    CAN1_filterconfig(0, 0x3B);
    CAN1_filterconfig(1, 0x3cb);  
    CAN1_filterconfig(2, 0x6b2); 
#else
    CAN1_filterconfig2();
    //CAN1_filterconfig(0, 0x141801b0);
    //CAN1_filterconfig(1, 0x141804b0);
#endif
    CAN1->FFA1R = 0x00000000;
    // Select FIFO number
    CAN1->FA1R = 0xFFFFFFFF;
    CAN1->FMR = 0x0;
    CAN1->IER = (CAN1->IER & (unsigned long)0xFFFFFFFF) | 0x00000002; // interrupt enable register
}

void CAN1_filterconfig2(void)
{
   CAN1->FMR = (CAN1->FMR & (unsigned long)0xffffffff) | 0x01; // FINIT Bit set = Filter Init mode

   CAN1->FS1R |= 0x0fffffff;
    // filter scale is 32-bit scale
    CAN1->FM1R |= 0x0fffffff;
    // filter mode is ID list mode 32-bit
   
    CAN1->sFilterRegister[0].FR1 = CAN_01B0;
    CAN1->sFilterRegister[0].FR2 = CAN_01B0;
    CAN1->sFilterRegister[1].FR1 = CAN_04B0;
    CAN1->sFilterRegister[1].FR2 = CAN_04B0;
}
/*
void CAN1_filterconfig(int msgboxnumber, int id)
{
    int32_t FilterMaskIdLow;
    int32_t FilterMaskIdHigh;

    CAN1->FMR = (CAN1->FMR & (unsigned long)0xffffffff) | 0x01; // FINIT Bit set = Filter Init mode

#if (debugging == 1) 
    CAN1->FS1R = (CAN1->FS1R & (unsigned long)0xFFFFFFFF) | 0x00;
    CAN1->FM1R = (CAN1->FM1R & (unsigned long)0xFFFFFFFF) | 0x00;
#else
    CAN1->FS1R |= 0x0fffffff;
    // filter scale is 32-bit scale
    CAN1->FM1R |= 0x0fffffff;
    // filter mode is ID list mode 32-bit
#endif

#if (debugging == 1)
    FilterMaskIdHigh = (id << 5);
    FilterMaskIdLow = 0;
#else
    FilterMaskIdHigh = (id << 3) & 0xFFFF0000;
    FilterMaskIdLow = ((id << 3) & 0xffff) | (0x1 << 2);
#endif
    CAN1->sFilterRegister[msgboxnumber].FR2 = FilterMaskIdHigh;
    CAN1->sFilterRegister[msgboxnumber].FR2 |= FilterMaskIdLow;
    CAN1->sFilterRegister[msgboxnumber].FR1 = FilterMaskIdHigh;
    CAN1->sFilterRegister[msgboxnumber].FR1 |= FilterMaskIdLow;
}
*/
void CAN2config(void)
{
    CAN2->MCR = (CAN2->MCR & (unsigned long)0xFFFFFFFD) | 0x01;
    // INRQ bit set, SLEEP bit clear.
#if (debugging == 1)
    CAN2->BTR = 0x00010004; // 1000 kpbs // ts1 = 2, ts2 = 1, seg = 1;
#else
    CAN2->BTR = 0x00140004; // 500 kbps // ts1 = 5, ts2 = 2, seg = 1;
#endif
    CAN2->IER |= 0x1;  
    NVIC->ISER[1] |= (1 << 31);
} 

void CAN_TxMessage2(int mailboxnumber, unsigned int id )
{
        CAN2->sTxMailBox[0].TIR = (( id << 3 ) | 0x4);
        CAN2->sTxMailBox[0].TDTR = ( CAN2->sTxMailBox[0].TDTR & (unsigned long)0xfffffff0)| 0x8;
        
        CAN2->sTxMailBox[0].TDLR = CAN1->sFIFOMailBox[0].RDLR;
        CAN2->sTxMailBox[0].TDHR = CAN1->sFIFOMailBox[0].RDHR;
        
        CAN2->sTxMailBox[0].TIR |= 0x1;
}

void CAN1_RX0_IRQHandler(void)
{
    CAN1->IER = 0x0;
    
    if(CAN_01B0 == CAN1->sFIFOMailBox[0].RIR)
    {
      CAN_TxMessage2(0, 0x143801B0);
    }
    
    if(CAN_04B0 == CAN1->sFIFOMailBox[0].RIR)
    {
      CAN_TxMessage2(1, 0x143804B0);
    }
    CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;  // Release FIFO 0 output mailbox
    CAN2->TSR |= 0x1;//(1 << 8); // auto ???
}

void CAN2_TX_IRQHandler(void)
{
 // CAN2->TSR &= 0xFFFFFFFC;
    CAN1->IER = (CAN1->IER & (unsigned long)0xFFFFFFFF) | 0x0000000E; // interrupt enable register
}