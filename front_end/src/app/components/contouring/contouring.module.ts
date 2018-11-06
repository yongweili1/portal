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
  DialogModule,
  SplitButtonModule
} from 'primeng/primeng';

import { ToastModule } from 'primeng/toast';
import { SharedModule } from '../../shared/shared.module';
import { EasyUIModule } from 'ng-easyui/components/easyui/easyui.module';

import { ContouringComponent } from './contouring.component';
import { ContouringService } from './shared/service/contouring.service';
import { PicTransverseComponent } from './pic-transverse/pic-transverse.component';
import { ContouringTopBarComponent } from './contouring-top-bar/contouring-top-bar.component';
import { ConMessageService } from './shared/service/conMessage.service';
import { PatientHttpService } from './shared/service/patientHttp.service';
import { RoiHttpService } from './shared/service/roiHttp.service';
import { SeriesHttpService } from './shared/service/seriesHttp.service';
import { StorageService } from './shared/service/storage.service';
import { actionService } from './pic-transverse/action.service';
import { ContourDirective } from './shared/directives/contour.directive';
import { SeriesListComponent } from './series-list/series-list.component';


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
    PanelModule,
    TreeTableModule,
    EasyUIModule,
    TreeModule,
    ColorPickerModule,
    ToastModule,
    SplitButtonModule
    //RouterModule.forChild(routes)
  ],
  declarations: [
    ContouringComponent,
    PicTransverseComponent,
    ContouringTopBarComponent,
    ContourDirective,
    SeriesListComponent,
  ],
  exports: [
    ContouringComponent,
  ],
  providers: [
    ContouringService, 
    ConfirmationService,
    ConMessageService,
    PatientHttpService,
    RoiHttpService,
    SeriesHttpService,
    StorageService,
    actionService]
})
export class ContouringModule {
}
