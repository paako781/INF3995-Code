export class SensorData {
  front: number;
  back: number;
  left: number;
  right: number;
  up?: number;
  down?: number;

  constructor(
    front: number = 0,
    back: number = 0,
    left: number = 0,
    right: number = 0,
    up: number = 0,
    down: number = 0
  ) {
    this.front = front;
    this.back = back;
    this.left = left;
    this.right = right;
    this.up = up;
    this.down = down;
  }
}
export class Coord {
  positionX: number;
  positionY: number;
  positionZ: number;

  constructor(x: number = 0, y: number = 0, z: number = 0) {
    this.positionX = x;
    this.positionY = y;
    this.positionZ = z;
  }
}

export class DronePosition {
  etat: string;
  id: number;
  position: Coord;
  sensorData: SensorData;

  constructor(
    id: number,
    position: Coord,
    sensorData: SensorData,
    etat: string = ''
  ) {
    this.etat = etat;
    this.id = id;
    this.position = position;
    this.sensorData = sensorData;
  }
}
