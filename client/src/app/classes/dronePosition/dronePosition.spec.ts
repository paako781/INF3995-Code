import {Coord, DronePosition, SensorData} from './dronePosition'

describe('DronePosition', () => {
  it('should create an instance', () => {
    expect(new DronePosition('',0,new Coord(),new SensorData())).toBeTruthy();
  });
});

