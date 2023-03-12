import subprocess
import sys
import os.path
import json

# Check if the image file exists
image_name = sys.argv[1]
image_size = sys.argv[2]
json_path = sys.argv[3]


if not os.path.exists(image_name):
    # If it doesn't exist, create a new image with the specified size
    subprocess.call(["fallocate", "-l", image_size, image_name])

    # Create a new GPT partition table on the image
    subprocess.call(["parted", "-s", image_name, "mklabel", "gpt"])

    # Create a new ext2 partition that takes up the entire image
    subprocess.call(["parted", "-s", image_name, "mkpart", "primary", "ext2", "2048s", "100%"])

    # Mount the first GPT partition of the image using the "mount" command
    subprocess.call(["sudo", "kpartx", "-av", image_name])
    output = subprocess.check_output(["sudo", "kpartx", "-l", image_name]).decode().strip()
    partition_name = "/dev/mapper/" + output.split()[0]

    # Format the partition with ext2 filesystem
    subprocess.call(["sudo", "mkfs.ext2", "-F", "-L", "kot", partition_name])

# Mount the first GPT partition of the image using the "mount" command
subprocess.call(["sudo", "kpartx", "-av", image_name])
output = subprocess.check_output(["sudo", "kpartx", "-l", image_name]).decode().strip()
partition_name = "/dev/mapper/" + output.split()[0]
subprocess.call(["sudo", "mkdir", "/mnt/kot-partition"])
subprocess.call(["sudo", "mount", partition_name, "/mnt/kot-partition"])

# Open the JSON file containing copy information
with open(json_path) as f:
    data = json.load(f)

# Iterate over each item in the JSON file and copy the files/folders to the mounted partition
for element in data:
    source_path = element['source']
    destination_path = "/mnt/kot-partition/" + element['destination']
    print("Copy " + element['source'] + " to " + element['destination'])
    if element['destination'] != "":
        subprocess.call(["sudo", "mkdir", "-m", "777", "-p", os.path.dirname(destination_path)])
    if source_path != "":
        subprocess.call(["sudo", "cp", "-r", source_path, destination_path])
        subprocess.call(["sudo", "chmod", "-R", "777", destination_path])

# Unmount the partition and remove device mappings using the "kpartx" command
subprocess.call(["sudo", "umount", "/mnt/kot-partition"])
subprocess.call(["sudo", "kpartx", "-dv", image_name])
subprocess.call(["sudo", "rmdir", "/mnt/kot-partition"])
