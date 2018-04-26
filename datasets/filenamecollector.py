import os

files = os.listdir("wikidump/")

with open("filenames", "w") as text_file:
    text_file.write(str(len(files)) + " ")
    for f in files:
        text_file.write(f + " ")
