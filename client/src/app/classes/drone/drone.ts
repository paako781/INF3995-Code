export enum State {
  Waiting = 'En attente',
  In_mission = 'En mission',
  Crashed = 'Accident', // si on fait RF13
}

export class Drone {
  id: number;
  state: string;
  batteryLevel: number;

  constructor(id: number, state: State, batteryLevel: number) {
    this.id = id;
    this.state = state;
    this.batteryLevel = batteryLevel;
  }
}
