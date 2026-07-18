"""
generate.py —— Stack 脚本代码生成器

用法:
    python generate.py <suffix>

示例:
    python generate.py student   → 生成 ds_stack_student_type.h
                                      ds_stack_student.h
                                      ds_stack_student.c

工作流程:
    1. 用户编辑 ds_stack_type.h，定义想要的元素类型
    2. 运行本脚本，传入后缀名
    3. 脚本读取母版三件套，自动做标识符重命名
    4. 生成三份带后缀的独立文件，母版文件不受任何影响

    如果不需要"同一编译单元使用多种元素类型"的功能，
    完全忽略本脚本即可，原有用法与之前无异。

替换原理:
    脚本使用"两趟占位符替换"算法，将所有与类型相关的标识符
    （类型名、宏名、函数名、结构体名、static inline 辅助函数名、
    include 路径）系统化地加上后缀。算法对任意后缀名安全。

注意:
    Stack 结构最简——无 MATCH 宏，无游标类型，无比较宏。
    static inline 函数仅 destroy_element 和 clone_element。
"""

import sys
import os


def two_pass_replace(text, replacements):
    """对 text 执行两趟替换，将所有 old 字符串替换为对应的 new 字符串。

    参数:
        text:         原始文本
        replacements: [(old, new), ...]  替换对列表，顺序任意

    算法:
        第一趟  old → 占位符（按 old 长度降序，长串优先）
        第二趟  占位符 → new（任意顺序，占位符之间互不冲突）

    为什么需要占位符？
        某些 new 字符串可能仍然以某个 old 字符串开头。
        比如 old="ds_stack_" 的 new="ds_stack_student_"
        如果直接替换后再执行其他规则，可能被二次匹配。
        占位符（如 @@0@@  @@1@@）不含任何原始标识符片段，
        彻底隔离了替换规则之间的相互干扰。
    """
    sorted_pairs = sorted(replacements, key=lambda r: len(r[0]), reverse=True)

    old_to_ph = {}
    ph_to_new = {}
    for i, (old, new) in enumerate(sorted_pairs):
        ph = f"@@{i}@@"
        old_to_ph[old] = ph
        ph_to_new[ph] = new

    for old, _ in sorted_pairs:
        text = text.replace(old, old_to_ph[old])

    for ph, new in ph_to_new.items():
        text = text.replace(ph, new)

    return text


def generate(suffix):
    """读取母版文件，生成带后缀的独立副本。"""

    SUFFIX = suffix.upper()          # 用于宏名（全大写）
    Cap    = suffix.capitalize()     # 用于结构体名（首字母大写）

    # ── 读取母版文件 ──
    def read(path):
        if not os.path.exists(path):
            sys.exit(f"[错误] 找不到文件: {path}")
        with open(path, "r", encoding="utf-8-sig") as f:
            return f.read()

    type_src   = read("ds_stack_type.h")
    head_src   = read("ds_stack.h")
    source_src = read("ds_stack.c")

    # ── 替换规则 ──
    # 格式: (原始文本, 替换后文本)
    # 顺序任意 —— 算法自动按长度排序，保证长串先于短串处理。
    # Stack 结构最简：无 MATCH，无游标，无比较宏。
    rules = [
        ("#include \"ds_stack_type.h\"", f"#include \"ds_stack_{suffix}_type.h\""),
        ("#include \"ds_stack.h\"",      f"#include \"ds_stack_{suffix}.h\""),

        ("DS_STACK_TYPE_H",          f"DS_STACK_{SUFFIX}_TYPE_H"),
        ("DS_STACK_DESTROY_ELEMENT", f"DS_STACK_{SUFFIX}_DESTROY_ELEMENT"),
        ("DS_STACK_CLONE_ELEMENT",   f"DS_STACK_{SUFFIX}_CLONE_ELEMENT"),
        ("DS_STACK_TYPE",            f"DS_STACK_{SUFFIX}_TYPE"),
        ("DS_STACK_H",               f"DS_STACK_{SUFFIX}_H"),

        # static inline 辅助函数
        ("destroy_element",          f"destroy_element_{suffix}"),
        ("clone_element",            f"clone_element_{suffix}"),

        ("ds_stack_type",            f"ds_stack_{suffix}_type"),
        ("DS_Stack",                 f"DS_Stack_{Cap}"),

        ("ds_stack_",                f"ds_stack_{suffix}_"),
    ]

    # ── 对三个母版分别执行替换 ──
    new_type   = two_pass_replace(type_src,   rules)
    new_header = two_pass_replace(head_src,   rules)
    new_source = two_pass_replace(source_src, rules)

    # ── 写出（已存在时询问用户）──
    def write(path, content):
        if os.path.exists(path):
            answer = input(f"  ⚠ 文件已存在: {path}\n"
                           f"     覆盖? [y/N] ").strip().lower()
            if answer != 'y':
                print(f"  ✗ 已跳过: {path}")
                return
        with open(path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"  ✓ 已生成: {path}")

    write(f"ds_stack_{suffix}_type.h", new_type)
    write(f"ds_stack_{suffix}.h",      new_header)
    write(f"ds_stack_{suffix}.c",      new_source)


def main():
    if len(sys.argv) != 2:
        print("用法: python generate.py <suffix>")
        print("示例: python generate.py student")
        sys.exit(1)

    suffix = sys.argv[1].strip().lower()

    if not suffix.isidentifier():
        print(f"[错误] 后缀名 '{suffix}' 不是合法的 C 标识符")
        sys.exit(1)

    print(f"\n{'='*60}")
    print(f"  Stack 代码生成器")
    print(f"  后缀: {suffix}")
    print(f"{'='*60}\n")
    print("[读取] ds_stack_type.h, ds_stack.h, ds_stack.c")

    generate(suffix)

    print(f"\n{'='*60}")
    print(f"  完成！在当前目录生成了 3 个文件：")
    print(f"    ds_stack_{suffix}_type.h")
    print(f"    ds_stack_{suffix}.h")
    print(f"    ds_stack_{suffix}.c")
    print(f"")
    print(f"  在 main.c 中使用：")
    print(f"    #include \"ds_stack_{suffix}.h\"")
    print(f"{'='*60}\n")


if __name__ == "__main__":
    main()
