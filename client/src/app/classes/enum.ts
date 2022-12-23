import { SensorData } from "./dronePosition/dronePosition";
import { Vec2 } from "./vec2/vec2";

export enum Mode {
  Physique,
  Simulation,
}

export interface ViewState {
  mode: Mode;
  consoleVisible: boolean;
}

export interface DroneInfo {
  id: number;
  state: string;
  battery: Number;
}

export interface DroneMap {
  id?: number;
  canvas?: CanvasRenderingContext2D;
  drones: Vec2[];
  obstacles: SensorData[];
  translator?: Vec2; // Pour les drones physiques, il faut conserver la position du tout premier drone
  angle?: number; // Pour les drones physiques, il faut conserver l'angle de rotation par rapport au y positif
}
