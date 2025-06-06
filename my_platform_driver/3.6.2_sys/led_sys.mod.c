#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc2996440, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa54048cf, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x2b71ee58, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x630fbfc5, __VMLINUX_SYMBOL_STR(device_create_file) },
	{ 0xa53750ed, __VMLINUX_SYMBOL_STR(devm_gpio_request) },
	{ 0xef70ba75, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0xaf6419d5, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x99c25d19, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xf3d6d4ea, __VMLINUX_SYMBOL_STR(device_remove_file) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Csys_rw_led");
MODULE_ALIAS("of:N*T*Csys_rw_ledC*");
