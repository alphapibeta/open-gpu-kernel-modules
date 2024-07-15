#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd31aec96, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x1fdc7df2, "_mcount" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xb43f9365, "ktime_get" },
	{ 0xb0a8579b, "device_destroy" },
	{ 0x13787c35, "__register_chrdev" },
	{ 0xdcb764ad, "memset" },
	{ 0x4b0a3f52, "gic_nonsecure_priorities" },
	{ 0xc5850110, "printk" },
	{ 0xace17747, "class_unregister" },
	{ 0x22431a9f, "device_create" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0x49f2fa43, "cpu_hwcaps" },
	{ 0xf424b0fe, "cpu_hwcap_keys" },
	{ 0x3eb58e34, "class_destroy" },
	{ 0x14b89635, "arm64_const_caps_ready" },
	{ 0x9dcd5c26, "__class_create" },
	{ 0x88db9f48, "__check_object_size" },
};

MODULE_INFO(depends, "");

