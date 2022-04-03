import os
import sys

InputFolderPath = sys.argv[1]
OutputFilePath = sys.argv[2]
InitFileName = sys.argv[3]

MaxNameSize = 36
Version = 1
HeaderSize = 29
FileHeaderSize = MaxNameSize + 8

def InitializeFileSystem(Self, FileCount, InitFilePosition):
    # Signature
    Self.write(b"R")
    Self.write(b"A")
    Self.write(b"M")
    Self.write(b"F")
    Self.write(b"S")

    Self.write(Version.to_bytes(8, 'little'))

    Self.write(FileCount.to_bytes(8, 'little'))

    Self.write(InitFilePosition.to_bytes(8, 'little'))

    print("Creating file system...\n")

def AddFile(Self, Name, Size, Data):
    print("Adding : " + Name)
    Self.write(Name.encode())
    Self.write(Size.to_bytes(8, 'little'))
    Self.write(Data)

def AddFileFromDisk(Self, Path):
    InputFile = open(Path, "rb")
    Size = os.stat(Path).st_size
    Data = InputFile.read()
    InputFile.close()
    Path = Path.split('/') 
    Name = Path[-1][:MaxNameSize] + (" " * (MaxNameSize - len(Path[-1])))
    AddFile(Self, Name, Size, Data) 

OuputFile = open(OutputFilePath, "wb")

InputFiles = os.listdir(InputFolderPath)
FileCount = len(InputFiles)

InitFilePos = HeaderSize
InitFileFound = 0

for i in range(FileCount):
    if InputFiles[i] == InitFileName:
        InitFileFound = 1
        break
    Size = os.stat(InputFolderPath + InputFiles[i]).st_size
    InitFilePos += FileHeaderSize
    InitFilePos += Size

print(InitFilePos)
if InitFileFound != 1:
    print("Warning : no start file found\n")

InitializeFileSystem(OuputFile, FileCount, InitFilePos)

for i in range(FileCount):
    AddFileFromDisk(OuputFile, InputFolderPath + InputFiles[i])

OuputFile.close()
print("RamFS creating with success!")