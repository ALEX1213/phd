# Experiments while porting CLgen from Tensorflow to Keras models.

py_test(
    name = "benchmark_module_import",
    srcs = ["benchmark_module_import.py"],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/absl",
        "//third_party/py/keras",
        "//third_party/py/pytest",
        "//third_party/py/tensorflow",
    ],
)

py_test(
    name = "benchmark_model_init",
    srcs = ["benchmark_model_init.py"],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/absl",
        "//third_party/py/keras",
        "//third_party/py/pytest",
        "//third_party/py/tensorflow",
    ],
)

py_test(
    name = "benchmark_string_concatenation",
    srcs = ["benchmark_string_concatenation.py"],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/absl",
        "//third_party/py/pytest",
    ],
)

py_test(
    name = "log_to_file",
    srcs = ["log_to_file.py"],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/absl",
        "//third_party/py/pytest",
    ],
)

py_binary(
    name = "opencl_baseline",
    srcs = ["opencl_baseline.py"],
    data = [
        "opencl_baseline_a.pbtxt",
        "opencl_baseline_b.pbtxt",
    ],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//deeplearning/clgen",
        "//labm8:bazelutil",
        "//third_party/py/absl",
    ],
)
