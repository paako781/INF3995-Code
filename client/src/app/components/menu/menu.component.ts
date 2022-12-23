import { Component, OnInit } from '@angular/core';
import { Mode } from 'src/app/classes/enum';
//import { MODE_TO_TEXT } from 'src/app/classes/constants';
import { ModeService } from '@services/mode.service';
import { Router } from '@angular/router';
import { DataService } from '@services/data-service.service';

@Component({
  selector: 'app-menu',
  templateUrl: './menu.component.html',
  styleUrls: ['./menu.component.scss'],
})
export class MenuComponent implements OnInit {
  constructor(
    public modeService: ModeService,
    private router: Router,
    public data: DataService
  ) {}

  ngOnInit(): void {}

  onSelectedMenu(value: string): void {
    this.modeService.mode = value;
    if (value == 'Physique' || value == 'Simulation') {
      this.modeService.changeMode(
        value == 'Physique' ? Mode.Physique : Mode.Simulation
      );
      this.router.navigate(['']);
      return;
    }

    // On route vers les pages de BD
    else if (value == 'Database') {
      this.data.update();
      this.router.navigate(['dataMissions']);
    }
  }
}
