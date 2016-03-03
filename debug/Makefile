ccflags-y +=-I$(srctree)/include/debug
ccflags-y +=-I$(srctree)/include/drivers
ccflags-y +=-I$(srctree)/arch/$(ARCH)/platforms/$(subst $(DQUOTE),,$(CONFIG_PLATFORM))

obj-$(CONFIG_GDB_SERVER) = gdb_server.o
