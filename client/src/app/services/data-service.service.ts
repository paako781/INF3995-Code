import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DataMission } from '../classes/dataMission/data-mission';
import { DronePosition } from '../classes/dronePosition/dronePosition';
import { ModeService } from './mode.service';
import { LogService } from './log.service';
import { HttpRequestsService } from './http-requests.service';
import { DroneMap } from '../classes/enum';

const SECOND_TO_MS = 1000;

@Injectable({
  providedIn: 'root'
})
export class DataService{

  drone: number = 0;

  dataMissions: DataMission[] =[];
  dronesPhysique: DronePosition[] = [];
  dronesSimulation: DronePosition[] = [];
  distanceTotale: number = 0;
  mission: DataMission = new DataMission('','',0,0,0,0,[]);
  mapData: DroneMap = {drones:[], obstacles:[]};


  //private baseUrl = 'http://127.0.0.1:5000/dataMissions';

  tempsDebut: number = 0;
  tempsFin: number = 0;

  constructor(private http: HttpClient, private mode: ModeService, private log: LogService   , private httpRequest: HttpRequestsService) {
    this.update();

  }


  public update(): void {
    this.httpRequest.getMissionsList().subscribe(data => {
      this.dataMissions = data;
    }
    );
  }


  public debutTempsVol(): void {
    this.tempsDebut = new Date().getTime();
  }
  
  public calculTempsVol():number {
    this.tempsFin = (new Date().getTime() - this.tempsDebut)/SECOND_TO_MS;
    return this.tempsFin;
  }
  

  public calculateDistance(): void {
    this.distanceTotale = 0;
    if (this.mode.curState.mode === 0) {
      for (let i = 0; i < this.log.log[0].length -1; i++) {
        this.distanceTotale += Math.pow(
          Math.pow(
            this.log.log[0][i].position.positionX -
            this.log.log[0][i+1].position.positionX,
            2
          ) +
            Math.pow(
              this.log.log[0][i].position.positionY -
              this.log.log[0][i+1].position.positionY,
              2
            ),
          0.5
        );
      }
    } else {
      for (let i = 0; i < this.log.log[1].length -1; i++) {
        this.distanceTotale += Math.pow(
          Math.pow(
            this.log.log[1][i].position.positionX -
            this.log.log[1][i+1].position.positionX,
            2
          ) +
            Math.pow(
              this.log.log[1][i].position.positionY -
              this.log.log[1][i+1].position.positionY,
              2
            ),
          0.5
        );
      }
    }

  }


}

