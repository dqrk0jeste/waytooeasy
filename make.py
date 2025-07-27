from aods import (
    Context,
    BuildType,
    run,
    pkgconfig_get_variable,
    assert_installed,
    default_flags,
)
import sys


def wayland_init_client_protocol(p: str, dest: str):
    dir = pkgconfig_get_variable("wayland-protocols", "pkgdatadir")

    path = "/".join([dir, p])

    name = p.split("/")[-1]
    header = dest + "/" + name.replace(".xml", "-protocol.h")
    code = dest + "/" + name.replace(".xml", "-protocol.c")

    result = run(["wayland-scanner", "client-header", path, header])
    if result.returncode != 0:
        raise Exception(f"wayland protocol initialization failed for `{p}`!")

    result = run(["wayland-scanner", "private-code", path, code])
    if result.returncode != 0:
        raise Exception(f"wayland protocol initialization failed for `{p}`!")

    return (header, code)


build_type = BuildType.RELEASE if "--release" in sys.argv else BuildType.DEBUG


ctx = Context.default("main")

assert_installed("wayland-protocols")

protocols = [
    "stable/xdg-shell/xdg-shell.xml",
]

for protocol in protocols:
    _, code = wayland_init_client_protocol(protocol, ctx.build_dir)

    ctx.add_source(code)


ctx.add_dependency(
    [
        "wayland-client",
        "xkbcommon",
    ]
)

ctx.add_include(
    [
        ".",
        "util",
        ctx.build_dir,
    ]
)

ctx.add_source(
    [
        "app.c",
        "buffer.c",
        "color.c",
        "event_loop.c",
        "memory.c",
        "wayland.c",
        "keyboard.c",
        "pointer.c",
    ]
)

ctx.add_flag(default_flags(build_type))

ctx.add_flag("-lm")

ctx.build()
