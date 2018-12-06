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
    MultiSelectModule,
} from 'primeng/primeng';


import { ToastModule } from 'primeng/toast';
import { SliderModule } from 'primeng/slider';
import { SharedModule } from '../../shared/shared.module';
import { EasyUIModule } from 'ng-easyui/components/easyui/easyui.module';

import { ContouringComponent } from './contouring.component';
import { ContourService } from './shared/service/contour.service';
import { CellComponent } from './cell/cell.component';
import { ContouringTopBarComponent } from './contouring-top-bar/contouring-top-bar.component';
import { RoiService } from './shared/service/roi.service';
import { ImageService } from './shared/service/image.service';
import { StorageService } from './shared/service/storage.service';
import { ToastService } from './shared/service/toast.service';
import { MacroService } from './shared/service/macro.service';
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
        MultiSelectModule,
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
        ContourService,
        ConfirmationService,
        RoiService,
        ImageService,
        StorageService,
        ToastService,
        MacroService,
    ]
})

export class ContouringModule {
}
