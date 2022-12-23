import { TestBed } from '@angular/core/testing';

import { ModeServiceService } from './mode.service';

describe('ModeServiceService', () => {
  let service: ModeServiceService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ModeServiceService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
