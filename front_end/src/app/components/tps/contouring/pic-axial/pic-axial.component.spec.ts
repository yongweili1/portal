import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PicAxialComponent } from './pic-axial.component';

describe('PicAxialComponent', () => {
  let component: PicAxialComponent;
  let fixture: ComponentFixture<PicAxialComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PicAxialComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PicAxialComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
