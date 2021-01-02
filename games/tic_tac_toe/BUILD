package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tic_tac_toe",
    srcs = [],
    hdrs = ["tic_tac_toe.h"],
    deps = [
        "//mcts",
        "@com_lenzebo_zbo//zbo:meta_enum",
    ],
)

cc_binary(
    name = "play",
    srcs = ["play_tic_tac_toe.cpp"],
    deps = [":tic_tac_toe"],
)

cc_binary(
    name = "solve",
    srcs = ["solve_tic_tac_toe.cpp"],
    deps = [
        ":tic_tac_toe",
        "@com_lenzebo_zbo//zbo:stop_watch",
    ],
)
