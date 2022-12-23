import { ComponentFixture, TestBed } from '@angular/core/testing';

import { LogMissionsComponent } from './log-missions.component';

describe('LogMissionsComponent', () => {
  let component: LogMissionsComponent;
  let fixture: ComponentFixture<LogMissionsComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ LogMissionsComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(LogMissionsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
