import os

# Top-level build switches. These can be overridden from the SCons command line.
target_mcu = ARGUMENTS.get('target_mcu', 'stm32f4')
target_chip = ARGUMENTS.get('target_chip', 'stm32f446xx')
board_name = ARGUMENTS.get('board', 'test_board_v1')
# Keep all generated artifacts under a single build tree.
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
# Emit absolute paths so clangd / IDE tooling can resolve files reliably.
env['COMPILATIONDB_USE_ABSPATH'] = True

chip_define = 'STM32F' + target_chip[len('stm32f'):]
linker_script = os.path.join('targets', target_mcu, 'links', f'{chip_define}_FLASH.ld')
map_file = os.path.join(build_dir, 'firmware.map')

# Project-wide include roots shared by every module SConscript.
env.AppendUnique(
    CPPPATH=[
        '#/drivers/hal/include',
        '#/core',
        f'#/targets/{target_mcu}/hal/Inc',
        f'#/targets/{target_mcu}/cmsis/Include',
        '#/targets/cmsis_core',
    ]
)

env.Append(
    CPPDEFINES=[chip_define, 'USE_LL_DRIVER'],
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

Export('env', 'target_chip')

sources = []

# Each module contributes source files only; the top-level script owns object output paths.
for script in [
    os.path.join('app', 'SConscript'),
    os.path.join('core', 'SConscript'),
    os.path.join('utils', 'SConscript'),
    os.path.join('drivers', 'hal', target_mcu, 'SConscript'),
    os.path.join('targets', target_mcu, 'SConscript'),
    os.path.join('bsp', board_name, 'SConscript'),
]:
    if os.path.exists(script):
        sources += SConscript(script)

objects = []

# Mirror the source tree under build/ so intermediate .o files never pollute source folders.
for source in sources:
    source_path = source.srcnode().path
    object_path = os.path.join(
        build_dir,
        os.path.splitext(source_path)[0] + env['OBJSUFFIX'],
    )
    objects += env.Object(object_path, source)

# Final default outputs: ELF, raw binary image, and compile database for editor tooling.
elf = env.Program(os.path.join(build_dir, 'firmware.elf'), objects)
firmware_bin = env.Command(
    os.path.join(build_dir, 'firmware.bin'),
    elf,
    '$OBJCOPY -O binary $SOURCE $TARGET',
)
compdb = env.CompilationDatabase(os.path.join(build_dir, 'compile_commands.json'))

Default([elf, firmware_bin, compdb])
