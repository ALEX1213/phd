# Experiemnts in random optimization.

py_library(
    name = "implementation",
    srcs = ["implementation.py"],
    deps = [
        "//compilers/llvm",
        "//compilers/llvm:clang",
        "//compilers/llvm:llvm_link",
        "//compilers/llvm:opt",
        "//experimental/compilers/random_opt/proto:random_opt_py_pb2",
        "//labm8:crypto",
        "//labm8:jsonutil",
        "//labm8:labdate",
        "//labm8:pbutil",
        "//labm8:text",
        "//third_party/py/absl",
        "//third_party/py/gym",
        "//third_party/py/numpy",
    ],
)

py_test(
    name = "implementation_test",
    srcs = ["implementation_test.py"],
    deps = [
        ":implementation",
        "//compilers/llvm:clang",
        "//third_party/py/absl",
        "//third_party/py/pytest",
    ],
)

py_library(
    name = "environments",
    srcs = ["environments.py"],
    deps = [
        # Note that :implementation is not directly imported, but required by
        # the gym registration process.
        ":implementation",
        "//compilers/llvm:util",
        "//labm8:labtypes",
        "//third_party/py/absl",
        "//datasets/benchmarks:bzip2",
        "//datasets/benchmarks:llvm_test_suite",
        "//experimental/compilers/random_opt/proto:random_opt_py_pb2",
    ],
)

py_test(
    name = "environments_test",
    srcs = ["environments_test.py"],
    deps = [
        ":environments",
        "//third_party/py/absl",
        "//third_party/py/gym",
        "//third_party/py/pytest",
    ],
)

py_binary(
    name = "results_to_dot",
    srcs = ["results_to_dot.py"],
    deps = [
        "//experimental/compilers/random_opt/proto:random_opt_py_pb2",
        "//labm8:graph",
        "//labm8:pbutil",
        "//third_party/py/absl",
    ],
)

py_binary(
    name = "random_opt",
    srcs = ["random_opt.py"],
    deps = [
        ":environments",
        ":implementation",
        "//experimental/compilers/random_opt/proto:random_opt_py_pb2",
        "//labm8:pbutil",
        "//third_party/py/absl",
        "//third_party/py/gym",
    ],
)
