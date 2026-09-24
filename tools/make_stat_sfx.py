#!/usr/bin/env python3
"""Synthesize the two stat-change SFX (gain/loss) as mono 8-bit PCM WAVs —
matching third_party/butano/examples/audio's alert.wav format (no source art
exists for these, so they're generated tones rather than converted assets).

Usage:
    python3 tools/make_stat_sfx.py
"""
import math
import wave
from pathlib import Path

RATE = 22050
OUT_DIR = Path(__file__).resolve().parent.parent / "assets" / "audio"


def tone(freq_start: float, freq_end: float, duration: float, volume: float = 0.6) -> list[int]:
    samples = []
    count = int(RATE * duration)

    for i in range(count):
        t = i / RATE
        progress = i / count
        freq = freq_start + (freq_end - freq_start) * progress
        # Simple decaying envelope so each tone doesn't click at the end.
        envelope = 1.0 - progress
        value = math.sin(2 * math.pi * freq * t) * volume * envelope
        samples.append(int(127 + value * 127))

    return samples


def write_wav(path: Path, samples: list[int]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    with wave.open(str(path), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(RATE)
        w.writeframes(bytes(samples))

    print(f"wrote {path} ({len(samples)} samples, {len(samples) / RATE:.2f}s)")


def main() -> None:
    # Rising two-note chirp for gaining a stat.
    gain = tone(440, 660, 0.08) + tone(660, 880, 0.10)
    write_wav(OUT_DIR / "stat_gain.wav", gain)

    # Falling buzz for losing a stat.
    loss = tone(392, 220, 0.22, volume=0.7)
    write_wav(OUT_DIR / "stat_loss.wav", loss)


if __name__ == "__main__":
    main()
