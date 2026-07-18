"""
generate.py —— DoubleLinkedList 脚本代码生成器

用法:
    python generate.py <suffix>

示例:
    python generate.py student   → 生成 ds_doublelinkedlist_student_type.h
                                      ds_doublelinkedlist_student.h
                                      ds_doublelinkedlist_student.c

工作流程:
    1. 用户编辑 ds_doublelinkedlist_type.h，定义想要的元素类型
    2. 运行本脚本，传入后缀名
    3. 脚本读取母版三件套，自动做标识符重命名
    4. 生成三份带后缀的独立文件，母版文件不受任何影响

替换原理:
    脚本使用"两趟占位符替换"算法，将所有与类型相关的标识符系统化地加上后缀。
    _type.h 操作宏统一使用 static inline 函数 + 宏调用模式。
    DoubleLinkedList 额外有游标类型 DoubleLinkedListNode。
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

    type_src   = read("ds_doublelinkedlist_type.h")
    head_src   = read("ds_doublelinkedlist.h")
    source_src = read("ds_doublelinkedlist.c")

    rules = [
        ("#include \"ds_doublelinkedlist_type.h\"",  f"#include \"ds_doublelinkedlist_{suffix}_type.h\""),
        ("#include \"ds_doublelinkedlist.h\"",       f"#include \"ds_doublelinkedlist_{suffix}.h\""),

        ("DS_DOUBLELINKEDLIST_TYPE_H",          f"DS_DOUBLELINKEDLIST_{SUFFIX}_TYPE_H"),
        ("DS_DOUBLELINKEDLIST_DESTROY_ELEMENT", f"DS_DOUBLELINKEDLIST_{SUFFIX}_DESTROY_ELEMENT"),
        ("DS_DOUBLELINKEDLIST_CLONE_ELEMENT",   f"DS_DOUBLELINKEDLIST_{SUFFIX}_CLONE_ELEMENT"),
        ("DS_DOUBLELINKEDLIST_MATCH_TYPE",      f"DS_DOUBLELINKEDLIST_{SUFFIX}_MATCH_TYPE"),
        ("DS_DOUBLELINKEDLIST_MATCH",           f"DS_DOUBLELINKEDLIST_{SUFFIX}_MATCH"),
        ("DS_DOUBLELINKEDLIST_TYPE",            f"DS_DOUBLELINKEDLIST_{SUFFIX}_TYPE"),
        ("DS_DOUBLELINKEDLIST_H",               f"DS_DOUBLELINKEDLIST_{SUFFIX}_H"),

        ("destroy_element",                     f"destroy_element_{suffix}"),
        ("clone_element",                       f"clone_element_{suffix}"),
        ("match_element",                       f"match_element_{suffix}"),

        ("ds_doublelinkedlist_type",            f"ds_doublelinkedlist_{suffix}_type"),
        ("DS_DoubleLinkedList",                 f"DS_DoubleLinkedList_{Cap}"),
        ("DoubleLinkedListNode",                f"DoubleLinkedListNode_{Cap}"),

        ("ds_doublelinkedlist_",                f"ds_doublelinkedlist_{suffix}_"),
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

    write(f"ds_doublelinkedlist_{suffix}_type.h", new_type)
    write(f"ds_doublelinkedlist_{suffix}.h",      new_header)
    write(f"ds_doublelinkedlist_{suffix}.c",      new_source)


def main():
    if len(sys.argv) != 2:
        print("用法: python generate.py <suffix>")
        sys.exit(1)
    suffix = sys.argv[1].strip().lower()
    if not suffix.isidentifier():
        print(f"[错误] 后缀名 '{suffix}' 不是合法的 C 标识符")
        sys.exit(1)

    print(f"\n{'='*60}")
    print(f"  DoubleLinkedList 代码生成器")
    print(f"  后缀: {suffix}")
    print(f"{'='*60}\n")
    print("[读取] ds_doublelinkedlist_type.h, ds_doublelinkedlist.h, ds_doublelinkedlist.c")
    generate(suffix)
    print(f"\n{'='*60}")
    print(f"  完成！在当前目录生成了 3 个文件：")
    print(f"    ds_doublelinkedlist_{suffix}_type.h")
    print(f"    ds_doublelinkedlist_{suffix}.h")
    print(f"    ds_doublelinkedlist_{suffix}.c")
    print(f"\n  在 main.c 中使用：")
    print(f"    #include \"ds_doublelinkedlist_{suffix}.h\"")
    print(f"{'='*60}\n")


if __name__ == "__main__":
    main()
