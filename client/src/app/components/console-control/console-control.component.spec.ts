import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ConsoleControlComponent } from './console-control.component';

describe('ConsoleControlComponent', () => {
  let component: ConsoleControlComponent;
  let fixture: ComponentFixture<ConsoleControlComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ConsoleControlComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ConsoleControlComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
