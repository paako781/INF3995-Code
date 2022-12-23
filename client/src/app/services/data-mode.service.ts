import { Injectable } from '@angular/core';

interface modeMission {
  value:string;
  viewValue:string;
}

@Injectable({
  providedIn: 'root'
})
export class DataModeService {

  selectedMode: string ='Physique';

  modeSelect: modeMission[] = [
    {value: 'Physique', viewValue: 'Physique'}, 
    {value: 'Simulation', viewValue:'Simulation'}
  ];

  constructor() { }
}
