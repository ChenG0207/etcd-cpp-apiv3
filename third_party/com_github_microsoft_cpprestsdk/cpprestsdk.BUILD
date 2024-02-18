package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "cpprestsdk",
    hdrs = glob(["Release/include/cpprest/**/*.h","Release/include/pplx/*.h"]),
    srcs = glob(["lib64/libcpprest.*"]),
    includes = ["Release/include"]
)
