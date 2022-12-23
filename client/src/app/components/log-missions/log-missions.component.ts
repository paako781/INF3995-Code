import { Component, Input, OnInit } from '@angular/core';
import { LogService } from '@services/log.service';
import { DataModeService } from '@services/data-mode.service';

@Component({
  selector: 'app-log-missions',
  templateUrl: './log-missions.component.html',
  styleUrls: ['./log-missions.component.scss'],
})
export class LogMissionsComponent implements OnInit {
  constructor(public log: LogService, public mode: DataModeService) {}

  ngOnInit(): void {}
}
