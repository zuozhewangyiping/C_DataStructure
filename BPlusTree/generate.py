"""
generate.py —— BPlusTree 脚本代码生成器

用法:
    python generate.py <suffix>

示例:
    python generate.py student   → 生成 ds_bplustree_student_type.h
                                      ds_bplustree_student.h
                                      ds_bplustree_student.c

工作流程:
    1. 用户编辑 ds_bplustree_type.h，定义 key/value 两个结构体及比较宏
    2. 运行本脚本，传入后缀名
    3. 脚本读取母版三件套，自动做标识符重命名
    4. 生成三份带后缀的独立文件，母版文件不受任何影响

注意:
    BPlusTree 是最特殊的容器：
    - 磁盘持久化（fread/fwrite 整页读写），必须定长 POD 类型
    - key 和 value 是两个独立结构体，分别需要类型别名和重命名
    - 无 DESTROY / CLONE 宏，无 static inline 函数
    - 有 5 组比较宏（LT GT EQ LE GE），仅用于 key
    - 游标类型为 BPlusTreeNode
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
        比如 old="ds_bplustree_" 的 new="ds_bplustree_student_"
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

    type_src   = read("ds_bplustree_type.h")
    head_src   = read("ds_bplustree.h")
    source_src = read("ds_bplustree.c")

    # ── 替换规则 ──
    # 格式: (原始文本, 替换后文本)
    # 顺序任意 —— 算法自动按长度排序。
    # BPlusTree 无 DESTROY / CLONE / static inline。
    # key 和 value 是独立类型，后缀插入在 bplustree 和 key/value 之间。
    rules = [
        ("#include \"ds_bplustree_type.h\"", f"#include \"ds_bplustree_{suffix}_type.h\""),
        ("#include \"ds_bplustree.h\"",      f"#include \"ds_bplustree_{suffix}.h\""),

        ("DS_BPLUSTREE_TYPE_H",          f"DS_BPLUSTREE_{SUFFIX}_TYPE_H"),
        ("DS_BPLUSTREE_KEY_TYPE",        f"DS_BPLUSTREE_{SUFFIX}_KEY_TYPE"),
        ("DS_BPLUSTREE_VALUE_TYPE",      f"DS_BPLUSTREE_{SUFFIX}_VALUE_TYPE"),
        ("DS_BPLUSTREE_LT",              f"DS_BPLUSTREE_{SUFFIX}_LT"),
        ("DS_BPLUSTREE_GT",              f"DS_BPLUSTREE_{SUFFIX}_GT"),
        ("DS_BPLUSTREE_EQ",              f"DS_BPLUSTREE_{SUFFIX}_EQ"),
        ("DS_BPLUSTREE_LE",              f"DS_BPLUSTREE_{SUFFIX}_LE"),
        ("DS_BPLUSTREE_GE",              f"DS_BPLUSTREE_{SUFFIX}_GE"),
        ("DS_BPLUSTREE_H",               f"DS_BPLUSTREE_{SUFFIX}_H"),

        # key/value 类型名 —— 后缀插在 bplustree 和 key/value 之间
        ("ds_bplustree_key_type",        f"ds_bplustree_{suffix}_key_type"),
        ("ds_bplustree_value_type",      f"ds_bplustree_{suffix}_value_type"),

        ("DS_BPlusTree",                 f"DS_BPlusTree_{Cap}"),
        ("BPlusTreeNode",                f"BPlusTreeNode_{Cap}"),

        ("ds_bplustree_",                f"ds_bplustree_{suffix}_"),
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

    write(f"ds_bplustree_{suffix}_type.h", new_type)
    write(f"ds_bplustree_{suffix}.h",      new_header)
    write(f"ds_bplustree_{suffix}.c",      new_source)


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
    print(f"  BPlusTree 代码生成器")
    print(f"  后缀: {suffix}")
    print(f"{'='*60}\n")
    print("[读取] ds_bplustree_type.h, ds_bplustree.h, ds_bplustree.c")

    generate(suffix)

    print(f"\n{'='*60}")
    print(f"  完成！在当前目录生成了 3 个文件：")
    print(f"    ds_bplustree_{suffix}_type.h")
    print(f"    ds_bplustree_{suffix}.h")
    print(f"    ds_bplustree_{suffix}.c")
    print(f"")
    print(f"  在 main.c 中使用：")
    print(f"    #include \"ds_bplustree_{suffix}.h\"")
    print(f"{'='*60}\n")


if __name__ == "__main__":
    main()
