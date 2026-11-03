def slot_machine(bet, result):
    if result[1] == result[2] and result[1] == result[0]:
        return bet*5
    else:
        return 0