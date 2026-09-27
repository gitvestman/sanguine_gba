#!/usr/bin/env python3
"""Wrap one raw 8-bit unsigned mono PCM clip as a minimal Scream Tracker 3
module (.s3m) that just plays it once and loops forever — for reusing a
pre-recorded piece of music as Butano bn::music (Direct Sound via Maxmod),
since that path needs a real tracker module (.mod/.s3m/.xm/.it), not a plain
audio file: there is no GBA-side MP3/OGG/WAV-as-background-music decoder.

The trick: one instrument (the whole clip, with its own sample loop points
covering nearly all of it) is triggered by a single note in pattern 0; every
other pattern in the order list is completely silent. The note is never
re-triggered — and so never audibly restarts the clip — until the order
list actually runs out, which `--loop-minutes` sizes to comfortably outlast
any real play session. The S3M layout here was derived by parsing (not
guessing) third_party/butano/examples/core/audio/soda7_xcopy_ohc.s3m, a
known-good file already proven to work with this project's own mmutil.

Usage:
    python3 tools/wav_to_s3m_loop.py <input.raw> <sample_rate> <output.s3m> \
        [--name "Song Name"] [--loop-minutes 15]

<input.raw> must already be raw 8-bit unsigned mono PCM (e.g. via
`ffmpeg -i in.ogg -t 40 -ac 1 -ar 10512 -f u8 out.raw`) — this tool only
wraps it, it doesn't transcode.
"""
import argparse
import struct
from pathlib import Path

ROWS_PER_PATTERN = 64


def pack_pattern(note_channel=None, note=None, instrument=None):
    """Builds one packed S3M pattern's row data. If note_channel is given,
    row 0 plays `note`+`instrument` on that channel; every other row (and
    every row at all, if note_channel is None) is empty. Each row ends with
    a single 0x00 terminator byte."""
    rows = bytearray()

    for row in range(ROWS_PER_PATTERN):
        if row == 0 and note_channel is not None:
            # Entry byte: bits 0-4 channel, bit 5 note+instrument follow.
            rows.append(0x20 | note_channel)
            rows.append(note)
            rows.append(instrument)

        rows.append(0x00)  # end of row

    return bytes(rows)


def build_s3m(pcm: bytes, sample_rate: int, name: str, loop_minutes: float) -> bytes:
    # Row duration at speed=6, tempo=125 (S3M defaults): 6 * (2500/125) ms.
    row_ms = 6 * (2500 / 125)
    pattern_ms = ROWS_PER_PATTERN * row_ms
    filler_patterns = max(1, int((loop_minutes * 60_000) / pattern_ms))

    trigger_pattern = pack_pattern(note_channel=0, note=0x40, instrument=1)  # C-4
    silent_pattern = pack_pattern()

    orders = bytes([0] + [1] * filler_patterns)
    ordnum = len(orders)

    header = bytearray(0x60)
    header[0:28] = name.encode("ascii", "replace")[:28].ljust(28, b"\0")
    header[0x1C] = 0x1A
    header[0x1D] = 16  # "type" byte — copied from the known-good reference file
    struct.pack_into("<HHHHHH", header, 0x20, ordnum, 1, 2, 0, 0x5128, 2)
    header[0x2C:0x30] = b"SCRM"
    # Master volume's bit 7 enables stereo — our source is mixed to mono
    # (see the ffmpeg -ac 1 step) and channel 0 is typed as a "left" PCM
    # channel below, so leaving stereo on made the mix play hard-panned
    # instead of centered. Clearing it makes the mixer output the one
    # channel identically to both speakers.
    struct.pack_into("<BBBBBB", header, 0x30, 64, 6, 125, 0x30, 8, 0x00)
    # 0x36..0x3D reserved (already zero), 0x3E special (already zero)
    channels = bytes([0]) + bytes([0xFF]) * 31
    header[0x40:0x60] = channels

    body = bytearray()
    body += orders

    # Placeholders for the instrument/pattern parapointer tables — patched
    # once we know where each block actually lands.
    ins_ptr_pos = len(header) + len(body)
    body += b"\0\0"
    pat_ptr_pos = len(header) + len(body)
    body += b"\0\0\0\0"

    def pad_to_paragraph(buf: bytearray):
        pad = (-len(buf)) % 16
        buf += b"\0" * pad

    pad_to_paragraph(body)
    ins_offset = len(header) + len(body)

    instrument = bytearray(0x50)
    instrument[0] = 1  # type: PCM sample
    # 0x01..0x0C dosname stays zero
    sample_offset_placeholder = len(header) + len(body) + len(instrument)
    # MemSeg patched below, once padded to its own paragraph.
    struct.pack_into("<III", instrument, 0x10, len(pcm), 0, len(pcm))
    struct.pack_into("<BBBB", instrument, 0x1C, 64, 0, 0, 1)  # vol, _, pack, flags(loop on)
    struct.pack_into("<I", instrument, 0x20, sample_rate)
    instrument[0x30:0x30 + 28] = name.encode("ascii", "replace")[:28].ljust(28, b"\0")
    instrument[0x4C:0x50] = b"SCRS"
    body += instrument

    pad_to_paragraph(body)
    pat0_offset = len(header) + len(body)
    body += struct.pack("<H", len(trigger_pattern)) + trigger_pattern

    pad_to_paragraph(body)
    pat1_offset = len(header) + len(body)
    body += struct.pack("<H", len(silent_pattern)) + silent_pattern

    pad_to_paragraph(body)
    sample_offset = len(header) + len(body)
    body += pcm

    # Patch parapointers now that every block's final offset is known.
    struct.pack_into("<H", body, ins_ptr_pos - len(header), ins_offset // 16)
    struct.pack_into("<HH", body, pat_ptr_pos - len(header), pat0_offset // 16, pat1_offset // 16)

    para = sample_offset // 16
    assert para <= 0xFFFF, "sample too large for this minimal S3M's 16-bit parapointer"
    ins_body_offset = ins_offset - len(header)
    struct.pack_into("<BH", body, ins_body_offset + 0x0D, 0, para)

    return bytes(header) + bytes(body)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_raw", type=Path)
    parser.add_argument("sample_rate", type=int)
    parser.add_argument("output_s3m", type=Path)
    parser.add_argument("--name", default="Sanguine Theme")
    parser.add_argument("--loop-minutes", type=float, default=15.0)
    args = parser.parse_args()

    pcm_data = args.input_raw.read_bytes()
    s3m = build_s3m(pcm_data, args.sample_rate, args.name, args.loop_minutes)
    args.output_s3m.parent.mkdir(parents=True, exist_ok=True)
    args.output_s3m.write_bytes(s3m)
    print(f"{args.input_raw} ({len(pcm_data)} bytes @ {args.sample_rate}Hz) -> "
          f"{args.output_s3m} ({len(s3m)} bytes)")
