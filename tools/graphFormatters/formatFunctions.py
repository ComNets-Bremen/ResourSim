from matplotlib.ticker import FuncFormatter

def _KbFormatter(x, val):
    return str(val) + "kb"

KbFormatter = FuncFormatter(_KbFormatter)

