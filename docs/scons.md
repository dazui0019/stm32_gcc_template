# SCons 构建说明

这份文档说明当前仓库里的精简版 SCons 构建系统如何组织、如何扩展，以及模块 `SConscript` 应该怎么写。

## 1. 设计目标

这套实现借鉴了 RT-Thread 的思路，但刻意保持精简：

- `SConstruct` 负责顶层构建流程
- 各模块 `SConscript` 只负责描述自己的源码和参数
- `DefineGroup()` 负责注册模块信息
- 顶层统一决定如何编译对象文件、链接固件、生成附加产物

当前精简版实现位于 `toolchain/building.py`，而仓库根目录下的 `building.py` 是 RT-Thread 原版参考代码。  
为了避免导入混淆，`SConstruct` 通过 `sys.path.insert(0, ...)` 优先使用 `toolchain/building.py`。

## 2. 当前构建入口

### 2.1 顶层入口

顶层入口是 `SConstruct`，它主要做这些事：

- 选择工具链
- 配置全局编译和链接参数
- 加载各模块的 `SConscript`
- 收集所有 group
- 调用 `BuildFirmware()` 生成：
  - `build/firmware.elf`
  - `build/firmware.bin`
  - `build/compile_commands.json`

### 2.2 模块入口

当前默认会加载这些模块脚本：

- `core/SConscript`
- `utils/SConscript`
- `drivers/SConscript`
- `app/SConscript`
- `targets/SConscript`

其中 `app/SConscript` 还会通过 `FindChildScripts()` 自动加载 `app/` 下直接子目录里的 `SConscript`。
`drivers/SConscript` 也会通过 `FindChildScripts()` 分发加载 `drivers/` 下各个驱动库目录里的 `SConscript`。

注意：`FindChildScripts()` 当前只扫描一层子目录，不做全工程递归搜索。

## 3. 构建流程

整体流程如下：

1. `SConstruct` 创建 SCons `Environment`
2. 顶层依次执行模块 `SConscript`
3. 父目录型模块可以继续分发子目录 `SConscript`
4. 每个实际模块通过 `DefineGroup()` 把自身信息注册到全局 `Projects`
5. `BuildEnvironment()` 汇总所有 group 的全局导出参数
6. `BuildGroupObjects()` 为每个 group 生成对象文件
7. `BuildFirmware()` 统一链接为 `firmware.elf`
8. 额外生成 `firmware.bin` 和 `compile_commands.json`

对象文件输出路径会镜像源码路径，例如：

```text
app/src/main.c
-> build/obj/app/src/main.o
```

这样源目录不会被中间产物污染。

## 4. 核心概念

### 4.1 `Projects`

`Projects` 是一个全局列表，用来保存所有模块 group 的元数据。  
每个 group 本质上是一个字典，典型字段包括：

- `name`
- `path`
- `src`
- `CPPPATH`
- `CPPDEFINES`
- `CCFLAGS`
- `LOCAL_CPPPATH`
- `LOCAL_CCFLAGS`

### 4.2 `DefineGroup()`

`DefineGroup()` 是模块注册入口，当前职责很明确：

- 检查依赖条件 `depend`
- 规范化 `src`
- 规范化路径类参数，例如 `CPPPATH`
- 把 group 注册到 `Projects`
- 如果存在同名 group，就把内容合并
- 返回当前 group 的源码节点列表

最常见的写法如下：

```python
import os
from building import *

cwd = GetCurrentDir()

group = DefineGroup(
    'app',
    Glob('src/*.c'),
    CPPPATH=[cwd, os.path.join(cwd, 'inc')],
)

Return('group')
```

### 4.3 group 合并

如果多次调用 `DefineGroup('app', ...)`，系统不会创建两个同名模块，而是合并：

- `src` 去重后合并
- 全局参数追加合并
- 局部参数追加合并

这和 RT-Thread 的“同名组件可分散声明再归并”的用法类似，但当前实现更简单。

## 5. 参数分类

当前支持两类 group 参数。

### 5.1 全局导出参数

这些参数会先汇总到统一的构建环境，再用于所有 group：

- `CFLAGS`
- `CCFLAGS`
- `CXXFLAGS`
- `CPPPATH`
- `CPPDEFINES`
- `ASFLAGS`
- `LINKFLAGS`
- `LIBS`
- `LIBPATH`

适合放“这个模块需要让其他模块也看到”的公共配置，比如公共头文件目录和全局宏。

### 5.2 局部参数

这些参数只会作用到当前 group 自己生成的对象文件：

- `LOCAL_CFLAGS`
- `LOCAL_CCFLAGS`
- `LOCAL_CXXFLAGS`
- `LOCAL_CPPPATH`
- `LOCAL_CPPDEFINES`
- `LOCAL_ASFLAGS`

适合放模块私有编译选项，避免污染全局环境。

示例：

```python
DefineGroup(
    'driver_uart',
    Glob('src/*.c'),
    CPPPATH=['inc'],
    LOCAL_CCFLAGS=['-Wno-unused-parameter'],
    LOCAL_CPPDEFINES=['UART_USE_DMA'],
)
```

## 6. 条件编译

`DefineGroup()` 支持 `depend` 参数：

```python
DefineGroup(
    'feature_x',
    Glob('src/*.c'),
    depend='ENABLE_FEATURE_X',
)
```

也支持列表：

```python
DefineGroup(
    'feature_y',
    Glob('src/*.c'),
    depend=['ENABLE_FEATURE_Y', 'ENABLE_LOG'],
)
```

依赖判断规则：

- `depend=None` 或空值时，总是参与构建
- 如果传了依赖项，就会去读取工程根目录下的 `rtconfig.h`
- 只有依赖宏全部存在时，这个 group 才会被注册

注意：如果仓库里没有 `rtconfig.h`，那所有带 `depend` 的模块都会被跳过。

## 7. 目录约定

推荐模块目录结构如下：

```text
module_name/
|-- SConscript
|-- inc/
`-- src/
```

如果某个目录本身只是“模块集合”，例如 `drivers/`，推荐只写成父目录分发器：

```python
from building import *

cwd = GetCurrentDir()

for script in FindChildScripts(cwd):
    SConscript(script)
```

推荐的 `SConscript` 写法：

```python
import os
from building import *

cwd = GetCurrentDir()

group = DefineGroup(
    'module_name',
    Glob('src/*.c') + Glob('src/*.s'),
    CPPPATH=[cwd, os.path.join(cwd, 'inc')],
)

for script in FindChildScripts(cwd):
    SConscript(script)

Return('group')
```

说明：

- `GetCurrentDir()` 返回当前 `SConscript` 所在目录
- `Glob('src/*.c')` 是相对当前 `SConscript` 的路径
- 不建议写成 `Glob('/src/*.c')`，前导 `/` 会导致路径语义错误

## 8. 当前已接入的模块

### 8.1 `app`

`app/SConscript` 当前会注册：

- `app/src/*.c`
- `app/inc`

并自动加载 `app` 下子模块的 `SConscript`。

### 8.2 `drivers`

`drivers/SConscript` 当前不直接注册源文件，而是作为父目录分发器，自动加载：

- `drivers/foo_hal/SConscript`
- 后续新增的其它驱动库 `SConscript`

每个驱动库子目录负责自己维护：

- `src/*.c`
- `inc/`
- `depend`
- 自己的 `DefineGroup(...)`

### 8.3 `target`

`targets/SConscript` 当前会根据 `target_mcu` 注册：

- `cmsis/Source/Templates/system_stm32f4xx.c`
- `cmsis/Source/Templates/gcc/startup_<target_chip>.s`

同时导出这些头文件路径：

- `targets/<target_mcu>/cmsis/Include`
- `targets/<target_mcu>/hal/Inc`
- `targets/cmsis_core`

## 9. 常用命令

如果你使用 `uv`：

```powershell
uv sync
uv run scons
```

如果你直接使用项目虚拟环境：

```powershell
.\.venv\Scripts\scons.exe
```

指定芯片：

```powershell
uv run scons target_mcu=stm32f4 target_chip=stm32f446xx
uv run scons target_mcu=stm32f4 target_chip=stm32f407xx
```

清理构建目录：

```powershell
uv run scons -c
```

## 10. 产物说明

默认输出：

```text
build/firmware.elf
build/firmware.bin
build/firmware.map
build/compile_commands.json
build/obj/...
```

其中：

- `firmware.elf` 是链接后的可执行映像
- `firmware.bin` 是烧录常用的裸二进制
- `firmware.map` 记录链接映射
- `compile_commands.json` 方便 clangd、VS Code 等工具做跳转和补全

## 11. 新增模块的步骤

以新增 `drivers/foo` 为例：

1. 创建目录

```text
drivers/foo/
|-- SConscript
|-- inc/
`-- src/
```

2. 编写 `drivers/foo/SConscript`

```python
import os
from building import *

cwd = GetCurrentDir()

group = DefineGroup(
    'foo',
    Glob('src/*.c'),
    CPPPATH=[cwd, os.path.join(cwd, 'inc')],
)

Return('group')
```

3. 由 `drivers/SConscript` 自动分发加载它

```python
from building import *

cwd = GetCurrentDir()

for script in FindChildScripts(cwd):
    SConscript(script)
```

## 12. 当前实现和 RT-Thread 的差异

当前版本只保留了最核心的模块化构建能力，还没有引入 RT-Thread 那些更重的功能，例如：

- 自动生成 Keil / IAR / VSCode 工程
- `LIBRARY` 分组构建静态库
- 更复杂的工具链兼容处理
- 更多构建选项和命令行开关

这样的好处是：

- 容易读懂
- 容易调试
- 便于针对 STM32 模板工程逐步扩展

## 13. 后续可扩展方向

如果后面要继续增强，这几个方向最值得优先做：

- 增加 `LIBRARY` 分组能力
- 支持 `drivers/` 和 `bsp/` 模块自动接入
- 扩展 `depend`，支持命令行和配置文件共同驱动
- 增加烧录命令封装，例如 `openocd` / `pyocd`
- 增加 IDE 工程导出功能
