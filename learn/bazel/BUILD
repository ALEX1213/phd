# Learning to use Bazel.

py_test(
    name = "python_binary",
    srcs = ["python_binary.py"],
    data = [
        ":data_binary",
        ":data_file",
    ],
    default_python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//labm8:bazelutil",
        "//third_party/py/absl",
    ],
)

filegroup(
    name = "data_file",
    srcs = ["data_file.txt"],
)

cc_binary(
    name = "data_binary",
    srcs = ["data_binary.cc"],
)

py_test(
    name = "tensorflow_test",
    srcs = ["tensorflow_test.py"],
    deps = [
        "//third_party/py/absl",
        "//third_party/py/pytest",
        "//third_party/py/tensorflow",
    ],
)
