def matches(c, p):
    return c==p or p=='.'

def match(string, pattern):
    slen = len(string)+1
    plen = len(pattern)+1
    mt = [[False for p in range(plen)] for s in range(slen)]
    mt[0][0] = True
    for p in range(2, plen):
        if pattern[p-1] == '*':
            mt[0][p] = mt[0][p-2]

    for s in range(1, slen):
        for p in range(1, plen):
            if matches(string[s-1], pattern[p-1]):
                mt[s][p] = mt[s-1][p-1]
            elif pattern[p-1] == '*':
                if mt[s][p-2]:
                    mt[s][p] = True
                elif matches(string[s-1], pattern[p-2]):
                    mt[s][p] = mt[s-1][p]

    return mt[-1][-1]


if __name__ == '__main__':
    with open('test_cases.txt','r') as TCs:
        allgood = True
        for line in TCs:
            res,s,p = line.strip().split(',')
            out = match(s,p)
            if (res == 'True' and not out) or (res == 'False' and out):
                print('Expected: '+str(res)+' Got: '+str(out)+' for string: "'+s+'" Pattern: "'+p+'"')
                allgood = False
        if allgood:
            print("All test cases passed")
