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
	{ 0xf24b3dfe, __VMLINUX_SYMBOL_STR(__ioremap) },
	{ 0x67138285, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x96a67a26, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0xf9c329e0, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x2d1451c, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x45a55ec8, __VMLINUX_SYMBOL_STR(__iounmap) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1fdc7df2, __VMLINUX_SYMBOL_STR(_mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cled-device-zhan");
MODULE_ALIAS("of:N*T*Cled-device-zhanC*");
