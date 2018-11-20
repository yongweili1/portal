import { NgModule } from '@angular/core';
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
  SplitButtonModule,
} from 'primeng/primeng';


import { ToastModule } from 'primeng/toast';
import { SliderModule } from 'primeng/slider';
import { SharedModule } from '../../shared/shared.module';
import { EasyUIModule } from 'ng-easyui/components/easyui/easyui.module';

import { ContouringComponent } from './contouring.component';
import { ContouringService } from './shared/service/contouring.service';
import { CellComponent } from './cell/cell.component';
import { ContouringTopBarComponent } from './contouring-top-bar/contouring-top-bar.component';
import { ConMessageService } from './shared/service/conMessage.service';
import { PatientHttpService } from './shared/service/patientHttp.service';
import { RoiHttpService } from './shared/service/roiHttp.service';
import { SeriesHttpService } from './shared/service/seriesHttp.service';
import { StorageService } from './shared/service/storage.service';
import { ImageCanvasDirective } from './shared/directives/imageCanvas.directive';
import { OverlayCanvasDirective } from './shared/directives/overlayCanvas.directive';
import { ActionCanvasDirective } from './shared/directives/actionCanvas.directive';
import { CrossCanvasDirective } from './shared/directives/crossCanvas.directive';
import { SeriesListComponent } from './series-list/series-list.component';
import { SliderBarComponent } from './slider-bar/slider-bar.component';

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
    SplitButtonModule,
    SliderModule,
  ],
  declarations: [
    ContouringComponent,
    CellComponent,
    ContouringTopBarComponent,
    OverlayCanvasDirective,
    ActionCanvasDirective,
    ImageCanvasDirective,
    CrossCanvasDirective,
    SeriesListComponent,
    SliderBarComponent,
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
    StorageService]
})

export class ContouringModule {
}
