from colorsys import hsv_to_rgb
from dataclasses import dataclass
from distutils.log import log
import json
import os.path
from time import time
import threading
# from tkinter.messagebox import RETRY

# Create a empty dict for Logs
log_physique_historical = dict()
log_simulation_historical = dict()

lock = threading.Lock()
# Lock for json write log
lock_physique = threading.Lock()


def is_log_exist(filename):
    is_existed = os.path.exists(filename)
    return is_existed


def getLog(filename):
    with open(filename) as json_file:
        if (filename == 'logPhysique.json'):
            log_physique_historical = json.load(json_file)
            return log_physique_historical
        if (filename == 'logSimulation.json'):
            log_simulation_historical = json.load(json_file)
            return log_simulation_historical


def write_LogPhysique(timestamp, coord):
    (handler_, front, back, left, right, up, x, y, z, id) = coord

    if (is_log_exist('logPhysique.json')):
        lock_physique.acquire()

        json_file = open('logPhysique.json')
        log_physique_historical = json.load(json_file)
        json_file.close()

        if not str(timestamp) in log_physique_historical.keys():
            log_physique_historical[str(timestamp)] = []
            log_physique_historical[str(timestamp)].append(
                (convert_to_centimeter(front), convert_to_centimeter(back), convert_to_centimeter(left), convert_to_centimeter(right), convert_to_centimeter(up),
                 float("{:.3f}".format(x)),  float("{:.3f}".format(y)),  float("{:.3f}".format(z)), id))
        else:
            log_physique_historical[str(timestamp)].append(
                (convert_to_centimeter(front), convert_to_centimeter(back), convert_to_centimeter(left), convert_to_centimeter(right), convert_to_centimeter(up),
                 float("{:.3f}".format(x)),  float("{:.3f}".format(y)),  float("{:.3f}".format(z)), id))

        json_object = json.dumps(log_physique_historical, indent=4)

        outfile = open("logPhysique.json", "w")
        outfile.write(json_object)
        outfile.close()
        lock_physique.release()


def write_LogSimulation(timestamp, coord):
    
    (state, id, x, y, z, sensG, sensArriere, sensD, sensAvant, battery ) = coord

    if (is_log_exist('logSimulation.json')):
        
        lock.acquire()

        json_file = open('logSimulation.json') 
        log_simulation_historical = json.load(json_file)
        json_file.close() 
   
        if not str(timestamp) in log_simulation_historical.keys():
            log_simulation_historical[str(timestamp)] = []
            log_simulation_historical[str(timestamp)].append(
                (int(id), state, float("{:.3f}".format(float(x))), float("{:.3f}".format(float(y))), float("{:.3f}".format(float(z))),
                    float("{:.3f}".format(float(sensG))), float("{:.3f}".format(float(sensArriere))), float("{:.3f}".format(float(sensD))), float("{:.3f}".format(float(sensAvant)))))
        else:
            log_simulation_historical[str(timestamp)].append(
                (int(id), state, float("{:.3f}".format(float(x))), float("{:.3f}".format(float(y))), float("{:.3f}".format(float(z))),
                    float("{:.3f}".format(float(sensG))), float("{:.3f}".format(float(sensArriere))), float("{:.3f}".format(float(sensD))), float("{:.3f}".format(float(sensAvant)))))


        json_object = json.dumps(log_simulation_historical, indent=4)

        outfile= open("logSimulation.json", "w") 
        outfile.write(json_object)
        outfile.close()
        
        lock.release()
   


def convert_to_centimeter(measure):
    return measure / 1000
