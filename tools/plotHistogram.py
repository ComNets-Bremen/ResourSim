#!/usr/bin/env python3

import csv
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc
import matplotlib

import argparse
import configparser
import sys
import json

import graphFormatters
import dataConverters


parser = argparse.ArgumentParser(description="Create an output graph")
parser.add_argument("configuration", type=str, help="Config file")

args = parser.parse_args()

config = configparser.ConfigParser()

if len(config.read(args.configuration)) != 1:
    print("Cannot read config file \"" + args.configuration + "\". Aborting.")
    sys.exit(1)

datas = {}
binsize = config.getint("Graphing", "binsize")
if config.getboolean("General", "usetex"):
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

converter = getattr(dataConverters, config.get("Input", "valueConverter"))

with open(config.get("Input", "inputfile"), "r") as csvfile:
    reader = csv.reader(csvfile, delimiter=';', quotechar="\"")
    for row in reader:
        if row[1] == "vector":
            name = row[3]
            if name not in json.loads(config.get("Input", "keysUsed")):
                print("Not used:", name)
                continue
            data = row[-1].split()
            datas[name] = []
            for d in data:
                v = converter(d)
                if v:
                    datas[name].append(v)

minVal = None
maxVal = None

for dk in datas:
    for v in datas[dk]:
        if minVal == None:
            minVal = maxVal = v
        minVal = min(minVal, v)
        maxVal = max(maxVal, v)

if minVal < binsize:
    minVal = 0

minVal = np.ceil(minVal / binsize)*(binsize-1)
maxVal = np.ceil(maxVal / binsize)*(binsize+1)

bins = np.arange(minVal, maxVal, binsize)


if config.has_option("General", "plotStyle"):
    plt.style.use(config.get("General", "plotStyle"))

figsize = (config.getint("Graphing", "width"), config.getint("Graphing", "height"))

fig = plt.figure(figsize=figsize)
ax = fig.add_subplot(111)

plotDatas = []
plotLabels = []
for dk in datas:
    plotDatas.append(datas[dk])
    plotLabels.append(dk)
if config.has_option("Graphing", "labels"):
    labels = json.loads(config.get("Graphing", "labels"))
    if len(labels) != len(plotDatas):
        print("Length mismatch label vs datasets:", len(labels), len(plotDatas))
    else:
        plotLabels = labels
ax.hist(plotDatas, bins, density=True, alpha=0.75, label=plotLabels)

if config.has_option("Graphing", "xlimit"):
    ax.set_xlim(json.loads(config.get("Graphing", "xlimit")))

if config.has_option("Graphing", "ylimit"):
    ax.set_ylim(json.loads(config.get("Graphing", "ylimit")))

if config.has_option("Graphing", "showLegend"):
    loc = config.get("Graphing", "legendPos", fallback="best")
    plt.legend(loc=loc)

if config.has_option("Graphing", "xLabel"):
    ax.set_xlabel(config.get("Graphing", "xLabel"))

if config.has_option("Graphing", "yLabel"):
    ax.set_ylabel(config.get("Graphing", "yLabel"))

if config.has_option("Graphing", "title"):
    ax.set_title(config.get("Graphing", "title"))

if config.get("Graphing", "yFormatterMajor", fallback=None) in dir(graphFormatters):
    func = getattr(graphFormatters, config.get("Graphing", "yFormatterMajor"))
    if not isinstance(func, matplotlib.ticker.Formatter):
        func = func()
    ax.yaxis.set_major_formatter(func)

if config.get("Graphing", "xFormatterMajor", fallback=None) in dir(graphFormatters):
    func = getattr(graphFormatters, config.get("Graphing", "xFormatterMajor"))
    if not isinstance(func, matplotlib.ticker.Formatter):
        func = func()
    ax.xaxis.set_major_formatter(func)

if config.get("Graphing", "yFormatterMinor", fallback=None) in dir(graphFormatters):
    func = getattr(graphFormatters, config.get("Graphing", "yFormatterMinor"))
    if not isinstance(func, matplotlib.ticker.Formatter):
        func = func()
    ax.yaxis.set_minor_formatter(func)

if config.get("Graphing", "xFormatterMinor", fallback=None) in dir(graphFormatters):
    func = getattr(graphFormatters, config.get("Graphing", "xFormatterMinor"))
    if not isinstance(func, matplotlib.ticker.Formatter):
        func = func()
    ax.xaxis.set_minor_formatter(func)

plt.grid(config.getboolean("Graphing", "grid", fallback=False))


fig.tight_layout()
if config.get("Output", "outputfile"):
    plt.savefig(
            config.get("Output", "outputfile"),
            dpi=config.getint("Output", "dpi", fallback=None)
            )
if config.getboolean("Output", "show", fallback=True):
    plt.show()
