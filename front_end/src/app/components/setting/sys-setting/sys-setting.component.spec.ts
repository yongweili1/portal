import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SysSettingComponent } from './sys-setting.component';

describe('SysSettingComponent', () => {
  let component: SysSettingComponent;
  let fixture: ComponentFixture<SysSettingComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SysSettingComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SysSettingComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
