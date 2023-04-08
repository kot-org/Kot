from pydub import AudioSegment
import struct

audio = AudioSegment.from_file("audio.mp3")

audio = audio.set_sample_width(2)
audio = audio.set_frame_rate(48000).set_channels(2)

size = 0

with open("music.bin", "wb") as fichier:
    for sample in audio.get_array_of_samples():
        if size >= (1048576 / 2):
            break
        size += 1
        data = struct.pack("<h", sample)
        fichier.write(data)

