from random import randint

a = ["foo", "bar", "faa"]
b = [0, 1]
c = [["foo"], ["bar"]]

def RandomSelection(array):
    return array[randint(0, len(array) - 1)]

print(RandSelect(a), RandSelect(b), RandSelect(c))
