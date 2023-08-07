#!/usr/bin/env python3

import glob
import matplotlib.pyplot as plt
import numpy as np

from ScaFile import ScaFile

FILE_PATTERN = "*.sca"

results = {}

# callback for graph
def getXYValue(datalines):
    res = {}

    for line in datalines:
        if line["user"] not in res:
            res[line["user"]] = []
        res[line["user"]].append((
                line["screenCorrectionFactor"],
                line["Number of cancelled background Events"]/line['Number of Background Events']
                ))
    for u in res:
        res[u].sort()
    return res

# Callback for graph
def getBgCollisions(datalines):
    res = {}

    for line in datalines:
        if line["user"] not in res:
            res[line["user"]] = []
        res[line["user"]].append((
            line["screenCorrectionFactor"],
            line["#collisionUser"]
            ))
    for u in res:
        res[u].sort()
    return res

# Connect graph selection and data with cb function
graphSelectionPattern = [
        {
        "enableBackgroundOptimizations" : "true",
        "_values" : [],
        "_handler" : getXYValue,
        "_label_prefix" : "Optimized: ",
        },{
        "enableBackgroundOptimizations" : "true",
        "_values" : [],
        "_handler" : getBgCollisions,
        "_label_prefix" : "BG Collisions, Optimized: ",
        "_twinx" : True,
        "_ls" : ":"
        },

        ]




for f in glob.glob(FILE_PATTERN):
    sf = ScaFile(f)
    key = sf.getRun()
    results[key] = {}
    if "jd.xml" in sf.getParam("eventFilename"): results[key]["user"] = "user 1"
    elif "anna.xml" in sf.getParam("eventFilename"): results[key]["user"] = "user 2"
    elif "iz.xml" in sf.getParam("eventFilename"): results[key]["user"] = "user 3"
    else: results[key]["user"] = "unknown"

    results[key]["screenCorrectionFactor"] = sf.getParam("screenCorrectionFactor")
    results[key]["enableBackgroundOptimizations"] = sf.getParam("enableBackgroundOptimizations")
    results[key]["Number of Background Events"] = sf.getScalar("Number of Background Events")
    results[key]["Number of cancelled background Events"] = sf.getScalar("Number of cancelled background Events")
    results[key]["#collisionUser"] = sf.getScalar("#collisionUser")
    results[key]["#collisionBackground"] = sf.getScalar("#collisionBackground")

for pattern in graphSelectionPattern:
    for data in results:
        addValue = True
        for value in results[data]:
            if value in pattern and pattern[value] != results[data][value]:
                addValue = False
        if addValue:
            pattern["_values"].append(results[data])



figure = plt.figure(figsize=(12, 6)) # size of resulting figure in inches
ax = figure.add_subplot(111) # Add just one single subplot
ax2 = None

for graph in graphSelectionPattern:
    if "_twinx" in graph:
        ax2 = ax.twinx()
        break

lines = []

for graph in graphSelectionPattern:
    graphs = graph["_handler"](graph["_values"])

    users = list(graphs.keys())
    users.sort()

    usedAxis = ax
    if "_twinx" in graph and graph["_twinx"]:
        usedAxis = ax2

    for user in users:
        x, y = zip(*graphs[user])
        label = user
        if "_label_prefix" in graph:
            label = graph["_label_prefix"] + label
        ls = None
        if "_ls" in graph:
            ls = graph["_ls"]

        lines.append(usedAxis.plot(x, y, ls=ls, label=label)[0])

ax.set_ylim((0,1))

labels = [l.get_label() for l in lines]

ax.legend(lines, labels, loc='best')
plt.tight_layout()
plt.show()
#plt.savefig("test.pdf")
