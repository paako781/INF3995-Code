import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DataMissionsListComponent } from './data-missions-list.component';

describe('DataMissionsListComponent', () => {
  let component: DataMissionsListComponent;
  let fixture: ComponentFixture<DataMissionsListComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ DataMissionsListComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DataMissionsListComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
