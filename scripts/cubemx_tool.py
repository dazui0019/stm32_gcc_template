from pathlib import Path
import hashlib
import subprocess

# 用户生成.ioc文件的hash值, 如果.ioc文件没有改变则会跳过cubemx生成工程的步骤。
def file_sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

# 执行 CubeMX 生成代码
# 参考 UM1718 《3.3.2 Running STM32CubeMX in command-line mode》
def generate_with_cubemx(ioc_path: str, force: bool = False) -> bool:
    cubemx_dir = Path(r"D:\Fake Program Files\STM32CubeMX")
    ioc = Path(ioc_path).resolve()

    stamp = ioc.parent / ".cubemx_generate.sha256"
    current_hash = file_sha256(ioc)

    if not force and stamp.exists() and stamp.read_text(encoding="utf-8") == current_hash:
        print(f"CubeMX skip: {ioc.name} not changed")
        return False

    script = ioc.parent / ".cubemx_generate.txt"
    script.write_text(
        f'config load "{ioc}"\n'
        "project generate\n"
        "exit\n",
        encoding="utf-8",
    )

    subprocess.run(
        [
            str(cubemx_dir / "jre" / "bin" / "java.exe"),
            "-jar",
            "STM32CubeMX.exe",
            "-q",
            str(script),
        ],
        cwd=cubemx_dir,
        check=True,
    )

    stamp.write_text(current_hash, encoding="utf-8")
    return True
