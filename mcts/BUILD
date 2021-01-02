package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mcts",
    srcs = [],
    hdrs = [
        "details/problem_impl.h",
        "details/solver_impl.h",
        "rollout/random_rollout.h",
        "rollout/rollout.h",
        "selection/selection.h",
        "selection/ucb1.h",
        "node_statistic.h",
        "problem.h",
        "solver.h",
        "state.h",
        "tree.h",
        "types.h",
        "tree_export.h",
    ],
    deps = ["@com_lenzebo_zbo//zbo:max_size_vector",
            "@com_lenzebo_zbo//zbo:named_type"]
)
