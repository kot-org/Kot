from pydub import AudioSegment
import struct

# Ouvrir le fichier MP3
audio = AudioSegment.from_file("audio.mp3")

# Convertir en PCM 32 bits
audio = audio.set_sample_width(2)
audio = audio.set_frame_rate(48000).set_channels(2)

# Stocker les données PCM dans un fichier binaire
with open("music.bin", "wb") as fichier:
    for sample in audio.get_array_of_samples():
        # Convertir l'échantillon en format binaire
        data = struct.pack("<h", sample)
        # Écrire l'échantillon dans le fichier binaire
        fichier.write(data)

