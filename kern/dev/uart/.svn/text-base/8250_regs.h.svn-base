#ifndef _8250_REGS_H_
#define _8250_REGS_H_
	#define RBR_REG_8250 0
	#define THR_REG_8250 0
	#define DLL_REG_8250 0
	#define IER_REG_8250 1
	#define DLM_REG_8250 1
	#define IIR_REG_8250 2
	#define FCR_REG_8250 2
	#define LCR_REG_8250 3
	#define MCR_REG_8250 4
	#define LSR_REG_8250 5
	#define MSR_REG_8250 6
	#define SCR_REG_8250 7
	
	/* IER stuff */
	#define IER_RXA   (1 << 0)
	#define IER_THRE  (1 << 1)
	#define IER_LSR (1 << 2)
	#define IER_MSR (1 << 3)
	#define IER_SLP_MODE  (1 << 4)
	#define IER_LP_MODE  (1 << 5)
	
	/* IIR stuff */
	#define IIR_INT_PENDING(x) (!(x & 1))
	#define IIR_NO_INT (1 << 0)
	#define IIR_TO_CAUSE(x) ((x >> 1) & 0x7)
	#define IIR_CAUSE_MSR 0
	#define IIR_CAUSE_THRE 1
	#define IIR_CAUSE_RXA 2
	#define IIR_CAUSE_LSR 3
	#define IIR_CAUSE_TIMEOUT 6
	#define IIR_THRE(x) (IIR_TO_CAUSE(x) == IIR_CAUSE_THRE)
	#define IIR_RXA(x) (IIR_TO_CAUSE(x) == IIR_CAUSE_RXA)
	#define IIR_REG_TO_CLEAR(x) ( \
		((IIR_TO_CAUSE(x) == IIR_CAUSE_MSR) ? MSR_REG_8250 : \
		 (IIR_TO_CAUSE(x) == IIR_CAUSE_THRE) ? IIR_REG_8250 : \
		 (IIR_TO_CAUSE(x) == IIR_CAUSE_RXA) ? RBR_REG_8250 : \
		 (IIR_TO_CAUSE(x) == IIR_CAUSE_LSR) ? LSR_REG_8250 : \
		 (IIR_TO_CAUSE(x) == IIR_CAUSE_TIMEOUT) ? RBR_REG_8250 : \
		 SCR_REG_8250)\
		)
		 
	/* FCR stuff */
	#define FCR_ENABLE (1 << 0)
	#define FCR_DISABLE (0 << 0)
	#define FCR_RX_CLR (1 << 1)
	#define FCR_TX_CLR (1 << 2)
	#define FCR_DMA0 (0 << 3)
	#define FCR_DMA1 (1 << 3)
	#define FCR_64_ENABLE (1 << 5)
	#define FCR_RX_INT_1 (0 << 6)
	#define FCR_RX_INT_4 (1 << 6)
	#define FCR_RX_INT_8 (2 << 6)
	#define FCR_RX_INT_14 (3 << 6)
	
	/* LCR stuff */
	#define LCR_WORD_5bit (0 << 0)
	#define LCR_WORD_6bit (1 << 0)
	#define LCR_WORD_7bit (2 << 0)
	#define LCR_WORD_8bit (3 << 0)
	#define LCR_STOP_1bit (0 << 2)
	#define LCR_STOP_WEIRD (1 << 2)
	#define LCR_NO_PARITY (0 << 3)
	#define LCR_ODD_PARITY (1 << 3)
	#define LCR_EVEN_PARITY (3 << 3)
	#define LCR_HIGH_PARITY (5 << 3)
	#define LCR_LOW_PARITY (7 << 3)
	#define LCR_BREAK_EN (1 << 6)
	#define LCR_BREAK_DIS (0 << 6)
	#define LCR_DLAB_ON (1 << 7)
	#define LCR_DLAB_OFF (0 << 7)
	
	/* MCR stuff */
	#define MCR_DTR (1 << 0)
	#define MCR_RTS (1 << 1)
	#define MCR_AO1 (1 << 2)
	#define MCR_AO2 (1 << 3)
	#define MCR_LOOP (1 << 4)
	#define MCR_AUTOFLOW (1 << 5)
	
	/* LSR stuff */
	#define LSR_DATA_AVAIL (1 << 0)
	#define LSR_OVRRN_ERR  (1 << 1)
	#define LSR_PARITY_ERR  (1 << 2)
	#define LSR_FRAMING_ERR  (1 << 3)
	#define LSR_BREAK  (1 << 4)
	#define LSR_THRE  (1 << 5)
	#define LSR_THRE_IDLE  (1 << 6)
	#define LSR_FIFO_ERR  (1 << 7)
	
	/* MSR stuff */
	#define MSR_CTS_CHANGE (1 << 0)
	#define MSR_DSR_CHANGE (1 << 1)
	#define MSR_RING_TE (1 << 2)
	#define MSR_CD_CHANGE (1 << 3)
	#define MSR_CTS (1 << 4)
	#define MSR_DSR (1 << 5)
	#define MSR_RI (1 << 6)
	#define MSR_CD (1 << 7)
#endif
