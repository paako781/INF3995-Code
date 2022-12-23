import { Component, OnInit } from '@angular/core';
import { LogService } from '@services/log.service';
import { ModeService } from '@services/mode.service';

@Component({
  selector: 'app-console',
  templateUrl: './console.component.html',
  styleUrls: ['./console.component.scss'],
})
export class ConsoleComponent implements OnInit {
  constructor(public logService: LogService, public modeService: ModeService) {}

  ngOnInit(): void {
    this.logService.initializeLog();
  }
}
