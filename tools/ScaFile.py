import csv

class ScaFile(object):
    def __init__(self, filename=None):
        self.__filename = filename

    def __repr__(self):
        return "File:" + self.__filename

    def getFile(self):
        return self.__filename

    def setFile(self, filename):
        self.__filename = filename

    def get(self, o):
        if self.__filename == None:
            return None
        results = []
        with open(self.__filename) as fh:
            csvHandler = csv.reader(fh, delimiter=" ", escapechar="\\", )
            for line in csvHandler:
                if len(line) < 2:
                    continue
                if line[0] == o:
                    results.append(line[1:])
        return results

    def getScalars(self):
        keys = []
        for scalar in self.get("scalar"):
            keys.append(scalar[1])
        return keys


    def getScalar(self, key):
        for scalar in self.get("scalar"):
            if scalar[1] == key:
                return self.__getNumeric(scalar[2])
        return None

    def getParams(self):
        result = []
        for par in self.get("param"):
            result.append(par[0])
        return result

    def getParam(self, key):
        for par in self.get("param"):
            if par[0].endswith(key):
                return self.__getNumeric(par[1])
        return None

    def getAttrs(self):
        result = []
        for attr in self.get("attr"):
            result.append(attr[0])
        return result

    def getAttr(self, key):
        for attr in self.get("attr"):
            if attr[0] == key:
                return self.__getNumeric(" ".join(attr[1:]))

    def getRun(self):
        return self.__getNumeric(self.get("run")[0][0])

    def getVersion(self):
        return self.__getNumeric(self.get("version")[0][0])


    def __getNumeric(self, value):
        value = str(value)
        if value.replace(".", "", 1).isnumeric():
            value = float(value)
            if value.is_integer():
                value = int(value)
        return value


