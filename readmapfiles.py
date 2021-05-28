from os import listdir

path = "../../../gpfs/gpfs0/a.klepach/DATA_short"

path = "./DATA/DATA_short"
files = listdir(path)

f = open("filemapnames.txt", "a")
for file in files:
    if file[-3:] == 'map':
        f.write(path + '/' + file + "\n")
f.close()
