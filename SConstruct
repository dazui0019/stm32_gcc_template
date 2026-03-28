import os
import sys

# get building.py
cwd = Dir('#').abspath
sys.path.insert(0, os.path.join(cwd, 'toolchain'))

try:
    from building import *
except:
    print('Cannot found toolchain directory')
    exit(-1)

# Top-level build switches. These can be overridden from the SCons command line.
target_mcu = ARGUMENTS.get('target_mcu', 'stm32f4')
target_chip = ARGUMENTS.get('target_chip', 'stm32f446xx')
board_name = ARGUMENTS.get('board', 'test_board_v1')
build_dir = 'build'

toolchain_bin = os.path.abspath(
    # os.path.join()用来拼接路径
    os.path.join(
        'toolchain',
        'mingw-w64-i686-arm-none-eabi',
        'bin',
    )
)

if target_mcu != 'stm32f4':
    Exit('Unsupported target: %s' % target_mcu)

if not target_chip.startswith('stm32f'):
    Exit('Unsupported target_chip: %s' % target_chip)

# Use GCC as the compiler/link driver, and GNU as for raw .s startup files.
env = Environment(
    # tools=['default', 'ninja'],
    CC=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    AS=os.path.join(toolchain_bin, 'arm-none-eabi-as.exe'),
    AR=os.path.join(toolchain_bin, 'arm-none-eabi-ar.exe'),
    LINK=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    OBJCOPY=os.path.join(toolchain_bin, 'arm-none-eabi-objcopy.exe'),
    OBJSUFFIX='.o',
    PROGSUFFIX='.elf',
)

env.Tool('compilation_db')
env['COMPILATIONDB_USE_ABSPATH'] = True

chip_define = 'STM32F' + target_chip[len('stm32f'):]
linker_script = os.path.join('targets', target_mcu, 'links', f'{chip_define}_FLASH.ld')
map_file = os.path.join(build_dir, 'firmware.map')

env.Append(
    CPPDEFINES=[chip_define, 'USE_FULL_LL_DRIVER'],
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

for script in [
    os.path.join('core', 'SConscript'),
    os.path.join('utils', 'SConscript'),
    os.path.join('drivers', 'SConscript'),
    os.path.join('app', 'SConscript'),
    os.path.join('targets', 'SConscript'),
]:
    if os.path.exists(script):
        SConscript(script)

targets = BuildFirmware(env, build_dir, groups=GetProjects())
Default(targets)
