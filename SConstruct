import os
import sys
from SCons.Script import Action
from scripts.cubemx_tool import *
from scripts.building import *

# Top-level build switches. These can be overridden from the SCons command line.
target_mcu = ARGUMENTS.get('target_mcu', 'stm32f4')
target_chip = ARGUMENTS.get('target_chip', 'stm32f446xx')
board_name = ARGUMENTS.get('board', 'test_board_v1')
build_dir = 'build'
verbose_arg = str(ARGUMENTS.get('verbose', ARGUMENTS.get('V', '0'))).lower()
verbose_build = verbose_arg not in ('', '0', 'false', 'no')

toolchain_bin = os.path.abspath(
    os.path.join(
        'toolchain',
        'mingw-w64-x86_64-arm-none-eabi',
        'bin',
    )
)

if target_mcu != 'stm32f4':
    Exit('Unsupported target: %s' % target_mcu)

if not target_chip.startswith('stm32f'):
    Exit('Unsupported target_chip: %s' % target_chip)

# Use GCC as the compiler/link driver, and GNU as for raw .s startup files.
env = Environment(
    tools=['gcc', 'g++', 'gnulink', 'ar', 'as'],
    CC=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    AS=os.path.join(toolchain_bin, 'arm-none-eabi-as.exe'),
    AR=os.path.join(toolchain_bin, 'arm-none-eabi-ar.exe'),
    LINK=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    OBJCOPY=os.path.join(toolchain_bin, 'arm-none-eabi-objcopy.exe'),
    OBJSUFFIX='.o',
    PROGSUFFIX='.elf',
)

if not verbose_build:
    env.Replace(
        CCCOMSTR='CC      $SOURCE',
        CXXCOMSTR='CXX     $SOURCE',
        ASCOMSTR='AS      $SOURCE',
        ASPPCOMSTR='AS      $SOURCE',
        ARCOMSTR='AR      $TARGET',
        RANLIBCOMSTR='RANLIB  $TARGET',
        LINKCOMSTR='LD      $TARGET',
        SHCCCOMSTR='SHCC    $SOURCE',
        SHCXXCOMSTR='SHCXX   $SOURCE',
        SHLINKCOMSTR='SHLD    $TARGET',
        COMPILATIONDB_COMSTR=None,
    )

env.Tool('compilation_db')
env['COMPILATIONDB_USE_ABSPATH'] = True

chip_define = 'STM32F' + target_chip[len('stm32f'):]
linker_script = os.path.join('targets', target_mcu, 'links', f'{chip_define}_FLASH.ld')
map_file = os.path.join(build_dir, 'firmware.map')

env.Append(
    CPPDEFINES=[chip_define, 'USE_FULL_LL_DRIVER', 'USE_FULL_HAL_DRIVER'],
)

# C compilation flags for all project sources.
env.Append( CCFLAGS = [
    '-mthumb',
    '-mcpu=cortex-m4',
    '-mfloat-abi=hard',
    '-mfpu=fpv4-sp-d16',
    '-std=gnu11',
    '-Ofast',
    '-ffunction-sections',
    '-fdata-sections',
    '-g',
    '-Wall',
    '-specs=nano.specs',
])

# Assembly flags for startup and low-level .s files.
env.Append( ASFLAGS = [
    '-mcpu=cortex-m4',
    '-mthumb',
    '-mfpu=fpv4-sp-d16',
    '-mfloat-abi=hard',
])

# Link-time settings: linker script, dead-code stripping, map file and C runtime.
env.Append( LINKFLAGS = [
	'-mthumb',
    '-mcpu=cortex-m4',
    '-mfloat-abi=hard',
    '-mfpu=fpv4-sp-d16',
    '-specs=nosys.specs',
    '-static',
    '-Wl,-cref,-u,Reset_Handler',
    f'-Wl,-Map={map_file}',
    '-Wl,--gc-sections',
    '-Wl,--defsym=malloc_getpagesize_P=0x80',
    '-Wl,--start-group',
    '-lc',
    '-lm',
    '-Wl,--end-group',
    '-specs=nano.specs',
    '-Wl,--print-memory-usage',
    '-T' + linker_script,
]) 

Export('env', 'target_chip', 'target_mcu')

# 执行 CubeMX 生成代码
generate_with_cubemx(".\\board\\cubemx_config\\cubemx_config.ioc")

# 寻找子脚本
for script in [
    os.path.join('core', 'SConscript'),
    os.path.join('utils', 'SConscript'),
    os.path.join('libs', 'SConscript'),
    os.path.join('drivers', 'SConscript'),
    os.path.join('app', 'SConscript'),
    os.path.join('targets', 'SConscript'),
    os.path.join('board', 'SConscript'),
]:
    if os.path.exists(script):
        SConscript(script)

targets = BuildFirmware(env, build_dir, groups=GetProjects())
elf = targets[0]
targets.append(
    env.Command(
        os.path.join(build_dir, 'firmware.hex'),
        elf,
        Action(
            '$OBJCOPY -O ihex $SOURCE $TARGET',
            cmdstr=None,
        ),
    )
)
Default(targets)
