const std = @import("std");
const mylib = @cImport({
    @cInclude("max-includes/ext.h");
});

const testing = std.testing;

export fn add(a: i32, b: i32) i32 {
    // mylib.a();
    return a + b;
}

test "basic add functionality" {
    try testing.expect(add(3, 7) == 10);
}
