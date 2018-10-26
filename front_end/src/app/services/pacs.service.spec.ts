import { TestBed } from '@angular/core/testing';

import { PacsService } from './pacs.service';

describe('PacsService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: PacsService = TestBed.get(PacsService);
    expect(service).toBeTruthy();
  });
});
