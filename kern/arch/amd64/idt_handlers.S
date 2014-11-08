#include "include/irq.h" // AKA <arch/irq.h
#include "idt_common.h"

#define STORE_ALL_REGS \
	movq %rax, TF_RAX_OFF(%rsp) ;\
	movq %rbx, TF_RBX_OFF(%rsp) ;\
	movq %rcx, TF_RCX_OFF(%rsp) ;\
	movq %rdx, TF_RDX_OFF(%rsp) ;\
	movq %rsi, TF_RDI_OFF(%rsp) ;\
	movq %rdi, TF_RSI_OFF(%rsp) ;\
	movq %rbp, TF_RBP_OFF(%rsp) ;\
	movq %rsp, TF_RSP_OFF(%rsp) ;\
	movq %r8, TF_R8_OFF(%rsp) ;\
	movq %r9, TF_R9_OFF(%rsp) ;\
	movq %r10, TF_R10_OFF(%rsp) ;\
	movq %r11, TF_R11_OFF(%rsp) ;\
	movq %r12, TF_R12_OFF(%rsp) ;\
	movq %r13, TF_R13_OFF(%rsp) ;\
	movq %r14, TF_R14_OFF(%rsp) ;\
	movq %r15, TF_R15_OFF(%rsp) ;\

#define UNSTORE_ALL_REGS \
	xchg %rax, TF_RAX_OFF(%rsp) ;\
	xchg %rbx, TF_RBX_OFF(%rsp) ;\
	xchg %rcx, TF_RCX_OFF(%rsp) ;\
	xchg %rdx, TF_RDX_OFF(%rsp) ;\
	xchg %rsi, TF_RDI_OFF(%rsp) ;\
	xchg %rdi, TF_RSI_OFF(%rsp) ;\
	xchg %rbp, TF_RBP_OFF(%rsp) ;\
	xchg %rsp, TF_RSP_OFF(%rsp) ;\
	xchg %r8, TF_R8_OFF(%rsp) ;\
	xchg %r9, TF_R9_OFF(%rsp) ;\
	xchg %r10, TF_R10_OFF(%rsp) ;\
	xchg %r11, TF_R11_OFF(%rsp) ;\
	xchg %r12, TF_R12_OFF(%rsp) ;\
	xchg %r13, TF_R13_OFF(%rsp) ;\
	xchg %r14, TF_R14_OFF(%rsp) ;\
	xchg %r15, TF_R15_OFF(%rsp) ;\

#define TRAP_DEF(x)    \
	STORE_ALL_REGS \
	movq $(x), TF_WHICH(%rsp) ;\
	addq $TF_WHICH, %rsp /* remember TF_WHICH is negative */;\
    jmp default_handler

#define TRAP_DEF_NO_ERR_PUSH(x)    \
	pushq $0 ; \
	STORE_ALL_REGS \
	movq $(x), TF_WHICH(%rsp) ;\
	addq $TF_WHICH, %rsp /* remember TF_WHICH is negative */;\
    jmp default_handler

.globl generic_handler
generic_handler:
	movq $-1, %rdi ;\
	call generic_c_handler ;\
	jmp . ;\
	iretq

default_handler:
	movq %rsp, %rdi ;\
	callq dflt_c_handler ;\
	subq $TF_WHICH, %rsp /* remember TF_WHICH is negative */;\
	UNSTORE_ALL_REGS ;\
	iretq

IDTVEC(T_DIVIDE_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_DIVIDE_FAULT)
IDTVEC(T_DEBUG)
	TRAP_DEF_NO_ERR_PUSH(T_DEBUG)
IDTVEC(T_NMI_INT)
	TRAP_DEF_NO_ERR_PUSH(T_NMI_INT)
IDTVEC(T_BKPT_TRAP)
	TRAP_DEF_NO_ERR_PUSH(T_BKPT_TRAP)
IDTVEC(T_OVRFLW_TRAP)
	TRAP_DEF_NO_ERR_PUSH(T_OVRFLW_TRAP)
IDTVEC(T_BOUND_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_BOUND_FAULT)
IDTVEC(T_UNDEF_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_UNDEF_FAULT)
IDTVEC(T_NOMATH_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_NOMATH_FAULT)
IDTVEC(T_DOUBLE_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_DOUBLE_FAULT)
IDTVEC(T_COPROC_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_COPROC_FAULT)
IDTVEC(T_TSS_FAULT)
	TRAP_DEF(T_TSS_FAULT)
IDTVEC(T_SEG_FAULT)
	TRAP_DEF(T_SEG_FAULT)
IDTVEC(T_STACK_FAULT)
	TRAP_DEF(T_STACK_FAULT)
IDTVEC(T_GP_FAULT)
	TRAP_DEF(T_GP_FAULT)
IDTVEC(T_PAGE_FAULT)
	TRAP_DEF(T_PAGE_FAULT)
IDTVEC(T_RSVD)
	TRAP_DEF(T_RSVD)
IDTVEC(T_MATH_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_MATH_FAULT)
IDTVEC(T_ALIGN_FAULT)
	TRAP_DEF(T_ALIGN_FAULT)
IDTVEC(T_MACH_ABORT)
	TRAP_DEF_NO_ERR_PUSH(T_MACH_ABORT)
IDTVEC(T_SIMD_FAULT)
	TRAP_DEF(T_SIMD_FAULT)

// Create an IRQ handler for all possible external
// interrupts, even though we won't use all
#define IRQ_HANDLER(index, vec_name) \
IDTVEC(vec_name) \
	STORE_ALL_REGS ;\
	addq $TF_WHICH, %rsp /* remember TF_WHICH is negative */;\
	movq $(index - IRQ_EXTERNAL), %rdi ;\
	call    generic_c_handler ;\
	subq $TF_WHICH, %rsp /* remember TF_WHICH is negative */;\
	UNSTORE_ALL_REGS ;\
	iretq

// TODO: make below macro work
.macro setup_all_irqs from=IRQ_EXTERNAL, to=IRQ_MAX
     IRQ_HANDLER(\from, irq_handler_\from)
     .if \to-\from
     setup_all_irqs "(\from+1)",\to
     .endif
.endm
// setup_all_irqs
// I made the following with a VIM macro
IRQ_HANDLER(32, irq_vector_32);
IRQ_HANDLER(33, irq_vector_33);
IRQ_HANDLER(34, irq_vector_34);
IRQ_HANDLER(35, irq_vector_35);
IRQ_HANDLER(36, irq_vector_36);
IRQ_HANDLER(37, irq_vector_37);
IRQ_HANDLER(38, irq_vector_38);
IRQ_HANDLER(39, irq_vector_39);
IRQ_HANDLER(40, irq_vector_40);
IRQ_HANDLER(41, irq_vector_41);
IRQ_HANDLER(42, irq_vector_42);
IRQ_HANDLER(43, irq_vector_43);
IRQ_HANDLER(44, irq_vector_44);
IRQ_HANDLER(45, irq_vector_45);
IRQ_HANDLER(46, irq_vector_46);
IRQ_HANDLER(47, irq_vector_47);
IRQ_HANDLER(48, irq_vector_48);
IRQ_HANDLER(49, irq_vector_49);
IRQ_HANDLER(50, irq_vector_50);
IRQ_HANDLER(51, irq_vector_51);
IRQ_HANDLER(52, irq_vector_52);
IRQ_HANDLER(53, irq_vector_53);
IRQ_HANDLER(54, irq_vector_54);
IRQ_HANDLER(55, irq_vector_55);
IRQ_HANDLER(56, irq_vector_56);
IRQ_HANDLER(57, irq_vector_57);
IRQ_HANDLER(58, irq_vector_58);
IRQ_HANDLER(59, irq_vector_59);
IRQ_HANDLER(60, irq_vector_60);
IRQ_HANDLER(61, irq_vector_61);
IRQ_HANDLER(62, irq_vector_62);
IRQ_HANDLER(63, irq_vector_63);
IRQ_HANDLER(64, irq_vector_64);
IRQ_HANDLER(65, irq_vector_65);
IRQ_HANDLER(66, irq_vector_66);
IRQ_HANDLER(67, irq_vector_67);
IRQ_HANDLER(68, irq_vector_68);
IRQ_HANDLER(69, irq_vector_69);
IRQ_HANDLER(70, irq_vector_70);
IRQ_HANDLER(71, irq_vector_71);
IRQ_HANDLER(72, irq_vector_72);
IRQ_HANDLER(73, irq_vector_73);
IRQ_HANDLER(74, irq_vector_74);
IRQ_HANDLER(75, irq_vector_75);
IRQ_HANDLER(76, irq_vector_76);
IRQ_HANDLER(77, irq_vector_77);
IRQ_HANDLER(78, irq_vector_78);
IRQ_HANDLER(79, irq_vector_79);
IRQ_HANDLER(80, irq_vector_80);
IRQ_HANDLER(81, irq_vector_81);
IRQ_HANDLER(82, irq_vector_82);
IRQ_HANDLER(83, irq_vector_83);
IRQ_HANDLER(84, irq_vector_84);
IRQ_HANDLER(85, irq_vector_85);
IRQ_HANDLER(86, irq_vector_86);
IRQ_HANDLER(87, irq_vector_87);
IRQ_HANDLER(88, irq_vector_88);
IRQ_HANDLER(89, irq_vector_89);
IRQ_HANDLER(90, irq_vector_90);
IRQ_HANDLER(91, irq_vector_91);
IRQ_HANDLER(92, irq_vector_92);
IRQ_HANDLER(93, irq_vector_93);
IRQ_HANDLER(94, irq_vector_94);
IRQ_HANDLER(95, irq_vector_95);
IRQ_HANDLER(96, irq_vector_96);
IRQ_HANDLER(97, irq_vector_97);
IRQ_HANDLER(98, irq_vector_98);
IRQ_HANDLER(99, irq_vector_99);
IRQ_HANDLER(100, irq_vector_100);
IRQ_HANDLER(101, irq_vector_101);
IRQ_HANDLER(102, irq_vector_102);
IRQ_HANDLER(103, irq_vector_103);
IRQ_HANDLER(104, irq_vector_104);
IRQ_HANDLER(105, irq_vector_105);
IRQ_HANDLER(106, irq_vector_106);
IRQ_HANDLER(107, irq_vector_107);
IRQ_HANDLER(108, irq_vector_108);
IRQ_HANDLER(109, irq_vector_109);
IRQ_HANDLER(110, irq_vector_110);
IRQ_HANDLER(111, irq_vector_111);
IRQ_HANDLER(112, irq_vector_112);
IRQ_HANDLER(113, irq_vector_113);
IRQ_HANDLER(114, irq_vector_114);
IRQ_HANDLER(115, irq_vector_115);
IRQ_HANDLER(116, irq_vector_116);
IRQ_HANDLER(117, irq_vector_117);
IRQ_HANDLER(118, irq_vector_118);
IRQ_HANDLER(119, irq_vector_119);
IRQ_HANDLER(120, irq_vector_120);
IRQ_HANDLER(121, irq_vector_121);
IRQ_HANDLER(122, irq_vector_122);
IRQ_HANDLER(123, irq_vector_123);
IRQ_HANDLER(124, irq_vector_124);
IRQ_HANDLER(125, irq_vector_125);
IRQ_HANDLER(126, irq_vector_126);
IRQ_HANDLER(127, irq_vector_127);
IRQ_HANDLER(128, irq_vector_128);
IRQ_HANDLER(129, irq_vector_129);
IRQ_HANDLER(130, irq_vector_130);
IRQ_HANDLER(131, irq_vector_131);
IRQ_HANDLER(132, irq_vector_132);
IRQ_HANDLER(133, irq_vector_133);
IRQ_HANDLER(134, irq_vector_134);
IRQ_HANDLER(135, irq_vector_135);
IRQ_HANDLER(136, irq_vector_136);
IRQ_HANDLER(137, irq_vector_137);
IRQ_HANDLER(138, irq_vector_138);
IRQ_HANDLER(139, irq_vector_139);
IRQ_HANDLER(140, irq_vector_140);
IRQ_HANDLER(141, irq_vector_141);
IRQ_HANDLER(142, irq_vector_142);
IRQ_HANDLER(143, irq_vector_143);
IRQ_HANDLER(144, irq_vector_144);
IRQ_HANDLER(145, irq_vector_145);
IRQ_HANDLER(146, irq_vector_146);
IRQ_HANDLER(147, irq_vector_147);
IRQ_HANDLER(148, irq_vector_148);
IRQ_HANDLER(149, irq_vector_149);
IRQ_HANDLER(150, irq_vector_150);
IRQ_HANDLER(151, irq_vector_151);
IRQ_HANDLER(152, irq_vector_152);
IRQ_HANDLER(153, irq_vector_153);
IRQ_HANDLER(154, irq_vector_154);
IRQ_HANDLER(155, irq_vector_155);
IRQ_HANDLER(156, irq_vector_156);
IRQ_HANDLER(157, irq_vector_157);
IRQ_HANDLER(158, irq_vector_158);
IRQ_HANDLER(159, irq_vector_159);
IRQ_HANDLER(160, irq_vector_160);
IRQ_HANDLER(161, irq_vector_161);
IRQ_HANDLER(162, irq_vector_162);
IRQ_HANDLER(163, irq_vector_163);
IRQ_HANDLER(164, irq_vector_164);
IRQ_HANDLER(165, irq_vector_165);
IRQ_HANDLER(166, irq_vector_166);
IRQ_HANDLER(167, irq_vector_167);
IRQ_HANDLER(168, irq_vector_168);
IRQ_HANDLER(169, irq_vector_169);
IRQ_HANDLER(170, irq_vector_170);
IRQ_HANDLER(171, irq_vector_171);
IRQ_HANDLER(172, irq_vector_172);
IRQ_HANDLER(173, irq_vector_173);
IRQ_HANDLER(174, irq_vector_174);
IRQ_HANDLER(175, irq_vector_175);
IRQ_HANDLER(176, irq_vector_176);
IRQ_HANDLER(177, irq_vector_177);
IRQ_HANDLER(178, irq_vector_178);
IRQ_HANDLER(179, irq_vector_179);
IRQ_HANDLER(180, irq_vector_180);
IRQ_HANDLER(181, irq_vector_181);
IRQ_HANDLER(182, irq_vector_182);
IRQ_HANDLER(183, irq_vector_183);
IRQ_HANDLER(184, irq_vector_184);
IRQ_HANDLER(185, irq_vector_185);
IRQ_HANDLER(186, irq_vector_186);
IRQ_HANDLER(187, irq_vector_187);
IRQ_HANDLER(188, irq_vector_188);
IRQ_HANDLER(189, irq_vector_189);
IRQ_HANDLER(190, irq_vector_190);
IRQ_HANDLER(191, irq_vector_191);
IRQ_HANDLER(192, irq_vector_192);
IRQ_HANDLER(193, irq_vector_193);
IRQ_HANDLER(194, irq_vector_194);
IRQ_HANDLER(195, irq_vector_195);
IRQ_HANDLER(196, irq_vector_196);
IRQ_HANDLER(197, irq_vector_197);
IRQ_HANDLER(198, irq_vector_198);
IRQ_HANDLER(199, irq_vector_199);
IRQ_HANDLER(200, irq_vector_200);
IRQ_HANDLER(201, irq_vector_201);
IRQ_HANDLER(202, irq_vector_202);
IRQ_HANDLER(203, irq_vector_203);
IRQ_HANDLER(204, irq_vector_204);
IRQ_HANDLER(205, irq_vector_205);
IRQ_HANDLER(206, irq_vector_206);
IRQ_HANDLER(207, irq_vector_207);
IRQ_HANDLER(208, irq_vector_208);
IRQ_HANDLER(209, irq_vector_209);
IRQ_HANDLER(210, irq_vector_210);
IRQ_HANDLER(211, irq_vector_211);
IRQ_HANDLER(212, irq_vector_212);
IRQ_HANDLER(213, irq_vector_213);
IRQ_HANDLER(214, irq_vector_214);
IRQ_HANDLER(215, irq_vector_215);
IRQ_HANDLER(216, irq_vector_216);
IRQ_HANDLER(217, irq_vector_217);
IRQ_HANDLER(218, irq_vector_218);
IRQ_HANDLER(219, irq_vector_219);
IRQ_HANDLER(220, irq_vector_220);
IRQ_HANDLER(221, irq_vector_221);
IRQ_HANDLER(222, irq_vector_222);
IRQ_HANDLER(223, irq_vector_223);
IRQ_HANDLER(224, irq_vector_224);
IRQ_HANDLER(225, irq_vector_225);
IRQ_HANDLER(226, irq_vector_226);
IRQ_HANDLER(227, irq_vector_227);
IRQ_HANDLER(228, irq_vector_228);
IRQ_HANDLER(229, irq_vector_229);
IRQ_HANDLER(230, irq_vector_230);
IRQ_HANDLER(231, irq_vector_231);
IRQ_HANDLER(232, irq_vector_232);
IRQ_HANDLER(233, irq_vector_233);
IRQ_HANDLER(234, irq_vector_234);
IRQ_HANDLER(235, irq_vector_235);
IRQ_HANDLER(236, irq_vector_236);
IRQ_HANDLER(237, irq_vector_237);
IRQ_HANDLER(238, irq_vector_238);
IRQ_HANDLER(239, irq_vector_239);
IRQ_HANDLER(240, irq_vector_240);
IRQ_HANDLER(241, irq_vector_241);
IRQ_HANDLER(242, irq_vector_242);
IRQ_HANDLER(243, irq_vector_243);
IRQ_HANDLER(244, irq_vector_244);
IRQ_HANDLER(245, irq_vector_245);
IRQ_HANDLER(246, irq_vector_246);
IRQ_HANDLER(247, irq_vector_247);
IRQ_HANDLER(248, irq_vector_248);
IRQ_HANDLER(249, irq_vector_249);
IRQ_HANDLER(250, irq_vector_250);
IRQ_HANDLER(251, irq_vector_251);
IRQ_HANDLER(252, irq_vector_252);
IRQ_HANDLER(253, irq_vector_253);
IRQ_HANDLER(254, irq_vector_254);
IRQ_HANDLER(255, irq_vector_255);
