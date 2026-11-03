def slot_machine(bet, result):
    if result[1] == result[2] and result[1] == result[0] and result[0] == '$':
        return bet*20
    else:
        return 0