"""This module contains some dependency"""

# buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def clean_dep(dep):
    return str(Label(dep))

# buildifier: disable=function-docstring-args
def etcd_workspace(path_prefix = "", repo_name = "", **kwargs):

    if not native.existing_rule("com_googlesource_code_re2"):
        http_archive(
            name = "com_googlesource_code_re2",
            sha256 = "0915741f524ad87debb9eb0429fe6016772a1569e21dc6d492039562308fcb0f",
            strip_prefix = "re2-2020-10-01",
            urls = ["https://github.com/google/re2/archive/2020-10-01.tar.gz"],
        )

    # bazel_skylib version and summary.
    bazel_skylib_ver = kwargs.get("bazel_skylib_ver", "1.0.2")
    bazel_skylib_sha256 = kwargs.get("bazel_skylib_sha256", "e5d90f0ec952883d56747b7604e2a15ee36e288bb556c3d0ed33e818a4d971f2")
    bazel_skylib_urls = [
        "https://github.com/bazelbuild/bazel-skylib/archive/{ver}.tar.gz".format(ver = bazel_skylib_ver),
    ]
    if not native.existing_rule("bazel_skylib"):
        http_archive(
            name = "bazel_skylib",
            sha256 = bazel_skylib_sha256,
            strip_prefix = "bazel-skylib-{ver}".format(ver = bazel_skylib_ver),
            urls = bazel_skylib_urls,
        )

    # rules_python version and summary（required for higher versions of protobuf.）
    rules_python_ver = kwargs.get("rules_python_ver", "4b84ad270387a7c439ebdccfd530e2339601ef27")
    rules_python_sha256 = kwargs.get("rules_python_sha256", "e5470e92a18aa51830db99a4d9c492cc613761d5bdb7131c04bd92b9834380f6")
    rules_python_urls = [
        "https://github.com/bazelbuild/rules_python/archive/{ver}.tar.gz".format(ver = rules_python_ver),
    ]
    if not native.existing_rule("rules_python"):
        http_archive(
            name = "rules_python",
            sha256 = rules_python_sha256,
            strip_prefix = "rules_python-{ver}".format(ver = rules_python_ver),
            urls = rules_python_urls,
        )

    # zlib
    zlib_ver = kwargs.get("zlib_ver", "1.2.11")
    zlib_sha256 = kwargs.get("zlib_sha256", "629380c90a77b964d896ed37163f5c3a34f6e6d897311f1df2a7016355c45eff")
    zlib_urls = [
        "https://github.com/madler/zlib/archive/v{ver}.tar.gz".format(ver = zlib_ver),
    ]
    if not native.existing_rule("zlib"):
        http_archive(
            name = "zlib",
            build_file = clean_dep("//third_party/zlib:zlib.BUILD"),
            sha256 = zlib_sha256,
            strip_prefix = "zlib-{ver}".format(ver = zlib_ver),
            urls = zlib_urls,
        )
    
    new_git_repository(
        name = "com_github_microsoft_cpprestsdk",
        remote = "https://github.com/microsoft/cpprestsdk.git",
        branch = "master",
        build_file = "//third_party/com_github_microsoft_cpprestsdk:cpprestsdk.BUILD",
    )


    # protobuf version and summary
    com_google_protobuf_ver = kwargs.get("com_google_protobuf_ver", "3.15.8")
    com_google_protobuf_sha256 = kwargs.get("com_google_protobuf_sha256", "0cbdc9adda01f6d2facc65a22a2be5cecefbefe5a09e5382ee8879b522c04441")
    com_google_protobuf_strip_prefix = "protobuf-{ver}".format(ver = com_google_protobuf_ver)
    com_google_protobuf_urls = [
        "https://github.com/protocolbuffers/protobuf/archive/v{ver}.tar.gz".format(ver = com_google_protobuf_ver),
    ]
    http_archive(
        name = "com_google_protobuf",
        sha256 = com_google_protobuf_sha256,
        strip_prefix = com_google_protobuf_strip_prefix,
        urls = com_google_protobuf_urls,
    )

    http_archive(
        name = "com_github_grpc_grpc",
        strip_prefix = "grpc-1.45.0",
        sha256 = "ec19657a677d49af59aa806ec299c070c882986c9fcc022b1c22c2a3caf01bcd",
        urls = ["https://github.com/grpc/grpc/archive/refs/tags/v1.45.0.tar.gz"],
    )


