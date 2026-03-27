# stm32_gcc_template

基于 `SCons + Arm GNU Toolchain` 的 MCU 工程模板，当前以 `STM32F4 + CMSIS + LL` 为主，构建脚本已经支持按 `target_mcu` 和 `target_chip` 选择目标系列和具体芯片型号。

## 当前目标

- 用 `SConstruct` 统一管理工具链、全局头文件路径、编译参数和链接参数
- 用各目录下的 `SConscript` 负责本目录源码收集
- 让 `target_mcu` 控制系列级资源
- 让 `target_chip` 控制具体芯片的启动文件、芯片宏和链接脚本
- 当前按 `LL` 方式组织底层驱动使用

## 目录说明

```text
.
|-- SConstruct
|-- app/                  # 应用层代码
|-- core/                 # 公共核心代码
|-- utils/                # 通用工具代码
|-- bsp/                  # 板级支持包，当前预留
|-- drivers/              # 驱动抽象层，当前预留
|-- targets/
|   |-- cmsis_core/       # 通用 CMSIS Core 头文件
|   |-- stm32f4/          # STM32F4 目标相关文件
|   |   |-- cmsis/
|   |   |-- hal/
|   |   `-- link/         # 链接脚本目录
|   `-- gd32f4/           # 预留目录
`-- toolchain/            # 本地工具链目录，已被 .gitignore 忽略
```

## 构建参数

当前顶层构建脚本读取以下参数：

- `target_mcu`：目标系列，默认 `stm32f4`
- `target_chip`：具体芯片，默认 `stm32f446xx`
- `board`：板级目录名，默认 `test_board_v1`

示例：

```powershell
scons
scons target_mcu=stm32f4 target_chip=stm32f446xx
scons target_mcu=stm32f4 target_chip=stm32f407xx
```

## 工具链

当前 `SConstruct` 默认从仓库内查找工具链：

```text
toolchain/arm-gnu-toolchain-15.2.rel1-mingw-w64-i686-arm-none-eabi/bin
```

也就是说：

- 工具链不依赖系统 `PATH`
- `toolchain/` 目录不会被提交到 Git
- 克隆仓库后，需要你自己把 Arm GNU Toolchain 放到上述目录，或者自行修改 `SConstruct`

## 当前构建流程

1. `SConstruct` 创建 GCC 构建环境
2. 根据 `target_mcu` 和 `target_chip` 生成全局编译配置
3. 顶层依次加载各模块 `SConscript`
4. 每个 `SConscript` 返回对象文件列表
5. 顶层统一链接生成：

```text
firmware.elf
firmware.bin
```

## 关键实现

- 全局头文件路径在 `SConstruct` 中集中配置，所有模块共享
- `targets/stm32f4/SConscript` 当前负责：
  - `system_stm32f4xx.c`
  - `startup_<target_chip>.s`
- 芯片宏由 `target_chip` 自动生成，例如 `stm32f446xx -> STM32F446xx`
- 当前构建宏使用 `USE_LL_DRIVER`
- 链接脚本路径按以下规则生成：

```text
targets/<target_mcu>/link/<ChipDefine>_FLASH.ld
```

例如：

```text
targets/stm32f4/link/STM32F446xx_FLASH.ld
```

## 当前状态

当前仓库已经具备基础模板结构，但还没有补完整个可直接量产使用的工程内容。

已具备：

- SCons 构建骨架
- STM32F4 CMSIS 头文件和启动文件
- STM32F4 厂商包资源目录，当前按 LL 方式使用
- `target_mcu + target_chip` 选择机制

待补充：

- `targets/<mcu>/link/` 下的正式链接脚本
- `bsp/<board>/SConscript` 和板级源码
- `drivers/hal/<mcu>/SConscript` 以及实际选用的 LL 驱动源码收集
- 根据工程需要补齐 `syscalls`、时钟初始化、外设初始化等内容

## 当前最小示例

`app/main.c` 目前是最小占位程序：

```c
int main(void)
{
    return 0;
}
```

后续可以在 `app/` 中逐步扩展应用代码。

## 建议的下一步

- 补一个可用的 `STM32F446xx_FLASH.ld`
- 增加 `bsp/test_board_v1/` 目录和对应 `SConscript`
- 把实际需要的 LL 外设驱动源码加入构建流程
- 增加与 LL 使用方式匹配的板级初始化代码
