ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
# Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += ../realview-pbx-a9/include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_libcpu_HEADERS =  ../../../libcpu/arm/shared/include/arm-cp15.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../../libbsp/shared/include/mm.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../shared/include/linker-symbols.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/include/arm-cp15-start.h
include_bsp_HEADERS += ../shared/include/arm-errata.h
include_bsp_HEADERS += ../shared/include/arm-gic.h
include_bsp_HEADERS += ../shared/include/arm-gic-irq.h
include_bsp_HEADERS += ../shared/include/arm-gic-regs.h
include_bsp_HEADERS += ../shared/include/arm-gic-tm27.h
include_bsp_HEADERS += ../shared/include/arm-release-id.h
include_bsp_HEADERS += ../shared/include/arm-cache-l1.h
include_bsp_HEADERS += ../shared/armv467ar-basic-cache/cache_.h

include_bsp_HEADERS += include/car.h
include_bsp_HEADERS += include/console.h
if BSP_START_WITH_CMDLINE
include_bsp_HEADERS += include/cmdline.h
endif
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/memory.h
include_bsp_HEADERS += include/mmu.h

###############################################################################
# Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += ../shared/startup/linkcmds.base

###############################################################################
# LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c
if BSP_START_IN_HYP_SUPPORT
libbsp_a_SOURCES += ../shared/startup/bsp-start-in-hyp-support.S
endif

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

# Clock and reset
libbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h
libbsp_a_SOURCES += clock/car.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console.c
libbsp_a_SOURCES += console/debug-console.c

# IRQ
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-gic-irq.c
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c

# Restart
libbsp_a_SOURCES += startup/bspreset.c

# Startup
libbsp_a_SOURCES += startup/bspstart.c
if HAS_SMP
libbsp_a_SOURCES += ../shared/arm-a9mpcore-smp.c
endif

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

# LIBMM
libbsp_a_SOURCES += startup/mm_config_table.c
libbsp_a_SOURCES += ../shared/mminit.c

# misc
if BSP_START_WITH_CMDLINE
libbsp_a_SOURCES += misc/cmdline.c
endif
libbsp_a_SOURCES += misc/timer.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
