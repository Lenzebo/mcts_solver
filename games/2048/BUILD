package(default_visibility = ["//visibility:public"])

cc_library(
    name = "2048",
    srcs = [
        "2048.cpp",
        "board.cpp",
    ],
    hdrs = [
        "2048.h",
        "board.h",
    ],
    deps = [
        "//mcts",
    ],
)

cc_binary(
    name = "play",
    srcs = ["play_2048.cpp"],
    deps = [":2048"],
)

cc_binary(
    name = "solve",
    srcs = [
        "policies.cpp",
        "policies.h",
        "solve_2048.cpp",
    ],
    deps = [
        ":2048",
    ],
)

cc_test(
    name = "test",
    srcs = ["test_2048.cpp"],
    deps = [
        ":2048",
        "@com_google_googletest//:gtest_main",
    ],
)
