// see https://github.com/zig-gamedev/zig-gamedev/blob/main/libs/zsdl/build.zig
const std = @import("std");
const assert = std.debug.assert;

inline fn thisDir() []const u8 {
    return comptime std.fs.path.dirname(@src().file) orelse ".";
}
// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // set a preferred release mode, allowing the user to decide how to optimize.
    const optimize = b.standardOptimizeOption(.{});
    const lib = b.addSharedLibrary(.{
        .name = "zigtest",
        // In this case the main source file is merely a path, however, in more
        // complicated build scripts, this could be a generated file.
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    lib.linkLibC();
    lib.linkSystemLibrary("MaxApi");

    switch (target.result.os.tag) {
        .windows => {
            assert(target.result.cpu.arch.isX86());
            const maxapi_dll_dir = "c:/Program Files/Cycling '74/Max 8/resources/support/";
            lib.addLibraryPath(.{ .cwd_relative = maxapi_dll_dir });
            lib.addIncludePath(.{ .cwd_relative = "C:/Users/FrancescElies/src/oss/max-sdk/source/max-sdk-base/c74support" });
            // dll needs to be copied to the zig-cache/bin folder
            b.installFile(maxapi_dll_dir ++ "/MaxApi.dll", "bin/MaxApi.dll");

            std.fs.cwd().makePath("zig-out\\bin") catch unreachable;
            const src_dir = std.fs.cwd().openDir(maxapi_dll_dir, .{}) catch unreachable;
            src_dir.copyFile("MaxApi.dll", std.fs.cwd(), "zig-out\\bin\\MaxApi.dll", .{}) catch unreachable;
        },
        else => {
            unreachable;
        },
    }
    b.installArtifact(lib);

    // lib.linkLibrary("MaxApi");

    // This declares intent for the library to be installed into the standard
    // location when the user invokes the "install" step (the default step when
    // running `zig build`).
    b.installArtifact(lib);

    // Creates a step for unit testing. This only builds the test executable
    // but does not run it.
    // const main_tests = b.addTest(.{
    //     .root_source_file = .{ .path = "src/main.zig" },
    //     .target = target,
    //     .optimize = optimize,
    // });
    //
    // const run_main_tests = b.addRunArtifact(main_tests);
    //
    // // This creates a build step. It will be visible in the `zig build --help` menu,
    // // and can be selected like this: `zig build test`
    // // This will evaluate the `test` step rather than the default, which is "install".
    // const test_step = b.step("test", "Run library tests");
    // test_step.dependOn(&run_main_tests.step);
}
