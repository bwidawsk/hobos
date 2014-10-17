void arch_pause() {
	__asm__ volatile ("pause");
}
