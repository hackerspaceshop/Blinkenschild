import sys


start= int(sys.argv[1])



for i in range(start+56, start+64):
	sys.stdout.write(str(i))
	sys.stdout.write(",")

sys.stdout.write("\n")


for i in reversed(range(start+48, start+56)):
        sys.stdout.write(str(i))
        sys.stdout.write(",")
sys.stdout.write("\n")






for i in range(start+40, start+48):
        sys.stdout.write(str(i))
        sys.stdout.write(",")

sys.stdout.write("\n")


for i in reversed(range(start+32, start+40)):
        sys.stdout.write(str(i))
        sys.stdout.write(",")
sys.stdout.write("\n")





for i in range(start+24, start+32):
        sys.stdout.write(str(i))
        sys.stdout.write(",")

sys.stdout.write("\n")


for i in reversed(range(start+16, start+24)):
        sys.stdout.write(str(i))
        sys.stdout.write(",")
sys.stdout.write("\n")



for i in range(start+8, start+16):
        sys.stdout.write(str(i))
        sys.stdout.write(",")

sys.stdout.write("\n")


for i in reversed(range(start, start+8)):
        sys.stdout.write(str(i))
        sys.stdout.write(",")
sys.stdout.write("\n")







