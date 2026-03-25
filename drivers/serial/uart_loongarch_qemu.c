/*
 * LoongArch64 QEMU UART (MMIO, polling)
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <soc.h>
#include <uart.h>
#include <sys_io.h>

#define DEV_CFG(dev) \
	((const struct uart_device_config * const)(dev)->config->config_info)

/* Line control / status bits for 16550-like UART */
#define LCR_DLAB	0x80
#define LCR_8N1		0x03
#define LSR_THRE	0x20
#define LSR_DR		0x01

static void uart_loongarch_hw_init(uintptr_t base)
{
	/* 1. Disable UART interrupts */
	sys_write8(0x00, base + UART_IER);

	/* 2. Enable DLAB to set baud rate */
	sys_write8(LCR_DLAB, base + UART_LCR);

	/* 3. Set baud rate divisor = 0x0012 (115200 @ 33MHz) */
	sys_write8(0x12, base + UART_DLL);
	sys_write8(0x00, base + UART_DLH);

	/* 4. 8 data bits, no parity, 1 stop bit, disable DLAB */
	sys_write8(LCR_8N1, base + UART_LCR);

	/* 5. Enable FIFO, clear RX/TX FIFO */
	sys_write8(0x07, base + UART_FCR);

	/* 6. Polling mode: keep interrupts disabled */
	sys_write8(0x00, base + UART_IER);
}

static unsigned char uart_loongarch_qemu_poll_out(struct device *dev,
					      unsigned char c)
{
	uintptr_t base = DEV_CFG(dev)->regs;

	while ((sys_read8(base + UART_LSR) & LSR_THRE) == 0) {
		;
	}

	sys_write8(c, base + UART_THR);
	return c;
}

static int uart_loongarch_qemu_poll_in(struct device *dev, unsigned char *c)
{
	uintptr_t base = DEV_CFG(dev)->regs;

	if (sys_read8(base + UART_LSR) & LSR_DR) {
		*c = sys_read8(base + UART_RHR);
		return 0;
	}

	return -1;
}

static int uart_loongarch_qemu_init(struct device *dev)
{
	uart_loongarch_hw_init(DEV_CFG(dev)->regs);
	return 0;
}

static const struct uart_driver_api uart_loongarch_qemu_driver_api = {
	.poll_in = uart_loongarch_qemu_poll_in,
	.poll_out = uart_loongarch_qemu_poll_out,
	.err_check = NULL,
};

static const struct uart_device_config uart_loongarch_qemu_dev_cfg_0 = {
	.regs = UART_BASE,
};

DEVICE_AND_API_INIT(uart_loongarch_qemu_0, CONFIG_UART_CONSOLE_ON_DEV_NAME,
		    uart_loongarch_qemu_init, NULL,
		    &uart_loongarch_qemu_dev_cfg_0,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    (void *)&uart_loongarch_qemu_driver_api);
