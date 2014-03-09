# Module for parsing VATA configuration file

import os

# vata tests modules
from vata_configuration import VATAConfiguration

class ConfigParser:
    def parse(self, configFilePath):
        if not os.path.isfile(configFilePath):
            raise Exception("{0} is not a file".format(0))

        configObj = VATAConfiguration()
        configFile = open(configFilePath,'r')
        vataConfig = VATAConfiguration()

        for i in configFile:
            i = i.strip()
            i = i.replace(' ','')
            commentFree = i.split('#')[0]
            if len(commentFree) > 0:
                self.parseOption(commentFree, vataConfig)
        return vataConfig

    def parseOption(self, option, vataConfig):
        valueIndex = 1
        parsedList = option.split('=')
        option = parsedList[0]

        if option == "representation":
            vataConfig.setRepres(parsedList[valueIndex].split(','))
        elif option == "operation":
            vataConfig.setOperation(parsedList[valueIndex])
        elif option == "options":
            vataConfig.setOptions(parsedList[valueIndex].split(','))
        elif option == "time":
            if parsedList[valueIndex] == "true":
                vataConfig.setTime(True)
        elif option == "algorithm":
            vataConfig.setAlgorithm(parsedList[valueIndex])
        elif option == "direction":
            vataConfig.setDirection(parsedList[valueIndex])
        elif option == "simulation":
            if parsedList[valueIndex] == "true":
                vataConfig.setSimulation(parsedList[valueIndex])
        elif option == "searchOrder":
            vataConfig.setSearchOrder(parsedList[valueIndex])

        return vataConfig
