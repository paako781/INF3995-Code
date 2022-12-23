import { DroneMap } from "../enum";

export class DataMission {
    id: string;
    tempsMission: string;
    tempsVol: number;
    nbsDronesPhys: number;
    nbsDronesSimul: number;
    distance: number;
    mapData: DroneMap[];


    constructor(id : string, tempsMission: string, tempsVol: number, nbsDronesPhys: number, nbsDronesSimul: number, distance: number, mapData: DroneMap[] ){
        this.id = id;
        this.tempsMission = tempsMission;
        this.tempsVol = tempsVol;
        this.nbsDronesPhys = nbsDronesPhys;
        this.nbsDronesSimul = nbsDronesSimul;
        this.distance = distance;
        this.mapData =  mapData;
    }

}
