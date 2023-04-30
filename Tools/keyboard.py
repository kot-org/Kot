import json

PathJson = "keys.json"

with open(PathJson) as JsonFile:
    DataFile = json.load(JsonFile)

CharArray = [[],[]]

for Key in DataFile:
    DoubleKey = Key["dk"]

    CharArray[DoubleKey].append(Key["char"])

with open("../Build/Bin/Modules/azerty.bin", "wb") as BinaryFile:

    for i in range(len(CharArray)):
        CharCount = len(CharArray[i])
        BinaryFile.write(bytes([CharCount]))
        for Char in CharArray[i]:
            if type(Char) is str:
                BinaryFile.write(Char.encode("utf-8"))
            elif type(Char) is int:
                BinaryFile.write(bytes([Char]))
