import { Injectable } from '@angular/core';
import {
  NB_COL,
  NB_ROW,
  WIDTH_SIMULATION,
  HEIGTH_SIMULATION,
  DRONE_RADIUS,
  OBSTACLE_RADIUS,
} from '../classes/constants';
import { SensorData } from '../classes/dronePosition/dronePosition';
import { DroneMap } from '../classes/enum';
import { Vec2 } from '../classes/vec2/vec2';

@Injectable({
  providedIn: 'root',
})
export class MapService {
  physicalTranslator: Vec2 = new Vec2();
  canvasWidth: number = 0;
  canvasHeight: number = 0;
  scaledWidth: number = 0;
  scaledHeight: number = 0;
  mapContext!: CanvasRenderingContext2D;
  // copie des infos des drones pour dessiner les cartes des missions antérieurs
  drones: DroneMap[] = [];

  constructor() {}

  drawGrid() {
    this.mapContext.beginPath();
    this.mapContext.strokeStyle = 'black';
    this.mapContext.lineWidth = 0.6;
    let canvaWidth: number = this.mapContext.canvas.width;
    let canvaHeight: number = this.mapContext.canvas.height;

    this.mapContext.fillStyle = 'black';

    // Tracer les lignes
    for (let i = 1; i <= NB_COL; i++) {
      this.mapContext.moveTo(0, (canvaHeight * i) / NB_COL);
      this.mapContext.lineTo(canvaWidth, (canvaHeight * i) / NB_COL);
    }

    // Tracer les colonnes
    for (let i = 1; i <= NB_ROW; i++) {
      this.mapContext.moveTo((canvaWidth * i) / NB_ROW, 0);
      this.mapContext.lineTo((canvaWidth * i) / NB_ROW, canvaHeight);
    }

    this.mapContext.stroke();
  }

  drawElements(key: number, infos: Map<number, DroneMap>) {
    let drones = infos.get(key)?.drones;
    let obstacles = infos.get(key)?.obstacles;
    if (key == 1) {
      console.log('obstacles : ', obstacles);
    }
    this.drawDrones(drones!);
    this.drawAllObstacles(drones!, obstacles!);
  }

  drawDrone(dronePosition: Vec2): void {
    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'red';
    let position = this.simuToMap(dronePosition);

    this.mapContext.arc(position.x, position.y, DRONE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawDrones(dronePositions: Vec2[]) {
    for (const drone of dronePositions) {
      this.drawDrone(drone);
    }
  }

  drawObstacles(dronePosition: Vec2, obstacles: SensorData) {
    this.drawFrontObstacle(dronePosition, obstacles.front);
    this.drawBackObstacle(dronePosition, obstacles.back);
    this.drawRightObstacle(dronePosition, obstacles.right);
    this.drawLeftObstacle(dronePosition, obstacles.left);
  }

  drawAllObstacles(dronePositions: Vec2[], obstacles: SensorData[]) {
    for (let i = 0; i < dronePositions.length; i++) {
      console.log('drone position : ', dronePositions[i]);
      console.log('obstacles : ', obstacles[i]);
      this.drawObstacles(dronePositions[i], obstacles[i]);
    }
  }

  drawFrontObstacle(dronePosition: Vec2, distance: number) {
    if (distance < 0) {
      return;
    }

    let pos = this.rotate90(dronePosition);

    pos.x *= this.scaledWidth / 5;
    pos.y *= this.scaledHeight / 5;

    let dist = (distance * 5) / HEIGTH_SIMULATION;
    dist *= this.scaledHeight / 5;
    pos.y += dist;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'black';
    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawBackObstacle(dronePosition: Vec2, distance: number) {
    if (distance < 0) {
      return;
    }

    let pos = this.rotate90(dronePosition);

    pos.x *= this.scaledWidth / 5;
    pos.y *= this.scaledHeight / 5;

    let dist = (distance * 5) / HEIGTH_SIMULATION;
    dist *= this.scaledHeight / 5;
    pos.y -= dist;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'black';
    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawRightObstacle(dronePosition: Vec2, distance: number) {
    if (distance < 0) {
      return;
    }

    let pos = this.rotate90(dronePosition);

    pos.x *= this.scaledWidth / 5;
    pos.y *= this.scaledHeight / 5;

    let dist = (distance * 5) / WIDTH_SIMULATION;
    dist *= this.scaledWidth / 5;
    pos.x += dist;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'black';
    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawLeftObstacle(dronePosition: Vec2, distance: number) {
    if (distance < 0) {
      return;
    }

    let pos = this.rotate90(dronePosition);

    pos.x *= this.scaledWidth / 5;
    pos.y *= this.scaledHeight / 5;

    let dist = (distance * 5) / WIDTH_SIMULATION;
    dist *= this.scaledWidth / 5;
    pos.x -= dist;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'black';
    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  private rotate90(dronePosition: Vec2): Vec2 {
    return new Vec2(dronePosition.y, -dronePosition.x);
  }

  private simuToMap(dronePosition: Vec2): Vec2 {
    let position = this.rotate90(dronePosition);

    position.x *= this.scaledWidth / 5;
    position.y *= this.scaledHeight / 5;

    position.x += this.canvasWidth / 2;
    position.y = this.canvasHeight - (position.y + this.canvasHeight / 2);

    return position;
  }

  // À utiliser pour faire la translation
  private translate(position: Vec2): Vec2 {
    let pos = position;
    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);
    return pos;
  }

  // Sert à conserver le vecteur qui permet de ramener le drone à (0, 0) au début
  // et donc à dessiner toutes les positions suivantes en fonction de ça
  initializeOrigin(dronePosition: Vec2): Vec2 {
    let translator = new Vec2();
    if (dronePosition.x == 0 && dronePosition.y == 0) {
      return translator;
    }

    translator.x = -dronePosition.x;
    translator.y = -dronePosition.y;
    return translator;
  }

  // translation à faire au cas ou la position d'origine du drone n'est pas (0, 0)
  initialTranslation(dronePosition: Vec2): Vec2 {
    let pos = dronePosition;
    pos.x += this.physicalTranslator.x;
    pos.y += this.physicalTranslator.y;
    return pos;
  }

  drawPhysicalElements(key: number, infos: Map<number, DroneMap>) {
    let drones = infos.get(key)?.drones;
    let obstacles = infos.get(key)?.obstacles;
    let angle = infos.get(key)?.angle;
    this.drawPhysicalDrones(drones!);
    this.drawAllPhysicalObstacles(drones!, obstacles!, angle!);
  }

  drawPhysical(dronePosition: Vec2) {
    // en mètres
    let pos = this.initialTranslation(dronePosition);

    // mise à l'échelle de la map
    pos.x *= this.canvasWidth / 10;
    pos.y *= this.canvasHeight / 10;

    // translation
    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'red';

    this.mapContext.arc(pos.x, pos.y, DRONE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawPhysicalDrones(dronePositions: Vec2[]) {
    for (const drone of dronePositions) {
      this.drawPhysical(drone);
    }
  }

  drawAllPhysicalObstacles(
    dronePositions: Vec2[],
    obstacles: SensorData[],
    angle: number
  ) {
    for (let i = 0; i < dronePositions.length; i++) {
      this.drawPhysicalObstacles(dronePositions[i], obstacles[i], angle);
    }
  }

  drawPhysicalObstacles(
    dronePosition: Vec2,
    obstacles: SensorData,
    angle: number
  ) {
    this.drawFront(dronePosition, obstacles.front, angle);
    this.drawBack(dronePosition, obstacles.back, angle);
    this.drawRight(dronePosition, obstacles.right, angle);
    this.drawLeft(dronePosition, obstacles.left, angle);
  }
  drawFront(dronePosition: Vec2, distance: number, angle: number) {
    let pos = this.initialTranslation(dronePosition);

    pos.x += distance * Math.cos(angle);
    pos.y += distance * Math.sin(angle);

    pos.x *= this.canvasWidth / 10;
    pos.y *= this.canvasHeight / 10;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'purple';

    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawBack(dronePosition: Vec2, distance: number, angle: number) {
    let pos = this.initialTranslation(dronePosition);

    pos.x += distance * Math.cos(angle + Math.PI);
    pos.y += distance * Math.sin(angle + Math.PI);

    pos.x *= this.canvasWidth / 10;
    pos.y *= this.canvasHeight / 10;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'purple';

    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawRight(dronePosition: Vec2, distance: number, angle: number) {
    let pos = this.initialTranslation(dronePosition);

    pos.x += distance * Math.cos(angle - Math.PI / 2);
    pos.y += distance * Math.sin(angle - Math.PI / 2);

    pos.x *= this.canvasWidth / 10;
    pos.y *= this.canvasHeight / 10;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'purple';

    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }

  drawLeft(dronePosition: Vec2, distance: number, angle: number) {
    let pos = this.initialTranslation(dronePosition);

    pos.x += distance * Math.cos(angle + Math.PI / 2);
    pos.y += distance * Math.sin(angle + Math.PI / 2);

    pos.x *= this.canvasWidth / 10;
    pos.y *= this.canvasHeight / 10;

    pos.x += this.canvasWidth / 2;
    pos.y = this.canvasHeight - (pos.y + this.canvasHeight / 2);

    this.mapContext.beginPath();
    this.mapContext.fillStyle = 'purple';

    this.mapContext.arc(pos.x, pos.y, OBSTACLE_RADIUS, 0, Math.PI * 2);
    this.mapContext.fill();
    this.mapContext.stroke();
  }
}
