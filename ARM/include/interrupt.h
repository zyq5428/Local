#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define EINT0CON0	(*((volatile unsigned long *)0x7f008900))
#define EINT0MASK       (*((volatile unsigned long *)0x7f008920))
#define EINT0PEND       (*((volatile unsigned long *)0x7f008924))

#define VIC0INTENABLE   (*((volatile unsigned long *)0x71200010))
#define VIC0VECTADDR0   (*((volatile unsigned long *)0x71200100))
#define VIC0VECTADDR1   (*((volatile unsigned long *)0x71200104))
#define VIC0VECTADDR25  (*((volatile unsigned long *)0x71200164))

#define VIC0ADDRESS	(*((volatile unsigned long *)0x71200f00))
#define VIC1ADDRESS     (*((volatile unsigned long *)0x71300f00))

#define TINT_CSTAT	(*((volatile unsigned long *)0x7F006044))


#endif /* INTERRUPT_H_  */
