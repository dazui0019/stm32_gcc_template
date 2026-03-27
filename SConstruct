import os

target_mcu = ARGUMENTS.get('target_mcu', 'stm32f4')
target_chip = ARGUMENTS.get('target_chip', 'stm32f446xx')
board_name = ARGUMENTS.get('board', 'test_board_v1')

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

env = Environment(
    tools=['gcc', 'gnulink', 'gas'],
    CC=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    AS=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    LINK=os.path.join(toolchain_bin, 'arm-none-eabi-gcc.exe'),
    OBJCOPY=os.path.join(toolchain_bin, 'arm-none-eabi-objcopy.exe'),
    OBJSUFFIX='.o',
    PROGSUFFIX='.elf',
)

core_flags = ['-mcpu=cortex-m4', '-mthumb', '-mfpu=fpv4-sp-d16', '-mfloat-abi=hard']
chip_define = 'STM32F' + target_chip[len('stm32f'):]
linker_script = os.path.join('targets', target_mcu, 'link', f'{chip_define}_FLASH.ld')

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
    CCFLAGS=core_flags + ['-O2', '-g', '-Wall'],
    ASFLAGS=core_flags,
    LINKFLAGS=core_flags + ['-T', linker_script, '--specs=nano.specs', '-Wl,--gc-sections'],
)

Export('env', 'target_chip')

objs = []

for script in [
    os.path.join('app', 'SConscript'),
    os.path.join('core', 'SConscript'),
    os.path.join('utils', 'SConscript'),
    os.path.join('drivers', 'hal', target_mcu, 'SConscript'),
    os.path.join('targets', target_mcu, 'SConscript'),
    os.path.join('bsp', board_name, 'SConscript'),
]:
    if os.path.exists(script):
        objs += SConscript(script)

elf = env.Program('firmware.elf', objs)
env.Command('firmware.bin', elf, '$OBJCOPY -O binary $SOURCE $TARGET')
