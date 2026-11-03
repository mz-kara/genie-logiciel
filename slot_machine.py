def slot_machine(bet, result):
    if result[0] == result[1]:
        return bet*2
    else:
        return 0