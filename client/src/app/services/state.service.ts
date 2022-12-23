import { Injectable, OnInit } from '@angular/core';
import { map } from 'rxjs';
import { ID_1, ID_2 } from '../classes/constants';
import { DroneInfo, Mode } from '../classes/enum';
import { SocketService } from './socket.service';

@Injectable({
  providedIn: 'root',
})
export class StateService implements OnInit {
  droneState: DroneInfo[][];
  curState: DroneInfo[];
  drone_nb_sim : number;
  constructor(public socketService: SocketService) {
    this.droneState = [[], []];
    this.droneState[Mode.Physique].push({ id: 0, state: '', battery: 0 });
    this.droneState[Mode.Physique].push({ id: 0, state: '', battery: 0 });
    // this.droneState[Mode.Simulation].push({ id: 0, state: '', battery: 0 });
    // this.droneState[Mode.Simulation].push({ id: 0, state: '', battery: 0 });
    this.curState = [];
    this.drone_nb_sim =0 ;
  }

  ngOnInit(): void {
    // this.socketService.socket
    //   .fromEvent('state_simulation')
    //   .pipe(map((data: any) => data))
    //   .subscribe((data) => {
    //     console.log(data);
    //     if (data.id == ID_1) {
    //       this.droneState[Mode.Simulation][ID_1 - 1].id = data.id;
    //       this.droneState[Mode.Simulation][ID_1 - 1].state = data.state;
    //     } else {
    //       this.droneState[Mode.Simulation][ID_2 - 1].id = data.id;
    //       this.droneState[Mode.Simulation][ID_2 - 1].state = data.state;
    //     }
    //   });
    // this.socketService.socket
    //   .fromEvent('state_physique')
    //   .pipe(map((data: any) => data))
    //   .subscribe((data) => {
    //     if (data.id == ID_1) {
    //       this.droneState[Mode.Physique][ID_1 - 1].id = data.id;
    //       this.droneState[Mode.Physique][ID_1 - 1].state = data.state;
    //     } else {
    //       this.droneState[Mode.Physique][ID_2 - 1].id = data.id;
    //       this.droneState[Mode.Physique][ID_2 - 1].state = data.state;
    //     }
    //   });
  }
}
