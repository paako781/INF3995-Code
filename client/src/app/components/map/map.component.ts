import {
  AfterViewInit,
  Component,
  ElementRef,
  OnInit,
  ViewChild,
} from '@angular/core';
import { LogService } from '@services/log.service';
import { MapService } from '@services/map.service';
import { SocketService } from '@services/socket.service';
import { ResizedEvent } from 'angular-resize-event';
import { map } from 'rxjs/operators';
import { SensorData } from 'src/app/classes/dronePosition/dronePosition';
import { Vec2 } from 'src/app/classes/vec2/vec2';
import { DroneMap, Mode } from 'src/app/classes/enum';
import { MatPaginatorIntl, PageEvent } from '@angular/material/paginator';
import { ModeService } from '@services/mode.service';

@Component({
  selector: 'app-map',
  templateUrl: './map.component.html',
  styleUrls: ['./map.component.scss'],
})
export class MapComponent implements OnInit, AfterViewInit {
  @ViewChild('drone1', { static: false }) canvasDrone1!: ElementRef<HTMLCanvasElement>;

  private nbDroneSim: number = 10;
  private initializedDronePosition1: boolean = false;
  private initializedDronePosition2: boolean = false;
  // Pour initialiser le premier canvas de simulation
  private simCanvasInitialized: boolean = false;
  simulationDroneIdValue: string = 'Drone 0';
  physicalDroneIdValue: string = 'drone 1';
  simuDroneChoice: string[] = [];
  // le premier c'est pour simulation, le deuxième pour physique
  canvasValues: string[][] = [[], []];
  simulationDrones: Map<number, DroneMap> = new Map();
  physicalDrones: Map<number, DroneMap> = new Map();

  constructor(
    private readonly mapService: MapService,
    public _MatPaginatorIntl: MatPaginatorIntl,
    public modeService: ModeService,
    private readonly logService: LogService,
    private readonly socketService: SocketService
  ) {}

  ngOnInit(): void {
    this.socketService.socket
      .fromEvent('nb_drones_sim')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        this.nbDroneSim = +data;
      });
    
    for (let i = 0; i < this.nbDroneSim; i++) {
      this.simuDroneChoice[i] = 'Drone ' + i.toString();
    }

    this._MatPaginatorIntl.itemsPerPageLabel = '';
    const frenchRangeLabel = (
      page: number,
      pageSize: number,
      length: number
    ) => {
      if (length === 0 || pageSize === 0) return `0 de ${length}`;
      length = Math.max(length, 0);
      const startIndex = page * pageSize;
      // If the start index exceeds the list length, do not try and fix the end index to the end.
      const endIndex =
        startIndex < length
          ? Math.min(startIndex + pageSize, length)
          : startIndex + pageSize;
      return `${endIndex} de ${length}`;
    };

    this._MatPaginatorIntl.getRangeLabel = frenchRangeLabel;
  }

  ngAfterViewInit(): void {
    this.physicalDrones.set(1, { drones: [], obstacles: [], angle: Math.PI / 2  });
    this.physicalDrones.set(2, { drones: [], obstacles: [], angle: Math.PI / 2  });
    
    this.mapService.mapContext = this.canvasDrone1.nativeElement.getContext('2d') as CanvasRenderingContext2D;
    this.mapService.canvasWidth = this.mapService.mapContext.canvas.width;
    this.mapService.canvasHeight = this.mapService.mapContext.canvas.height;
    this.mapService.scaledWidth = 0.75 * this.mapService.canvasWidth;
    this.mapService.scaledHeight = 0.75 * this.mapService.canvasHeight;
    this.mapService.drawGrid();

    this.socketService.socket
      .fromEvent('log_simulations')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        let dronePosition: Vec2 = new Vec2(parseFloat(data.position_X), parseFloat(data.position_Y));
        let obstacles: SensorData = new SensorData(
                                      data.senseur_avant,
                                      data.senseur_arrière,
                                      data.senseur_gauche,
                                      data.senseur_droit
                                    );
        let dataToUpdate = this.simulationDrones.get(+data.id);

        if (!dataToUpdate) {
          dataToUpdate = { drones: [], obstacles: [] };
        }

        dataToUpdate!.id = data.id;
        dataToUpdate.drones.push(dronePosition);
        dataToUpdate.obstacles.push(obstacles);
        this.simulationDrones.set(+data.id, dataToUpdate!);
        this.mapService.drones.push(dataToUpdate);

        let pushed = false;
        for (let drone of this.mapService.drones) {
          if (drone.id == dataToUpdate.id) {
            drone.drones.push(dronePosition);
            drone.obstacles.push(obstacles);
            pushed = true;
            break;
          }
        }
        if (pushed) {
          this.mapService.drones.push(dataToUpdate);
        }

        if (this.modeService.curState.mode && +this.simulationDroneIdValue[this.simulationDroneIdValue.length - 1] == +data.id) {
          this.mapService.drawDrone(dronePosition);
          this.mapService.drawObstacles(dronePosition, obstacles);
        }
      });

    this.socketService.socket
      .fromEvent('crazyflie_log')
      .pipe(map((data: any) => data))
      .subscribe((data) => {
        if (!this.initializedDronePosition1) {
          // Trouver la première donnée du drone 1
          for (let i = 0; i < this.logService.log[Mode.Physique].length; i++) {
            if (this.logService.log[Mode.Physique][i].id == 1) {
              let origin = new Vec2(  this.logService.log[Mode.Physique][i].position.positionX,
                                      this.logService.log[Mode.Physique][i].position.positionY
                                    );
              let dataToUpdate = this.physicalDrones.get(1);

              dataToUpdate!.translator = this.mapService.initializeOrigin(origin);
              this.physicalDrones.set(1, dataToUpdate!);
              this.initializedDronePosition1 = true;
              break;
            }
          }
        }

        if (!this.initializedDronePosition2) {
          // Trouver la première donnée du drone 2
          for (let i = 0; i < this.logService.log[Mode.Physique].length; i++) {
            if (this.logService.log[Mode.Physique][i].id == 2) {
              let origin = new Vec2(  this.logService.log[Mode.Physique][i].position.positionX,
                                      this.logService.log[Mode.Physique][i].position.positionY
                                    );
              let dataToUpdate = this.physicalDrones.get(2);
  
              dataToUpdate!.translator = this.mapService.initializeOrigin(origin);
              this.physicalDrones.set(2, dataToUpdate!);
              this.initializedDronePosition2 = true;
              break;
            }
          }
        }

        // Stocker les infos
        let dronePosition = new Vec2(data.x, data.y);
        let obstacles = new SensorData(data.front, data.back, data.left, data.right);
        let angle = this.convertToAngle(data.vecx, data.vacy);
        let dataToUpdate = this.physicalDrones.get(data.id);
        dataToUpdate!.id = data.id;
        dataToUpdate!.drones.push(dronePosition);
        dataToUpdate!.obstacles.push(obstacles);
        dataToUpdate!.angle = angle;
        this.physicalDrones.set(data.id, dataToUpdate!);

        let pushed = false;
        for (let drone of this.mapService.drones) {
          if (drone.id == dataToUpdate?.id) {
            drone.drones.push(dronePosition);
            drone.obstacles.push(obstacles);
            pushed = true;
            break;
          }
        }
        if (pushed) {
          this.mapService.drones.push(dataToUpdate!);
        }

        // Dessiner si c'est le canvas affiché
        if (this.modeService.curState.mode == Mode.Physique && +this.physicalDroneIdValue[this.physicalDroneIdValue.length - 1] == data.id) {
          this.mapService.physicalTranslator = this.physicalDrones.get(data.id)?.translator!;
          this.mapService.drawPhysical(dronePosition);
          this.mapService.drawPhysicalObstacles(dronePosition, obstacles, angle);
        }
      });
  }

  onPageChange(event: PageEvent): void {}

  attachWrapperWidth(wrapper: HTMLElement): Number {
    console.log(wrapper.offsetWidth);
    return wrapper.offsetWidth;
  }

  attachWrapperHeight(wrapper: HTMLElement): Number {
    console.log(wrapper.offsetHeight);
    return wrapper.offsetHeight;
  }

  onResized(event: ResizedEvent): void {
    this.mapService.mapContext.canvas.width = Math.round(event.newRect.width);
    this.mapService.mapContext.canvas.height = Math.round(event.newRect.height);

    // console.log('width : ', this.mapService.mapContext.canvas.width);


    this.mapService.canvasWidth = Math.round(event.newRect.width);
    this.mapService.canvasHeight = Math.round(event.newRect.height);
    this.mapService.scaledWidth = 0.75 * this.mapService.canvasWidth;
    this.mapService.scaledHeight = 0.75 * this.mapService.canvasHeight;

    // Effacer les anciens points
    this.mapService.mapContext.clearRect(0, 0, this.mapService.canvasWidth, this.mapService.canvasHeight);

    this.mapService.drawGrid();
    if (this.modeService.curState.mode) { // simu
      const key = +this.simulationDroneIdValue[this.simulationDroneIdValue.length - 1];
      this.mapService.drawElements(key, this.simulationDrones);
    } else { // physi
      const key = +this.physicalDroneIdValue[this.physicalDroneIdValue.length - 1];
      this.mapService.drawPhysicalElements(key, this.physicalDrones);
    }
  }

  changePhysicalDrone() {
    const key = +this.physicalDroneIdValue[this.physicalDroneIdValue.length - 1];
    this.mapService.physicalTranslator = this.physicalDrones.get(key)?.translator!;    
    this.mapService.mapContext.clearRect(0, 0, this.mapService.canvasWidth, this.mapService.canvasHeight);
    this.mapService.drawGrid();
    this.mapService.drawElements(key, this.physicalDrones);
    console.log('drones physique : ', this.physicalDrones);
  }

  changeSimulationDrone() {
    const key = +this.simulationDroneIdValue[this.simulationDroneIdValue.length - 1];
    this.mapService.mapContext.clearRect(0, 0, this.mapService.canvasWidth, this.mapService.canvasHeight);
    this.mapService.drawGrid();
    this.mapService.drawElements(key, this.simulationDrones);
  }

  private convertToAngle(x: number, y: number):number { 
    if (!x || !y){
      return Math.PI / 2;
    }
    return Math.atan(y / x);
  }
}
