import { Component, OnInit } from '@angular/core';
import { ModeService } from '@services/mode.service';

@Component({
  selector: 'app-console-control',
  templateUrl: './console-control.component.html',
  styleUrls: ['./console-control.component.scss'],
})
export class ConsoleControlComponent implements OnInit {
  constructor(public modeService: ModeService) {}

  ngOnInit(): void {}

  toggleConsoleVisibility(): void {
    this.modeService.toggleConsoleVisibility();
  }
}
