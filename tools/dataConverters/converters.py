def getFloatKbyte(value):
    try:
        v = float(value)
        if v == float("Inf"): # ignore inf
            return None
        else:
            return v/8/1000
    except ValueError:
        return None

