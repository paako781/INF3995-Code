from sqlite3 import Timestamp
import cflib.crtp
import sys
import struct
import cflib
from cflib.crazyflie import Crazyflie
from cflib.utils import uri_helper
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from flask import Flask
from flask_cors import CORS
from log import write_LogPhysique
from datetime import datetime


app = Flask(__name__)
# Avoid CORS errors
CORS(app)

# Adapt  drones 's adress depending off your adress
DRONE_ADRESS_1 = 'radio://0/80/2M/E7E7E7E712'
DRONE_ADRESS_2 = 'radio://0/80/2M/E7E7E7E7A2'
INDEX_OF_FIRST_DRONE = 0
INDEX_OF_SECOND_DRONE = 1

# Generates unique timeStamp for drones registry in log historical
hashkey = datetime.now()


class Drone:
    is_drone_initialized = False

    def __init__(self, drone_adress):
        print("La classe est crée")
        self.isconnected = False
        self.is_link_opened = False

        if (Drone.is_drone_initialized):
            cflib.ctrp.init_drivers()

        self.drone_adress = drone_adress
        self.url = uri_helper.uri_from_env(default=self.drone_adress)
        self._cf = Crazyflie()
        self._cf.open_link(self.url)

        # Callback for getting data from firmware
        self._cf.appchannel.packet_received.add_callback(
            self._app_packet_received)

    def intercept_identify(self):
        packet = struct.pack("<3s", b'ID')
        self._cf.appchannel.send_packet(packet)

    def intercept_start(self):
        packet = struct.pack("<3s", b'SM')
        self._cf.appchannel.send_packet(packet)

    def intercept_terminate(self):
        packet = struct.pack("<3s", b'TM')
        self._cf.appchannel.send_packet(packet)

    def intercept_returnBase(self):
        packet = struct.pack("<3s", b'RB')
        self._cf.appchannel.send_packet(packet)

    def intercept_P2P(self):
        packet = struct.pack("<3s", b'PP')
        self._cf.appchannel.send_packet(packet)

    def sendPacket(self, message):
        packet = struct.pack('<' + str(len(message)) + 's',
                             bytes(message, encoding='utf-8'))
        self._cf.appchannel.send_packet(packet)

    # Méthode de reception pour les drones
    def _app_packet_received(self, data):
        handler = int.from_bytes(data[:1], byteorder='little')

        drone_id = any

        if self.drone_adress == DRONE_ADRESS_1:
            drone_id = 1
        else:
            drone_id = 2

        if handler == 1:
            (handler, mainContent) = struct.unpack("<b25s", data)
            import app
            app.socketio.emit('state_physique', {
                              'id': drone_id, 'state': mainContent[:mainContent.index(0)].decode('UTF-8')})

        elif handler == 0:
            log = struct.unpack("<bHHHHHfff", data)
            (handler_, front, back, left, right, up, x, y, z) = log

            import app
            # Emit log for physiques drones
            log_new = list(log)

            log_new.insert(9, drone_id)

            # Write drone's log in each json historic depending of the adress
            write_LogPhysique(hashkey, log_new)

            # Send relative position and Coord of each physic drone
            app.socketio.emit('crazyflie_log', {'id': drone_id,
                                                'front': front/1000, 'back': back/1000, 'left': left/1000, 'right': right/1000, 'up': up/1000,
                                                'x': float("{:.3f}".format(x)), 'y': float("{:.3f}".format(y)), 'z': float("{:.3f}".format(z))})

        elif handler == 2:
            packet = struct.unpack("<bb", data)
            (handler_1, battery) = packet

            import app
            app.socketio.emit('battery_level_physique', {
                              'id': drone_id, 'battery_level': battery})


if __name__ == '__main__':
    app.run(host='0.0.0.0')
