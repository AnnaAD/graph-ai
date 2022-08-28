
vertices = 10000

import random
output = ""
seen = set()
for i in range(10000000):
    num1 = random.randint(0,vertices)
    num2 = random.randint(0,vertices)
    if(num1 != num2 and (str(num1) + "	" + str(num2)) not in seen and (str(num2) + "	" + str(num1)) not in seen):
        seen.add(str(num1) + "	" + str(num2))
        output += str(num1) + "	" + str(num2) + "\n"

f = open("input.txt", "w")
f.write(output)
f.close()