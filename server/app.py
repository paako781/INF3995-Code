from distutils.log import debug
import threading
from flask import Flask, jsonify, request, render_template, session
from flask_cors import CORS, cross_origin
import cflib.crtp
import sys
import cflib
from cflib.crazyflie import Crazyflie
from cflib.utils import uri_helper
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
import socket
from flask_socketio import SocketIO
from drone import Drone
from drone import *
from _thread import *
import threading
from pymongo import MongoClient
from bson.objectid import ObjectId
import yaml
from datetime import datetime
import time


HOST = "127.0.0.3"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

app = Flask(__name__)
# Avoid CORS errors
app.config['SECRET_KEY'] = 'my_app'
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

config = yaml.safe_load(open('database.yaml'))
client = MongoClient(config['uri'])
database = client['database']

CORS(app)

# Table for  physical drones instances
drones = []

nb_drone_sim = 0
simDrones = []
isReady = []


@socketio.on('connect')
def on_connect():
    print('Client connected')
    socketio.emit('my_response', {'data': 'Connected'})


@socketio.on('disconnect')
def on_disconnect():
    print("User disconnected!\nThe users are: ")


@socketio.on("initialize")
def on_initialize(methods=['GET', 'POST']):

    try:
        print('INITIALIZE SUCESS !!!')

        cflib.crtp.init_drivers()

        print('Scanning interfaces for Crazyflies...', file=sys.stderr)
        available = cflib.crtp.scan_interfaces(address=int('E7E7E7E7E7', 16))

        print('Crazyflies found:', file=sys.stderr)

        for i in available:
            print(i[0], file=sys.stderr)

        # Uncomment when you have two drones or one drone.
        drones.append(Drone(DRONE_ADRESS_1))
        drones.append(Drone(DRONE_ADRESS_2))
    except:
        print('JUST CONNNECT THE DONGLE !!!')

    # Emit to get json data for missions
    import log
    socketio.emit('my_logSim', {'file': log.getLog('logSimulation.json')})
    socketio.emit('my_logPhys', {'file': log.getLog('logPhysique.json')})

    return "Server running..."


@socketio.on("identify1")
def identify1(methods=['GET', 'POST']):
    for drones_ in drones:
        if drones_.drone_adress == DRONE_ADRESS_1:
            drones_.intercept_identify()
    return "Drone 1 identifié"


@socketio.on("identify2")
def identify2(methods=['GET', 'POST']):
    for drones_ in drones:
        if drones_.drone_adress == DRONE_ADRESS_2:
            drones_.intercept_identify()
    return "Drone 2 identifié"


@socketio.on("startMissionSimulation")
def s_start(methods=['GET', 'POST']):
    # initialize communication with simulation to get number of drones
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    message = "s"
    s.sendall(b"{}".join([message.encode()]))
    time.sleep(0.5)
    data = s.recv(100).decode()

    global nb_drone_sim
    nb_drone_sim = int(data)

    # emit number of drones to make de state table dynamic
    socketio.emit('nb_drones_sim', {'number': nb_drone_sim})

    global simDrones
    simDrones = [i for i in range(nb_drone_sim)]

    global isReady
    isReady = [False for i in range(nb_drone_sim)]

    for simDrone in simDrones:
        message = "t"
        t = threading.Thread(target=communication, args=([simDrone, message]))
        t.start()

    return "Simulation : lancer mission"


def communication(drone_number, message):
    print("drone_number", drone_number)
    global isReady
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT+drone_number))
        time.sleep(1)
        isReady[drone_number] = True
        while (True):

            s.send(b"{}".join([message.encode()]))
            time.sleep(1)
            if (all(isReady)):
                data = s.recv(1024).decode()

                # data has the following structuc "[state],[drone_id],[positionX],[positionY],[positionZ],[left_sensor], [back_sensor],[right_sensor],[front_sensor]"
                data = data.split('/')
                if '' in data:
                    data.remove('')
                data = data[0].split(',')
                data = data[:10]
                # sometimes data is empty
                if (len(data) == 10):
                    socketio.emit('state_simulation', {
                        'id': int(data[1]), 'state': data[0], 'battery_level': int(data[9])})

                    socketio.emit('log_simulations', {'state': data[0], 'id': int(data[1]), 'position_X':  float("{:.3f}".format(float(data[2]))),
                                                      'position_Y': float("{:.3f}".format(float(data[3]))), 'position_Z': float("{:.3f}".format(float(data[4]))),
                                                      'senseur_gauche': float("{:.3f}".format(float(data[5]))), 'senseur_arrière': float("{:.3f}".format(float(data[6]))),
                                                      'senseur_droit': float("{:.3f}".format(float(data[7]))), 'senseur_avant': float("{:.3f}".format(float(data[8])))})
                    import log
                    log.write_LogSimulation(hashkey, data)

    except Exception as x:
        print("oop probleme"+str(x))


@socketio.on("endMissionSimulation")
def s_end(methods=['GET', 'POST']):
    global nb_drone_sim
    global isReady
    isReady = [False for i in range(nb_drone_sim)]
    for simDrone in simDrones:
        message = "l"
        t = threading.Thread(target=communication, args=([simDrone, message]))
        t.start()

    import log
    socketio.emit('my_logSim', {'file': log.getLog('logSimulation.json')})
    return "Simulation : terminer Mission"


@socketio.on("returntoBaseSimulation")
def s_returnToBase(methods=['GET', 'POST']):
    isReady = [False for i in range(nb_drone_sim)]
    for simDrone in simDrones:
        message = "b"
        t = threading.Thread(target=communication, args=([simDrone, message]))
        t.start()

    return "Simulation : retour à la base."


@socketio.on("updateDroneSimulation")
def s_updateDrone(methods=['GET', 'POST']):
    return "Simulation : mise à jour des drones effectuée."


@socketio.on("startMissionPhysique")
def start(methods=['GET', 'POST']):

    for drone_index in drones:
        drone_index.intercept_start()
    import log
    socketio.emit('my_logPhys', {'file': log.getLog('logPhysique.json')})

    return "Mission avec les drones physiques lancée."


@socketio.on("endMissionPhysique")
def end(methods=['GET', 'POST']):
    for drone_index in drones:
        drone_index.intercept_terminate()

    return "Mission avec les drones physiques terminée."


@socketio.on("returntoBasePhysique")
def end(methods=['GET', 'POST']):
    for drone_index in drones:
        drone_index.intercept_returnBase()
    return "Retour à la base avec les drones physiques terminée."


@socketio.on("P2P")
def end(methods=['GET', 'POST']):
    print('Je recois le P2P')
    for drone_index in drones:
        drone_index.intercept_P2P()
    return "Commande P2P avec les drones physiques terminée."


@app.route('/')
def index():
    return "allo bienvenue"


@app.route('/dataMissions', methods=['POST', 'GET'])
def data():
    # POST data in db
    if request.method == 'POST':
        body = request.json
        tempsMission = str(datetime.now().isoformat(
            sep=" ", timespec="seconds"))
        tempsVol = body['tempsVol']
        nbsDronesPhys = body['nbsDronesPhys']
        nbsDronesSimul = body['nbsDronesSimul']
        distance = body['distance']
        mapData = body['mapData']

        data = database['dataMissions'].insert_one({
            'mapData': mapData,
            'tempsMission': tempsMission,
            'tempsVol': tempsVol,
            'nbsDronesPhys': nbsDronesPhys,
            'nbsDronesSimul': nbsDronesSimul,
            'distance': distance
        })

        return jsonify({
            'response': str(data.inserted_id),
            'tempsMission': tempsMission,
            'tempsVol': tempsVol,
            'nbsDronesPhys': nbsDronesPhys,
            'nbsDronesSimul': nbsDronesSimul,
            'distance': distance
        })

    # Get all data from db
    if request.method == 'GET':
        allData = database['dataMissions'].find()
        dataJson = []
        for data in allData:
            id = data['_id']
            tempsMission = data['tempsMission']
            tempsVol = data['tempsVol']
            nbsDronesPhys = data['nbsDronesPhys']
            nbsDronesSimul = data['nbsDronesSimul']
            distance = data['distance']
            mapData = data['mapData']
            dataDict = {
                'id': str(id),
                'tempsMission': tempsMission,
                'tempsVol': tempsVol,
                'nbsDronesPhys': nbsDronesPhys,
                'nbsDronesSimul': nbsDronesSimul,
                'distance': distance,
                'mapData': mapData
            }
            dataJson.append(dataDict)
        return jsonify(dataJson)


@app.route('/dataMissions/<string:id>', methods=['GET', 'DELETE', 'PUT'])
def onedata(id):

    # Get data by id
    if request.method == 'GET':
        data = database['dataMissions'].find_one({'_id': ObjectId(id)})
        id = data['_id']
        tempsMission = data['tempsMission']
        tempsVol = data['tempsVol']
        nbsDronesPhys = data['nbsDronesPhys']
        nbsDronesSimul = data['nbsDronesSimul']
        distance = data['distance']
        mapData = data['mapData']
        dataDict = {
            'id': str(id),
            'tempsMission': tempsMission,
            'tempsVol': tempsVol,
            'nbsDronesPhys': nbsDronesPhys,
            'nbsDronesSimul': nbsDronesSimul,
            'distance': distance,
            'mapData': mapData
        }
        print(dataDict)
        return jsonify(dataDict)

    # delete a data by id
    if request.method == 'DELETE':
        database['dataMissions'].delete_many({'_id': ObjectId(id)})
        return jsonify({'status': 'Data id: ' + id + ' deleted'})

    # Update data by id
    if request.method == 'PUT':
        body = request.json
        tempsVol = body['tempsVol']
        distance = body['distance']
        mapData = body['mapData']
        nbsDronesSimul = body['nbsDronesSimul']

        database['dataMissions'].update_one(
            {'_id': ObjectId(id)},
            {
                "$set": {
                    'tempsVol': tempsVol,
                    'distance': distance,
                    'mapData': mapData,
                    'nbsDronesSimul': nbsDronesSimul
                }
            }
        )
        print('\n # Update successful # \n')
        return jsonify({'status': 'Data id: ' + id + ' is updated!'})


if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', debug=True)
