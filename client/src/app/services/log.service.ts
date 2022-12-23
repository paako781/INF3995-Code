import { Injectable } from '@angular/core';
import { map } from 'rxjs/operators';
import { Drone } from '../classes/drone/drone';
import {
  Coord,
  DronePosition,
  SensorData,
} from '../classes/dronePosition/dronePosition';
import { Mode } from '../classes/enum';
import { SocketService } from './socket.service';

@Injectable({
  providedIn: 'root',
})
export class LogService {
  // Map to registrer json data
  mapPhysique = new Map<string, number[][]>();
  mapSimulation = new Map<string, (string | number)[][]>();
  log: DronePosition[][];
  curLog: DronePosition[];

  constructor(public socketService: SocketService) {
    this.log = [[], []];
    this.curLog = [];
   
  }

  public receiveLog(eventname: string): void {
    this.socketService.socket
      .fromEvent(eventname)
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        if (eventname === 'my_logPhys') {
          this.mapPhysique = new Map<string, number[][]>(
            Object.entries(data.file)
          );
        } else {
          this.mapSimulation = new Map<string, (string | number)[][]>(
            Object.entries(data.file)
          );
        }
      });
  }

  initializeLog(): void {
    this.socketService.socket
      .fromEvent('crazyflie_log')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        this.log[Mode.Physique].push(
          new DronePosition(
            data.id,
            new Coord(data.x, data.y, data.z),
            new SensorData(
              data.front,
              data.back,
              data.left,
              data.right,
              data.up
            )
          )
        );
      });

    this.socketService.socket
      .fromEvent('log_simulations')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        this.log[Mode.Simulation].push(
          new DronePosition(
            data.id,
            new Coord(data.position_X, data.position_Y, data.position_Z),
            new SensorData(
              data.senseur_avant,
              data.senseur_arrière,
              data.senseur_gauche,
              data.senseur_droit
            ),
            data.state
          )
        );
      });
  }

  clearLogArray(){
    this.log = [[], []];
  }
}
