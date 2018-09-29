import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {
  ButtonModule,
  InputTextModule,
  PasswordModule,
  CheckboxModule,
  DropdownModule,
  DataTableModule,
  PaginatorModule,
  SharedModule as primengSharedModule,
  ConfirmDialogModule,
  ConfirmationService,
  PanelModule,
  TreeTableModule,
  TreeModule,
  ColorPickerModule,
  DialogModule
} from 'primeng/primeng';

import { SharedModule } from '../../../shared/shared.module';
import { EasyUIModule } from 'ng-easyui/components/easyui/easyui.module';

import { ContouringComponent } from './contouring.component';
import { ContouringService } from '../shared/contouring.service';
import { PicAxialComponent } from './pic-axial/pic-axial.component';
import { ContouringTopBarComponent } from './contouring-top-bar/contouring-top-bar.component';
import { PatientDBService } from '../shared/patientDB.service';
import { PatientHttpService } from '../shared/patientHttp.service';
import { RoiHttpService } from '../shared/roiHttp.service';
import { SeriesHttpService } from '../shared/seriesHttp.service';
import { StorageService } from '../shared/storage.service';
import { actionService } from './pic-axial/action.service';


// const routes: Routes = [
//   {
//     path: '',
//     component: ContouringComponent,
//   }
// ];

@NgModule({
  imports: [
    SharedModule,
    ButtonModule,
    InputTextModule,
    PasswordModule,
    CheckboxModule,
    PaginatorModule,
    DropdownModule,
    ConfirmDialogModule,
    primengSharedModule,
    DataTableModule,
    DialogModule,
    DropdownModule,
    PanelModule,
    TreeTableModule,
    EasyUIModule,
    TreeModule,
    ColorPickerModule,
    //RouterModule.forChild(routes)
  ],
  declarations: [
    ContouringComponent,
    PicAxialComponent,
    ContouringTopBarComponent,
  ],
  exports: [
    ContouringComponent,
  ],
  providers: [
    ContouringService, 
    ConfirmationService,
    PatientDBService,
    PatientHttpService,
    RoiHttpService,
    SeriesHttpService,
    StorageService,
    actionService]
})
export class ContouringModule {
}
