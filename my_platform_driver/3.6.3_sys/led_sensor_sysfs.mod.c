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
	{ 0x12abd86a, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x72077955, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0xa53750ed, __VMLINUX_SYMBOL_STR(devm_gpio_request) },
	{ 0xbc64fe4e, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xcac00ef, __VMLINUX_SYMBOL_STR(sysfs_create_file_ns) },
	{ 0xef70ba75, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x3908fab5, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xa96f3eed, __VMLINUX_SYMBOL_STR(sysfs_remove_file_ns) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x8c1eaea6, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xaf6419d5, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x99c25d19, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Csys_rw_led_sensor");
MODULE_ALIAS("of:N*T*Csys_rw_led_sensorC*");
