import { Injectable, OnInit } from '@angular/core';
import { Socket } from 'ngx-socket-io';

@Injectable({
  providedIn: 'root',
})
export class SocketService implements OnInit {
  constructor(public socket: Socket) {}

  ngOnInit(): void {
    this.socket.connect();
  }

  public sendMessage(eventName: string, message?: String): void {
    this.socket.emit(eventName, message);
  }
}
