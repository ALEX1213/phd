# Package to handle data from Timing.app.

py_binary(
    name = "timing",
    srcs = ["timing.py"],
    visibility = ["//datasets/me_db:__pkg__"],
    deps = [
        "//datasets/me_db:importers",
        "//datasets/me_db:me_proto_pb2",
        "//labm8:bazelutil",
        "//labm8:pbutil",
        "//third_party/py/absl",
        "//third_party/py/humanize",
    ],
)
