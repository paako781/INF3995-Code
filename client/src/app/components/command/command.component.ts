import { Component, OnInit } from '@angular/core';
import { LogService } from '@services/log.service';
import { ModeService } from '@services/mode.service';
import { SocketService } from '@services/socket.service';
import { ID_1 } from 'src/app/classes/constants';
import { Mode } from 'src/app/classes/enum';
import { DataService } from '@services/data-service.service';
import { Vec2 } from 'src/app/classes/vec2/vec2';
import { SensorData } from 'src/app/classes/dronePosition/dronePosition';
import { HttpRequestsService } from '@services/http-requests.service';
import { StateService } from '@services/state.service';


@Component({
  selector: 'app-command',
  templateUrl: './command.component.html',
  styleUrls: ['./command.component.scss'],
})
export class CommandComponent implements OnInit {
  drones: Vec2[] = [];
  obstacles: SensorData[] = [];
  constructor(
    public modeService: ModeService,
    public socketService: SocketService,
    public logService: LogService,
    public dataService: DataService,
    private httpRequest: HttpRequestsService,
    private stateService: StateService
  ) {}

  ngOnInit(): void {
    this.socketService.sendMessage('initialize');

    this.logService.receiveLog('my_logPhys');
    this.logService.receiveLog('my_logSim');
  }

  identifier(droneId: number): void {
    // droneId nous indique si c'est identifier 1 ou 2
    if (droneId === ID_1) {
      this.socketService.sendMessage('identify1');
      return;
    }

    this.socketService.sendMessage('identify2');
  }

  async lancerMission(): Promise<void> {
    this.dataService.mission.tempsVol = 0;

    this.dataService.debutTempsVol();

    this.dataService.mission.nbsDronesPhys = 0;
    this.dataService.mission.nbsDronesSimul = 0;

    // Pour savoir si c'est physique ou simulation, testez la variable modeService.curState.mode
    if (this.modeService.curState.mode === Mode.Physique) {
      this.socketService.sendMessage('startMissionPhysique');
      this.dataService.mission.nbsDronesPhys = 2;
      this.dataService.mission.mapData = [
        {
          id: 1,
          drones: (this.drones = [{ x: 1, y: 1 }]),
          obstacles: (this.obstacles = [
            { front: 1, back: 1, left: 1, right: 1 },
          ]),
        },
      ];
      await this.httpRequest
        .createMission(this.dataService.mission)
        .toPromise();

      this.dataService.update();

      return;
    }
    this.socketService.sendMessage('startMissionSimulation');
    this.dataService.mission.nbsDronesSimul = 10;
    await this.httpRequest.createMission(this.dataService.mission).toPromise();

    this.dataService.update();
  }

  terminerMission(): void {
    this.dataService.mission.tempsVol = this.dataService.calculTempsVol();
    this.dataService.mission.nbsDronesSimul = this.stateService.drone_nb_sim;
    this.dataService.calculateDistance();
    this.dataService.mission.distance =
      Math.round((this.dataService.distanceTotale + Number.EPSILON) * 100) /
      100;
    this.logService.clearLogArray();

    this.httpRequest
      .updateMission(
        this.dataService.mission,
        this.dataService.dataMissions[this.dataService.dataMissions.length - 1]
          .id
      )
      .subscribe((data) => {});
    this.dataService.update();

    // Pour savoir si c'est physique ou simulation, testez la variable modeService.curState.mode
    if (this.modeService.curState.mode === Mode.Physique) {
      this.socketService.sendMessage('endMissionPhysique');
      return;
    }
    this.socketService.sendMessage('endMissionSimulation');
  }

  retourBase(): void {
    // Pour savoir si c'est physique ou simulation, testez la variable modeService.curState.mode
    if (this.modeService.curState.mode === Mode.Physique) {
      this.socketService.sendMessage('returntoBasePysique');
      return;
    }
    this.socketService.sendMessage('returntoBaseSimulation');
  }

  makeP2P(): void {
    if (this.modeService.curState.mode === Mode.Physique) {
      this.socketService.sendMessage('P2P');
      return;
    }
  }
}
