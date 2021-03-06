import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { TableModule } from 'primeng/table';
import {
  DataTableModule,
  ButtonModule,
  ConfirmationService,
  ConfirmDialogModule,
  PaginatorModule,
  DropdownModule,
  CalendarModule,
  PanelModule,
  TreeTableModule
} from 'primeng/primeng';
import { ToastModule } from 'primeng/toast';
import { AccordionModule } from 'primeng/accordion';
import { SharedModule } from '../../shared/shared.module';

import { PaListComponent } from './patient-template.component';
import { PatientTemplateService } from '../../services/patient-template.service'

const routes: Routes = [
  {
    path: '',
    component: PaListComponent
  }
];

@NgModule({
  imports: [
    FormsModule,
    SharedModule,
    TableModule,
    DataTableModule,
    ButtonModule,
    CommonModule,
    ConfirmDialogModule,
    PaginatorModule,
    DropdownModule,
    CalendarModule,
    PanelModule,
    RouterModule.forChild(routes),
    ToastModule,
    AccordionModule,
    TreeTableModule
  ],
  declarations: [
    PaListComponent,
  ],
  exports: [
    PaListComponent,
  ],
  providers: [PatientTemplateService, ConfirmationService]
})
export class PaListModule {
}
