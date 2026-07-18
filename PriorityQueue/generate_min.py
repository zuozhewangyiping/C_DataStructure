"""
generate_min.py —— PriorityQueue（最小堆）脚本代码生成器

用法:
    python generate_min.py <suffix>

示例:
    python generate_min.py student   → 生成 ds_priorityqueue_min_student_type.h
                                          ds_priorityqueue_min_student.h
                                          ds_priorityqueue_min_student.c

工作流程:
    1. 用户编辑 ds_priorityqueue_min_type.h，定义想要的元素类型
    2. 运行本脚本，传入后缀名
    3. 脚本读取母版三件套，自动做标识符重命名
    4. 生成三份带后缀的独立文件，母版文件不受任何影响

注意:
    PriorityQueue 分 min / max 两套独立文件，本脚本处理 min 版本。
    max 版本请使用 generate_max.py，结构完全对称。

    PriorityQueue 使用比较宏（LT GT EQ LE GE）而非 MATCH 宏。
    static inline 函数为 destroy_element_min 和 clone_element_min。
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

    type_src   = read("ds_priorityqueue_min_type.h")
    head_src   = read("ds_priorityqueue_min.h")
    source_src = read("ds_priorityqueue_min.c")

    # ── 替换规则 ──
    # 格式: (原始文本, 替换后文本)
    # 顺序任意 —— 算法自动按长度排序。
    # PriorityQueue 的比较宏是纯表达式，static inline 函数已带 _min 后缀。
    rules = [
        ("#include \"ds_priorityqueue_min_type.h\"",  f"#include \"ds_priorityqueue_min_{suffix}_type.h\""),
        ("#include \"ds_priorityqueue_min.h\"",       f"#include \"ds_priorityqueue_min_{suffix}.h\""),

        ("DS_PRIORITYQUEUE_MIN_TYPE_H",          f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_TYPE_H"),
        ("DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT", f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_DESTROY_ELEMENT"),
        ("DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT",   f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_CLONE_ELEMENT"),
        ("DS_PRIORITYQUEUE_MIN_LT",              f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_LT"),
        ("DS_PRIORITYQUEUE_MIN_GT",              f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_GT"),
        ("DS_PRIORITYQUEUE_MIN_EQ",              f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_EQ"),
        ("DS_PRIORITYQUEUE_MIN_LE",              f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_LE"),
        ("DS_PRIORITYQUEUE_MIN_GE",              f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_GE"),
        ("DS_PRIORITYQUEUE_MIN_TYPE",            f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_TYPE"),
        ("DS_PRIORITYQUEUE_MIN_H",               f"DS_PRIORITYQUEUE_MIN_{SUFFIX}_H"),

        # static inline 辅助函数（已带 _min，再加用户后缀）
        ("destroy_element_min",                  f"destroy_element_min_{suffix}"),
        ("clone_element_min",                    f"clone_element_min_{suffix}"),

        ("ds_priorityqueue_min_type",            f"ds_priorityqueue_min_{suffix}_type"),
        ("DS_PriorityQueue_min",                 f"DS_PriorityQueue_min_{Cap}"),

        ("ds_priorityqueue_min_",                f"ds_priorityqueue_min_{suffix}_"),
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

    write(f"ds_priorityqueue_min_{suffix}_type.h", new_type)
    write(f"ds_priorityqueue_min_{suffix}.h",      new_header)
    write(f"ds_priorityqueue_min_{suffix}.c",      new_source)


def main():
    if len(sys.argv) != 2:
        print("用法: python generate_min.py <suffix>")
        print("示例: python generate_min.py student")
        sys.exit(1)

    suffix = sys.argv[1].strip().lower()

    if not suffix.isidentifier():
        print(f"[错误] 后缀名 '{suffix}' 不是合法的 C 标识符")
        sys.exit(1)

    print(f"\n{'='*60}")
    print(f"  PriorityQueue (min) 代码生成器")
    print(f"  后缀: {suffix}")
    print(f"{'='*60}\n")
    print("[读取] ds_priorityqueue_min_type.h, ds_priorityqueue_min.h, ds_priorityqueue_min.c")

    generate(suffix)

    print(f"\n{'='*60}")
    print(f"  完成！在当前目录生成了 3 个文件：")
    print(f"    ds_priorityqueue_min_{suffix}_type.h")
    print(f"    ds_priorityqueue_min_{suffix}.h")
    print(f"    ds_priorityqueue_min_{suffix}.c")
    print(f"")
    print(f"  在 main.c 中使用：")
    print(f"    #include \"ds_priorityqueue_min_{suffix}.h\"")
    print(f"{'='*60}\n")


if __name__ == "__main__":
    main()
