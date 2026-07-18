"""
generate.py —— AVLTree 脚本代码生成器

用法:
    python generate.py <suffix>

示例:
    python generate.py student   → 生成 ds_avltree_student_type.h
                                      ds_avltree_student.h
                                      ds_avltree_student.c

替换原理:
    两趟占位符替换算法。AVLTree 没有 MATCH 宏，有 5 个比较宏（LT GT EQ LE GE）。
    static inline 函数仅 destroy_element 和 clone_element，无 match_element。
"""

import sys
import os


def two_pass_replace(text, replacements):
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
    SUFFIX = suffix.upper()
    Cap    = suffix.capitalize()

    def read(path):
        if not os.path.exists(path):
            sys.exit(f"[错误] 找不到文件: {path}")
        with open(path, "r", encoding="utf-8-sig") as f:
            return f.read()

    type_src   = read("ds_avltree_type.h")
    head_src   = read("ds_avltree.h")
    source_src = read("ds_avltree.c")

    rules = [
        ("#include \"ds_avltree_type.h\"",  f"#include \"ds_avltree_{suffix}_type.h\""),
        ("#include \"ds_avltree.h\"",       f"#include \"ds_avltree_{suffix}.h\""),

        ("DS_AVLTREE_TYPE_H",           f"DS_AVLTREE_{SUFFIX}_TYPE_H"),
        ("DS_AVLTREE_DESTROY_ELEMENT",  f"DS_AVLTREE_{SUFFIX}_DESTROY_ELEMENT"),
        ("DS_AVLTREE_CLONE_ELEMENT",    f"DS_AVLTREE_{SUFFIX}_CLONE_ELEMENT"),
        ("DS_AVLTREE_LT",               f"DS_AVLTREE_{SUFFIX}_LT"),
        ("DS_AVLTREE_GT",               f"DS_AVLTREE_{SUFFIX}_GT"),
        ("DS_AVLTREE_EQ",               f"DS_AVLTREE_{SUFFIX}_EQ"),
        ("DS_AVLTREE_LE",               f"DS_AVLTREE_{SUFFIX}_LE"),
        ("DS_AVLTREE_GE",               f"DS_AVLTREE_{SUFFIX}_GE"),
        ("DS_AVLTREE_TYPE",             f"DS_AVLTREE_{SUFFIX}_TYPE"),
        ("DS_AVLTREE_H",                f"DS_AVLTREE_{SUFFIX}_H"),

        # static inline 函数 —— AVLTree 无 match_element
        ("destroy_element",             f"destroy_element_{suffix}"),
        ("clone_element",               f"clone_element_{suffix}"),

        ("ds_avltree_type",             f"ds_avltree_{suffix}_type"),
        ("DS_AVLTree",                  f"DS_AVLTree_{Cap}"),
        ("AVLTreeNode",                 f"AVLTreeNode_{Cap}"),

        ("ds_avltree_",                 f"ds_avltree_{suffix}_"),
    ]

    new_type   = two_pass_replace(type_src,   rules)
    new_header = two_pass_replace(head_src,   rules)
    new_source = two_pass_replace(source_src, rules)

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

    write(f"ds_avltree_{suffix}_type.h", new_type)
    write(f"ds_avltree_{suffix}.h",      new_header)
    write(f"ds_avltree_{suffix}.c",      new_source)


def main():
    if len(sys.argv) != 2:
        print("用法: python generate.py <suffix>")
        sys.exit(1)
    suffix = sys.argv[1].strip().lower()
    if not suffix.isidentifier():
        print(f"[错误] 后缀名 '{suffix}' 不是合法的 C 标识符")
        sys.exit(1)

    print(f"\n  AVLTree 代码生成器 —— 后缀: {suffix}\n")
    generate(suffix)
    print(f"  完成！ds_avltree_{suffix}_type.h / .h / .c\n")


if __name__ == "__main__":
    main()
