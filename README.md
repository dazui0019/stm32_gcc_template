# stm32_gcc_template

基于 `SCons + Arm GNU Toolchain` 的 STM32F4 模板工程。当前默认目标为 `stm32f446xx`，构建产物统一输出到 `build/`。

## 当前特性

- `SConstruct` 统一管理工具链、头文件路径、编译参数和链接参数
- 支持通过 `target_mcu` 和 `target_chip` 选择目标
- 提供一个精简版、类似 RT-Thread 的 `DefineGroup` 模块注册机制
- 支持 group 级别的全局导出参数和局部 `LOCAL_*` 编译参数
- 自动生成 `build/compile_commands.json`
- 详细构建说明见 [docs/scons.md](docs/scons.md)

## 当前状态

- 当前工程优先复用 CubeMX 生成的启动、系统初始化和外设相关代码，作为模板工程的基础可运行版本
- CubeMX 生成代码当前主要放在 `board/cubemx_config/`
- `foo_hal` 仍处于逐步整理和补全阶段，现阶段会先与 CubeMX 生成代码并存使用
- 后续会把通用能力逐步沉淀到 `drivers/foo_hal/`，再逐步减少对 CubeMX 直接生成代码的依赖

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
|-- drivers/
|-- libs/
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
- `drivers/SConscript` 作为父目录分发器，负责加载各个驱动库子目录
- 每个驱动库目录独立维护自己的 `src/`、`inc/` 和 `SConscript`
- `libs/SConscript` 统一接入第三方库
- 普通本地库可在 `libs/<name>/SConscript` 里维护自己的构建规则
- 子模块库在 `libs/SConscript` 的 `SUBMODULE_LIBS` 里手动填写 `name`、`path`、`src` 和 `CPPPATH`
- `bsp/` 目录目前作为后续扩展入口预留
- 当前精简版构建辅助函数位于 `toolchain/building.py`

## 参考项目

- [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
  参考其 `furi` 风格的基础设施设计，例如 `check`、公共宏定义和 HAL 接口组织方式。
- [RT-Thread](https://github.com/RT-Thread/rt-thread)
  参考其基于 `SCons` 的模块化构建思路，以及 `DefineGroup` 风格的模块注册方式。

## TODO List

- [ ] 制作类似RT-Thread的Scons框架。
- [ ] 逐步完善 `foo_hal`，补齐通用 GPIO/HAL 接口能力。
- [ ] 完成对gd32f4的适配。
- [ ] 了解gcc编译器参数。
- [ ] 测试编译后的固件是否可用。
- [ ] 适配pyocd/openocd
