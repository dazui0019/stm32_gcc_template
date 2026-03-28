# stm32_gcc_template

基于 `SCons + Arm GNU Toolchain` 的 STM32F4 模板工程。当前默认目标为 `stm32f446xx`，构建产物统一输出到 `build/`。

## 当前特性

- `SConstruct` 统一管理工具链、头文件路径、编译参数和链接参数
- 支持通过 `target_mcu` 和 `target_chip` 选择目标
- 提供一个精简版、类似 RT-Thread 的 `DefineGroup` 模块注册机制
- 支持 group 级别的全局导出参数和局部 `LOCAL_*` 编译参数
- 自动生成 `build/compile_commands.json`
- 详细构建说明见 [docs/scons.md](docs/scons.md)

## 目录

```text
.
|-- SConstruct
|-- README.md
|-- docs/
|   `-- scons.md
|-- pyproject.toml
|-- app/
|-- core/
|-- utils/
|-- targets/
|   |-- cmsis_core/
|   `-- stm32f4/
|       |-- SConscript
|       |-- cmsis/
|       |-- hal/
|       `-- links/
|-- toolchain/   # 本地工具链目录，不提交到 Git
`-- build/       # 构建输出目录，不提交到 Git
```

## 环境准备

工具链默认放在：

```text
toolchain/mingw-w64-i686-arm-none-eabi/bin
```

Python 依赖推荐用 `uv` 安装：

```powershell
uv sync
```

## 构建

默认构建：

```powershell
uv run scons
```

指定芯片：

```powershell
uv run scons target_mcu=stm32f4 target_chip=stm32f446xx
uv run scons target_mcu=stm32f4 target_chip=stm32f407xx
```

## 输出文件

```text
build/firmware.elf
build/firmware.bin
build/firmware.map
build/compile_commands.json
```

## 说明

- 当前链接脚本路径规则为 `targets/<target_mcu>/links/<ChipDefine>_FLASH.ld`
- `targets/SConscript` 会根据 `target_mcu` 注册对应目标的启动文件和系统文件
- `drivers/` 和 `bsp/` 目录目前作为后续扩展入口预留
- 当前精简版构建辅助函数位于 `toolchain/building.py`

## TODO List

- [ ] 制作类似RT-Thread的Scons框架。
- [ ] 制作furi like的gpio hal库。
- [ ] 完成对gd32f4的适配。
- [ ] 了解gcc编译器参数。
- [ ] 测试编译后的固件是否可用。
- [ ] 适配pyocd/openocd
