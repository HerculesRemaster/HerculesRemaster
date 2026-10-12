// HERCULES.FS: 1,024 x 12-byte index records at offset 0 (id = CTPAX name hash, LE u32),
// payloads follow from 0x3000. See docs/archive-format.md.
#pragma endian little

import std.io;

fn hex32(u32 v) { return std::format("{:08X}", v); };

struct Entry {
    u32 id [[format("hex32")]];
    u32 offset;
    u32 size;
    if (size > 0) {
        char magic[4] @ offset;
        u8 payload[size] @ offset [[hidden]];
    }
} [[format("entry_name")]];

fn entry_name(ref Entry e) { return std::format("{:08X} @{:#x} +{}", e.id, e.offset, e.size); };

Entry index[1024] @ 0x00;
