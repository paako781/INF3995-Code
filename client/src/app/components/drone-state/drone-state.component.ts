import { Component, OnInit,ViewChild } from '@angular/core';
import { ModeService } from '@services/mode.service';
import { SocketService } from '@services/socket.service';
import { map } from 'rxjs';
import { Mode } from 'src/app/classes/enum';
import { DroneInfo } from 'src/app/classes/enum';
import { ID_1, ID_2 } from 'src/app/classes/constants';
import { StateService } from '@services/state.service';
import { MatTable } from '@angular/material/table';

const ELEMENT_DATA: DroneInfo[] = [
  { id: 1, state: 'En attente', battery: 60 },
  { id: 2, state: 'Accident', battery: 80 },
];

@Component({
  selector: 'app-drone-state',
  templateUrl: './drone-state.component.html',
  styleUrls: ['./drone-state.component.scss'],
})
export class DroneStateComponent implements OnInit {
  displayedColumns: string[] = ['id', 'state', 'battery'];
  dataSource = ELEMENT_DATA;
  @ViewChild(MatTable) table:MatTable<any> | undefined;
  constructor(
    public stateService: StateService,
    private socketService: SocketService,
  ) {}

  ngOnInit(): void {
    this.socketService.socket
      .fromEvent('state_simulation')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        this.stateService.droneState[Mode.Simulation][data.id].id = data.id;
        this.stateService.droneState[Mode.Simulation][data.id].state =
          data.state;
        this.stateService.droneState[Mode.Simulation][data.id].battery =
        data.battery_level;
      });
    this.socketService.socket
      .fromEvent('state_physique')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        if (data.id == ID_1) {
          this.stateService.droneState[Mode.Physique][ID_1 - 1].id = data.id;
          this.stateService.droneState[Mode.Physique][ID_1 - 1].state =
            data.state;
        } else {
          this.stateService.droneState[Mode.Physique][ID_2 - 1].id = data.id;
          this.stateService.droneState[Mode.Physique][ID_2 - 1].state =
            data.state;
        }
      });
    this.socketService.socket
      .fromEvent('battery_level_physique')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        if (data.id == ID_1) {
          this.stateService.droneState[Mode.Physique][ID_1 - 1].battery =
            data.battery_level;
        } else {
          this.stateService.droneState[Mode.Physique][ID_2 - 1].battery =
            data.battery_level;
        }
      });

      this.socketService.socket
      .fromEvent('nb_drones_sim')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        
        this.stateService.drone_nb_sim = data.number;
        for(let i = 0; i< this.stateService.drone_nb_sim; i++){
          this.stateService.droneState[Mode.Simulation].push({ id: i, state: '', battery: 0 });
        }
        this.table?.renderRows();

      });
  }
}
