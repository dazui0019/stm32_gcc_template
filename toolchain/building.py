import os

from SCons.Script import *


Projects = []

GLOBAL_GROUP_KEYS = (
    'CFLAGS',
    'CCFLAGS',
    'CXXFLAGS',
    'CPPPATH',
    'CPPDEFINES',
    'ASFLAGS',
    'LINKFLAGS',
    'LIBS',
    'LIBPATH',
)

LOCAL_GROUP_KEYS = (
    'LOCAL_CFLAGS',
    'LOCAL_CCFLAGS',
    'LOCAL_CXXFLAGS',
    'LOCAL_CPPPATH',
    'LOCAL_CPPDEFINES',
    'LOCAL_ASFLAGS',
)


def GetCurrentDir():
    conscript = File('SConscript')
    return os.path.dirname(conscript.rfile().abspath)


def _as_list(value):
    if value is None:
        return []
    if isinstance(value, (list, tuple)):
        items = []
        for item in value:
            items.extend(_as_list(item))
        return items
    return [value]


def _unique_items(items, key_func):
    result = []
    seen = set()
    for item in items:
        key = key_func(item)
        if key in seen:
            continue
        seen.add(key)
        result.append(item)
    return result


def _normalize_paths(items, base_dir):
    normalized = []
    for item in _as_list(items):
        if not item:
            continue
        if isinstance(item, str):
            if item.startswith('#') or os.path.isabs(item):
                normalized.append(os.path.normpath(item))
            else:
                normalized.append(os.path.normpath(os.path.join(base_dir, item)))
        else:
            normalized.append(item)
    return _unique_items(normalized, str)


def _normalize_sources(src):
    nodes = []
    for item in _as_list(src):
        if not item:
            continue
        node = item if hasattr(item, 'srcnode') else File(item)
        nodes.append(node)
    return _unique_items(nodes, lambda item: item.srcnode().path)


def _normalize_parameter(key, value, base_dir):
    if key in ('CPPPATH', 'LIBPATH', 'LOCAL_CPPPATH'):
        return _normalize_paths(value, base_dir)
    return [item for item in _as_list(value) if item not in (None, '')]


def GetConfigMacros(config_name='rtconfig.h'):
    config_path = os.path.join(Dir('#').abspath, config_name)
    macros = {}

    if not os.path.exists(config_path):
        return macros

    with open(config_path, 'r', encoding='utf-8') as config_file:
        for line in config_file:
            tokens = line.strip().split(None, 2)
            if len(tokens) >= 2 and tokens[0] == '#define':
                macros[tokens[1]] = tokens[2] if len(tokens) >= 3 else '1'

    return macros


def GetDepend(depend):
    depends = [item for item in _as_list(depend) if item]
    if not depends:
        return True

    macros = GetConfigMacros()
    return all(item in macros for item in depends)


def MergeGroup(existing_group, new_group):
    existing_group['src'] = _unique_items(
        existing_group['src'] + new_group['src'],
        lambda item: item.srcnode().path,
    )

    for key in GLOBAL_GROUP_KEYS + LOCAL_GROUP_KEYS:
        if key not in new_group:
            continue
        existing_group[key] = _unique_items(
            existing_group.get(key, []) + new_group[key],
            str,
        )


def DefineGroup(name, src, depend=None, **parameters):
    if not GetDepend(depend):
        return []

    group_path = GetCurrentDir()
    group = {
        'name': name,
        'path': group_path,
        'src': _normalize_sources(src),
    }

    for key in GLOBAL_GROUP_KEYS + LOCAL_GROUP_KEYS:
        group[key] = _normalize_parameter(key, parameters.get(key), group_path)

    for existing_group in Projects:
        if existing_group['name'] == name:
            MergeGroup(existing_group, group)
            return existing_group['src']

    Projects.append(group)
    return group['src']


def GetProjects():
    return Projects


def _append_unique(env, key, values):
    items = _as_list(values)
    if items:
        env.AppendUnique(**{key: items})


def BuildEnvironment(env, groups=None):
    build_env = env.Clone()
    for group in groups or Projects:
        for key in GLOBAL_GROUP_KEYS:
            _append_unique(build_env, key, group.get(key, []))
    return build_env


def _object_path(build_dir, source, objsuffix):
    source_path = source.srcnode().path
    return os.path.join(
        build_dir,
        'obj',
        os.path.splitext(source_path)[0] + objsuffix,
    )


def BuildGroupObjects(env, build_dir, groups=None):
    objects = []

    for group in groups or Projects:
        group_env = env.Clone()
        for key in LOCAL_GROUP_KEYS:
            env_key = key[len('LOCAL_'):]
            _append_unique(group_env, env_key, group.get(key, []))

        for source in group['src']:
            objects.append(
                group_env.Object(
                    _object_path(build_dir, source, group_env['OBJSUFFIX']),
                    source,
                )
            )

    return objects


def BuildFirmware(
    env,
    build_dir,
    program_name='firmware',
    groups=None,
    generate_bin=True,
    generate_compdb=True,
):
    groups = groups or Projects
    build_env = BuildEnvironment(env, groups)
    objects = BuildGroupObjects(build_env, build_dir, groups)

    elf = build_env.Program(
        os.path.join(build_dir, program_name + build_env['PROGSUFFIX']),
        objects,
    )

    targets = [elf]

    if generate_bin:
        targets.append(
            build_env.Command(
                os.path.join(build_dir, program_name + '.bin'),
                elf,
                '$OBJCOPY -O binary $SOURCE $TARGET',
            )
        )

    if generate_compdb and hasattr(build_env, 'CompilationDatabase'):
        targets.append(
            build_env.CompilationDatabase(
                os.path.join(build_dir, 'compile_commands.json')
            )
        )

    return targets


def FindChildScripts(base_dir=None):
    base_dir = base_dir or GetCurrentDir()
    scripts = []

    for item in sorted(os.listdir(base_dir)):
        script = os.path.join(base_dir, item, 'SConscript')
        if os.path.isfile(script):
            scripts.append(script)

    return scripts
