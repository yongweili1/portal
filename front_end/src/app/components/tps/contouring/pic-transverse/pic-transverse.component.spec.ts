import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PicTransverseComponent } from './pic-transverse.component';

describe('PicTransverseComponent', () => {
  let component: PicTransverseComponent;
  let fixture: ComponentFixture<PicTransverseComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PicTransverseComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PicTransverseComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
