import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import {
  ButtonModule,
  CalendarModule,
  CheckboxModule,
  ConfirmDialogModule,
  DataTableModule,
  DialogModule,
  DropdownModule, InputTextareaModule, InputTextModule, PaginatorModule, PanelModule, RadioButtonModule, SelectButtonModule
} from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';
import { ToastModule } from 'primeng/toast';

import { SettingComponent } from './setting.component';
import { UserSettingComponent } from './user-setting/user-setting.component';
import { SysSettingComponent } from './sys-setting/sys-setting.component';

const primengModule = [
  ButtonModule, InputTextModule, DropdownModule, DataTableModule,
  PaginatorModule, ConfirmDialogModule, PanelModule,
  SelectButtonModule, RadioButtonModule, InputTextareaModule, CheckboxModule,
  CalendarModule, DropdownModule
];


const routes: Routes = [
  { path: '', 
    component: SettingComponent,
    children: [
      { path: 'UserSetting', component: UserSettingComponent },
      { path: 'SysSetting', component: SysSettingComponent },
  ]
   },
];

@NgModule({
  imports: [
    ...primengModule,
    SharedModule,
    RouterModule.forChild(routes),
    ToastModule
  ],
  declarations: [
    SettingComponent,
    UserSettingComponent,
    SysSettingComponent,
  ],
  exports: [
    SettingComponent
  ],
  providers: [
  ]
})
export class SettingModule {
}
