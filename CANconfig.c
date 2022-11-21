#include <stm32f105xc.h>
#include <stdint.h>

#include "GPIOconfig.h"
#include "GlobalVariable.h"

#define CAN1_RF0R               (*((volatile uint32_t *)0x400065B0))
#define CAN1_RDT0R              (*((volatile uint32_t *)0x400065B4))

// ���� �̸� �ٲܰ�.
unsigned int x[6];
unsigned int y[3];
int RxIRQswitch = 0;
///
extern CAN_DATA globalCANdata[6];
int mailboxFLAG = 0;


void CAN1config(void);
void CAN2config(void);

void CAN1_filterconfig(int msgboxnumber, int id);  

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

    NVIC->ISER[0] = (3 << 20);
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
    CAN1_filterconfig(0, 0x141801b0);
    CAN1_filterconfig(1, 0x141802b0);
    CAN1_filterconfig(2, 0x141803b0);
    CAN1_filterconfig(3, 0x141804b0);
    CAN1_filterconfig(4, 0x141805b0);
    CAN1_filterconfig(5, 0x141806b0);
#endif
    CAN1->FFA1R = 0x00000000;
    // Select FIFO number
    CAN1->FA1R = 0xFFFF;
    CAN1->FMR = 0x0;
    CAN1->IER = (CAN1->IER & (unsigned long)0xFFFFFFFF) | 0x0000000E; // interrupt enable register
}

void CAN1_filterconfig(int msgboxnumber, int id)
{
    uint32_t FilterMaskIdLow;
    uint32_t FilterMaskIdHigh;

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
    FilterMaskIdHigh = (id << 3) >> 16;
    FilterMaskIdLow = ((id << 3) & 0xffff) | (0x1 << 2);
#endif
    CAN1->sFilterRegister[msgboxnumber].FR1 = FilterMaskIdHigh;
    CAN1->sFilterRegister[msgboxnumber].FR2 = FilterMaskIdLow;
    
    x[msgboxnumber] = CAN1->sFilterRegister[msgboxnumber].FR1 << 16;
}

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

// mailbox identifier configuration

// unsigned int testvalue = 0;


// void CAN1_RX0_IRQHandler(void)
// {
//     NVIC->ISER[0] = 0x10000000;
//     CAN1->IER = 0x0;
//     RxIRQswitch = GetRxIRQswitch();

//     switch (RxIRQswitch)
//     {
//     case BMSone:
//         CAN_AddRxMessage(BMSone, 0x142801B0);
//         break;
//     case BMStwo:
//         CAN_AddRxMessage(BMStwo, 0x142802B0);
//         break;
//     case BMSthree:
//         CAN_AddRxMessage(BMSthree, 0x142803B0);
//         break;
//         /*
//      case BMSfour:
//          CAN_AddRxMessage(BMSfour, 0x142804B0);
//          break;    
//      case BMSfive:
//          CAN_AddRxMessage(BMSfive, 0x142805B0);
//          break;    
//      case BMSsix:
//          CAN_AddRxMessage(BMSsix, 0x142806B0);
//          break;
//          */
//     }
//   CAN2->TSR |= (1 << 8);
//   CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20; 
// }
/* ------------------------------------------- */
unsigned int testvalue = 0;
int GetRxIRQswitch(void)
{
    // #if (debugging == 1)

    // #else
    //     return testvalue = 8 + ( ((CAN1->sFIFOMailBox[0].RIR >> 8) - 0xD) / 8);
    // #endif
}


void CAN_AddRxMessage(int mailboxnumber)
{
        globalCANdata[mailboxnumber].data[0] = CAN1->sFIFOMailBox[0].RDLR >> 0;
        globalCANdata[mailboxnumber].data[1] = CAN1->sFIFOMailBox[0].RDLR >> 8;
        globalCANdata[mailboxnumber].data[2] = CAN1->sFIFOMailBox[0].RDLR >> 16;
        globalCANdata[mailboxnumber].data[3] = CAN1->sFIFOMailBox[0].RDLR >> 24;

        globalCANdata[mailboxnumber].data[4] = CAN1->sFIFOMailBox[0].RDHR >> 0;
        globalCANdata[mailboxnumber].data[5] = CAN1->sFIFOMailBox[0].RDHR >> 8;
        globalCANdata[mailboxnumber].data[6] = CAN1->sFIFOMailBox[0].RDHR >> 16;
        globalCANdata[mailboxnumber].data[7] = CAN1->sFIFOMailBox[0].RDHR >> 24;
          //CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;
}

void CAN_TxMessage(int mailboxnumber, unsigned int id)
{
          CAN2->sTxMailBox[0].TIR = (( id << 3 ) | 0x4);
        //CAN2->sTxMailBox[0].TIR = (( 0x3b << 21 ) | 0x0 );
        CAN2->sTxMailBox[0].TDTR = ( CAN2->sTxMailBox[0].TDTR & (unsigned long)0xfffffff0)| 0x8;

        CAN2->sTxMailBox[0].TDLR = ((uint32_t)globalCANdata[mailboxnumber].data[0] << 0 | 
                                    (uint32_t)globalCANdata[mailboxnumber].data[1] << 8 |
                                    (uint32_t)globalCANdata[mailboxnumber].data[2] << 16 |
                                    (uint32_t)globalCANdata[mailboxnumber].data[3] << 24 ) ;
        CAN2->sTxMailBox[0].TDHR = ((uint32_t)globalCANdata[mailboxnumber].data[4] << 0 | 
                                    (uint32_t)globalCANdata[mailboxnumber].data[5] << 8 |
                                    (uint32_t)globalCANdata[mailboxnumber].data[6] << 16 |
                                    (uint32_t)globalCANdata[mailboxnumber].data[7] << 24 ) ;
        CAN2->sTxMailBox[0].TIR |= 0x1;
}

void CAN1_RX0_IRQHandler(void)
{
    NVIC->ISER[0] = 0x10000000;
    CAN1->IER = 0x0;
    //RxIRQswitch = GetRxIRQswitch();
        if (CAN1_RF0R == x[0])
        {
            RxIRQswitch =  1;
        }
        else if (CAN1_RF0R == x[1])
        {
            RxIRQswitch =  2;
        }
        else if (CAN1_RF0R == x[2])
        {
            RxIRQswitch =  3;
        }

    switch (RxIRQswitch)
    {
    case BMSone:
        CAN_AddRxMessage(0);
        CAN_TxMessage(0,0x142801B0);
        break;
    case BMStwo:
        CAN_AddRxMessage(1);
        CAN_TxMessage(1,0x142802B0);
        break;
    case BMSthree:
        CAN_AddRxMessage(2);
        CAN_TxMessage(2,0x142803B0); 
        break;
        /*
     case BMSfour:
         CAN_AddRxMessage(BMSfour, 0x142804B0);
         break;    
     case BMSfive:
         CAN_AddRxMessage(BMSfive, 0x142805B0);
         break;    
     case BMSsix:
         CAN_AddRxMessage(BMSsix, 0x142806B0);
         break;
         */
    }
  CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20; 
  CAN2->TSR |= (1 << 8);
        CAN2->TSR &= 0xFFFFFFFC;
      CAN1->IER = (CAN1->IER & (unsigned long)0xFFFFFFFF) | 0x0000000E; // interrupt enable register

}
///////
// void CAN1_RX0_IRQHandler(void)
// {
//   NVIC->ISER[0] = 0x10000000;
//   CAN1->IER = 0x0;

//   if ( (( CAN1->RF0R & CAN_RF0R_FULL0_Pos) == 1) || (CAN1->RF0R & CAN_RF0R_FMP0) > 0)
//   {
//     while( CAN1->RF0R != 0x0)
//     {
//       CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;


//   if ( CAN1_RF0R == x[0])
//   {
//     globalCANdata[0].data[0] = CAN1->sFIFOMailBox[0].RDLR >> 0;
//     globalCANdata[0].data[1] = CAN1->sFIFOMailBox[0].RDLR >> 8;
//     globalCANdata[0].data[2] = CAN1->sFIFOMailBox[0].RDLR >> 16;
//     globalCANdata[0].data[3] = CAN1->sFIFOMailBox[0].RDLR >> 24;

//     globalCANdata[0].data[4] = CAN1->sFIFOMailBox[0].RDHR >> 0;
//     globalCANdata[0].data[5] = CAN1->sFIFOMailBox[0].RDHR >> 8;
//     globalCANdata[0].data[6] = CAN1->sFIFOMailBox[0].RDHR >> 16;
//     globalCANdata[0].data[7] = CAN1->sFIFOMailBox[0].RDHR >> 24;

//     mailboxFLAG = 0;
//   }

//   else if ( CAN1_RF0R == x[1])
//   {  
//     globalCANdata[1].data[0] = CAN1->sFIFOMailBox[0].RDLR >> 0;
//     globalCANdata[1].data[1] = CAN1->sFIFOMailBox[0].RDLR >> 8;
//     globalCANdata[1].data[2] = CAN1->sFIFOMailBox[0].RDLR >> 16;
//     globalCANdata[1].data[3] = CAN1->sFIFOMailBox[0].RDLR >> 24;

//     globalCANdata[1].data[4] = CAN1->sFIFOMailBox[0].RDHR >> 0;
//     globalCANdata[1].data[5] = CAN1->sFIFOMailBox[0].RDHR >> 8;
//     globalCANdata[1].data[6] = CAN1->sFIFOMailBox[0].RDHR >> 16;
//     globalCANdata[1].data[7] = CAN1->sFIFOMailBox[0].RDHR >> 24;

//     mailboxFLAG = 1;
//   }

//   else if ( CAN1_RF0R == x[2])
//   {  
//     globalCANdata[2].data[0] = CAN1->sFIFOMailBox[0].RDLR >> 0;
//     globalCANdata[2].data[1] = CAN1->sFIFOMailBox[0].RDLR >> 8;
//     globalCANdata[2].data[2] = CAN1->sFIFOMailBox[0].RDLR >> 16;
//     globalCANdata[2].data[3] = CAN1->sFIFOMailBox[0].RDLR >> 24;

//     globalCANdata[2].data[4] = CAN1->sFIFOMailBox[0].RDHR >> 0;
//     globalCANdata[2].data[5] = CAN1->sFIFOMailBox[0].RDHR >> 8;
//     globalCANdata[2].data[6] = CAN1->sFIFOMailBox[0].RDHR >> 16;
//     globalCANdata[2].data[7] = CAN1->sFIFOMailBox[0].RDHR >> 24;

//     mailboxFLAG = 2;
//   }

//   if (mailboxFLAG == 0)
//   {
//       CAN2->sTxMailBox[0].TIR = (( 0x142801B0 << 3 ) | 0x4);
//       //CAN2->sTxMailBox[0].TIR = (( 0x3b << 21 ) | 0x0 );
//       CAN2->sTxMailBox[0].TDTR = ( CAN2->sTxMailBox[0].TDTR & (unsigned long)0xfffffff0)| 0x5;
      
//       CAN2->sTxMailBox[0].TDLR = ((uint32_t)globalCANdata[0].data[0] << 0 | 
//                                   (uint32_t)globalCANdata[0].data[1] << 8 |
//                                   (uint32_t)globalCANdata[0].data[2] << 16 |
//                                   (uint32_t)globalCANdata[0].data[3] << 24 ) ;
//       CAN2->sTxMailBox[0].TDHR = ((uint32_t)globalCANdata[0].data[4] << 0 | 
//                                   (uint32_t)globalCANdata[0].data[5] << 8 |
//                                   (uint32_t)globalCANdata[0].data[6] << 16 |
//                                   (uint32_t)globalCANdata[0].data[7] << 24 ) ;
//       CAN2->sTxMailBox[0].TIR |= 0x1;
//   }

//   else if (mailboxFLAG == 1)
//   {
//       CAN2->sTxMailBox[0].TIR = (( 0x142802B1 << 3 ) | 0x4);     
//       //CAN2->sTxMailBox[0].TIR = (( 0x3cb << 21 ) | 0x0 );

//       CAN2->sTxMailBox[0].TDTR = ( CAN2->sTxMailBox[0].TDTR & (unsigned long)0xfffffff0) | 0x7;

//       CAN2->sTxMailBox[0].TDLR = ((uint32_t)globalCANdata[1].data[0] << 0 | 
//                                   (uint32_t)globalCANdata[1].data[1] << 8 |
//                                   (uint32_t)globalCANdata[1].data[2] << 16 |
//                                   (uint32_t)globalCANdata[1].data[3] << 24 ) ;
//       CAN2->sTxMailBox[0].TDHR = ((uint32_t)globalCANdata[1].data[4] << 0 | 
//                                   (uint32_t)globalCANdata[1].data[5] << 8 |
//                                   (uint32_t)globalCANdata[1].data[6] << 16 |
//                                   (uint32_t)globalCANdata[1].data[7] << 24 ) ;    
//       CAN2->sTxMailBox[0].TIR |= 0x1;
//   }  

//   else if (mailboxFLAG == 2)
//   {
//       CAN2->sTxMailBox[0].TIR = (( 0x142803B2 << 3 ) | 0x4);     
//       //CAN2->sTxMailBox[0].TIR = (( 0x6b2 << 21 ) | 0x0 );

//       CAN2->sTxMailBox[0].TDTR = ( CAN2->sTxMailBox[0].TDTR & (unsigned long)0xfffffff0) | 0x8;

//       CAN2->sTxMailBox[0].TDLR = ((uint32_t)globalCANdata[2].data[0] << 0 | 
//                                   (uint32_t)globalCANdata[2].data[1] << 8 |
//                                   (uint32_t)globalCANdata[2].data[2] << 16 |
//                                   (uint32_t)globalCANdata[2].data[3] << 24 ) ;
//       CAN2->sTxMailBox[0].TDHR = ((uint32_t)globalCANdata[2].data[4] << 0 | 
//                                   (uint32_t)globalCANdata[2].data[5] << 8 |
//                                   (uint32_t)globalCANdata[2].data[6] << 16 |
//                                   (uint32_t)globalCANdata[2].data[7] << 24 ) ;   
//       CAN2->sTxMailBox[0].TIR |= 0x1;
//   }  
//       }
//   }
//   CAN2->TSR |= (1 << 8);
//   CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;  
// }




// void CAN1_RX0_IRQHandler(void)
// {
//   NVIC->ISER[0] = 0x10000000;
//   CAN1->IER = 0x0;

//   if ( (( CAN1->RF0R & CAN_RF0R_FULL0_Pos) == 1) || (CAN1->RF0R & CAN_RF0R_FMP0) > 0)
//   {
//     while( CAN1->RF0R != 0x0)
//     {


//   if ( CAN1_RF0R == x[0])
//   {
//     CAN_AddRxMessage(0);
//     mailboxFLAG = 0;
//   }

//   else if ( CAN1_RF0R == x[1])
//   {  
//     CAN_AddRxMessage(1);
//     mailboxFLAG = 1;
//   }

//   else if ( CAN1_RF0R == x[2])
//   {  
//     CAN_AddRxMessage(2);
//     mailboxFLAG = 2;
//   }

//   if (mailboxFLAG == 0)
//   {
//     CAN_TxMessage(0,0x142801B0);
//   }

//   else if (mailboxFLAG == 1)
//   {
//     CAN_TxMessage(1,0x142802B0);
//   }  

//   else if (mailboxFLAG == 2)
//   {
//     CAN_TxMessage(2,0x142803B0);
//   }  
  
//         CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;
//     }
//   }
//   CAN2->TSR |= (1 << 8);
//   //CAN1->RF0R = ( CAN1->RF0R & (unsigned long)0xFFFFFFFF) | 0x20;  
// }

  void CAN2_TX_IRQHandler(void)
  {
      // need test
  }


