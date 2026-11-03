def slot_machine(bet, result):
    if result[1] == result[2]:
        return bet*2
    else:
        return 0