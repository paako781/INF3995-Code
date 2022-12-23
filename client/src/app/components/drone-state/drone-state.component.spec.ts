import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DroneStateComponent } from './drone-state.component';

describe('DroneStateComponent', () => {
  let component: DroneStateComponent;
  let fixture: ComponentFixture<DroneStateComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ DroneStateComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DroneStateComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
