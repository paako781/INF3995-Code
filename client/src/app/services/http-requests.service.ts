import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';



@Injectable({
  providedIn: 'root'
})
export class HttpRequestsService {

  private baseUrl = 'http://127.0.0.1:5000/dataMissions';


  constructor(private http: HttpClient) { }

  public getMissions(_id: string): Observable<any> {
    return this.http.get(`${this.baseUrl}/${_id}`);
  }

  public deleteMission(_id: string): Observable<any> {
    return this.http.delete(`${this.baseUrl}/${_id}`)
  }

  public createMission(mission: Object): Observable<Object> {
    return this.http.post(`${this.baseUrl}`, mission);
  }

  public getMissionsList(): Observable<any> {
    return this.http.get(`${this.baseUrl}`);
  }

  public updateMission(mission:Object, _id: string): Observable<Object> {
    return this.http.put(`${this.baseUrl}/${_id}`, mission);
  }
}
