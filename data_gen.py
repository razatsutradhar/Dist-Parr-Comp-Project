import os
import random
def basic_case_gen(n,m,k,name):
    try:
        os.mkdir(os.getcwd()+"/data/"+name)
    except:
        pass
    main_file = open(os.getcwd() + "/data/" + name + "/general","w")
    main_file.write("4")
    main_file.close()
    for i in range(n):
        file = open(os.getcwd() + "/data/" + name + "/{worker}".format(worker=i),"w")
        arr = []
        for j in range(m):
            arr.append(random.randint(0,k))
        arr.sort()
        s = "{m}".format(m=m)
        for j in range(m):
            s = s + "\n{value}".format(value = arr[j])
        file.write(s)
        file.close()
def main():
    basic_case_gen(4,10,10,"base")
main()
