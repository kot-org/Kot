from pydub import AudioSegment

# Ouvrir le fichier MP3
audio = AudioSegment.from_file("audio.mp3")

# Convertir en PCM 32 bits
audio = audio.set_sample_width(2)
audio = audio.set_frame_rate(48000).set_channels(2)

# Stocker les données PCM dans un fichier texte
size = 0
sizemax = 1048576
with open("music.h", "w") as fichier:
    fichier.write("int16_t Music[] = {")
    for sample in audio.get_array_of_samples():
        fichier.write(str(sample) + ",")
        size += 2
        if(size >= sizemax):
            break
    fichier.write("0};")
print(size)
