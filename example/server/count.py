import sys
dict={}
for line in sys.stdin:
    line=line[:-1]
    linel=line.split()
    for each in linel:
        if each in dict:
	    dict[each]+=1
	else:
	    dict[each]=1
for each in dict:
    print each+"|||"+str(dict[each])
