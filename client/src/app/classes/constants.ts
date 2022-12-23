import { Mode } from './enum';

export let MODE_TO_TEXT = new Map<Mode, string>([
  [Mode.Physique, 'Physique'],
  [Mode.Simulation, 'Simulation'],
]);

export const NB_COL = 20;
export const NB_ROW = 20;
export const WIDTH_SIMULATION = 428.789;
export const HEIGTH_SIMULATION = 428.789;

export const ID_1 = 1;
export const ID_2 = 2;

export const DRONE_RADIUS = 3;
export const OBSTACLE_RADIUS = 5;
