import { Injectable } from '@angular/core';
import { MODE_TO_TEXT } from '../classes/constants';
import { Mode, ViewState } from '../classes/enum';
import { LogService } from './log.service';
import { StateService } from './state.service';

@Injectable({
  providedIn: 'root',
})
export class ModeService {
  mode: string;
  curState: ViewState;
  stateList: Array<ViewState>;
  constructor(
    public stateService: StateService,
    public LogService: LogService
  ) {
    let physiqueState: ViewState = {
      mode: Mode.Physique,
      consoleVisible: true,
    };
    let simState: ViewState = { mode: Mode.Simulation, consoleVisible: true };
    this.stateList = new Array<ViewState>();
    this.stateList.push(physiqueState);
    this.stateList.push(simState);
    this.curState = physiqueState;
    this.stateService.curState =
      this.stateService.droneState[this.curState.mode];
    this.LogService.curLog = this.LogService.log[this.curState.mode];
    this.mode = MODE_TO_TEXT.get(Mode.Physique) as string;
  }

  toggleConsoleVisibility(): void {
    this.curState.consoleVisible = !this.curState.consoleVisible;
  }

  changeMode(mode: Mode): void {
    if (mode != this.curState.mode) {
      this.curState = this.stateList.reverse()[0];
      this.stateService.curState = this.stateService.droneState[mode];
      this.LogService.curLog = this.LogService.log[mode];
    }
  }
}
