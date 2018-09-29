import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ContouringTopBarComponent } from './contouring-top-bar.component';

describe('ContouringTopBarComponent', () => {
  let component: ContouringTopBarComponent;
  let fixture: ComponentFixture<ContouringTopBarComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ContouringTopBarComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ContouringTopBarComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
