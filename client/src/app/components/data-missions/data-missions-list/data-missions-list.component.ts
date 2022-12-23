import { Component, ElementRef, ViewChild } from '@angular/core';
import { DataService } from '@services/data-service.service';
import { Sort } from '@angular/material/sort';
import { MapService } from '@services/map.service';
import { ResizedEvent } from 'angular-resize-event';
import { DroneMap } from 'src/app/classes/enum';
import { HttpRequestsService } from '@services/http-requests.service';

@Component({
  selector: 'app-data-missions-list',
  templateUrl: './data-missions-list.component.html',
  styleUrls: ['./data-missions-list.component.scss'],
})
export class DataMissionsListComponent {
  @ViewChild('map', { static: false }) canvas!: ElementRef<HTMLCanvasElement>;

  constructor(
    public dataService: DataService,
    public mapService: MapService,
    private httpRequest: HttpRequestsService
  ) {}

  public sortData(sort: Sort): void {
    const data = this.dataService.dataMissions.slice();
    if (!sort.active || sort.direction === '') {
      this.dataService.dataMissions = data;
      return;
    }

    this.dataService.dataMissions = data.sort((a, b) => {
      const isAsc = sort.direction === 'asc';
      switch (sort.active) {
        case 'tempsVol':
          return compare(a.tempsVol, b.tempsVol, isAsc);
        case 'tempsMission':
          return compare(a.tempsMission, b.tempsMission, isAsc);
        case 'nbsDronesPhys':
          return compare(a.nbsDronesPhys, b.nbsDronesPhys, isAsc);
        case 'nbsDronesSimul':
          return compare(a.nbsDronesSimul, b.nbsDronesSimul, isAsc);
        case 'distance':
          return compare(a.distance, b.distance, isAsc);
        default:
          return 0;
      }
    });
  }

  public async getMissionData(id: string): Promise<void> {
    this.httpRequest.getMissions(id).subscribe((data) => {
      this.dataService.mission = data;
    });

    await this.httpRequest.getMissionsList().toPromise();
  }

  public async deleteMission(id: string): Promise<void> {
    this.httpRequest.deleteMission(id).subscribe((data) => {});
    await this.httpRequest.getMissionsList().toPromise();
    window.location.reload();
  }

  public updateMap(id: number): void {
    for (let i = 0; i < this.dataService.mission.mapData.length; i++) {
      if (id === this.dataService.mission.mapData[i].id) {
        this.dataService.mapData = this.dataService.mission.mapData[i];

        let map: Map<number, DroneMap> = new Map();
        map.set(i, this.dataService.mapData);
        if (this.dataService.mission.nbsDronesPhys == 0) {
          // on est en simulation
          this.mapService.drawElements(i, map);
        } else {
          // on est en physique
          this.mapService.drawPhysicalElements(i, map);
        }
        break;
      }
    }
  }

  onResized(event: ResizedEvent): void {
    this.mapService.mapContext.canvas.width = Math.round(event.newRect.width);
    this.mapService.mapContext.canvas.height = Math.round(event.newRect.height);
    this.mapService.canvasWidth = Math.round(event.newRect.width);
    this.mapService.canvasHeight = Math.round(event.newRect.height);
    this.mapService.scaledWidth = 0.75 * this.mapService.canvasWidth;
    this.mapService.scaledHeight = 0.75 * this.mapService.canvasHeight;

    // Dessiner grid et  points
    this.mapService.drawGrid();
  }
}

function compare(a: number | string, b: number | string, isAsc: boolean) {
  return (a < b ? -1 : 1) * (isAsc ? 1 : -1);
}
