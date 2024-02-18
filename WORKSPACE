workspace(
    name = "com_github_etcd_cpp_apiv3_etcd_cpp_apiv3",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

load("workspace.bzl", "etcd_workspace")
etcd_workspace()


load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
