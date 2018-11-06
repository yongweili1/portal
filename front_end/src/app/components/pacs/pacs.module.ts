import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { PacsComponent } from './pacs.component';
import { Routes, RouterModule } from '@angular/router';


import { FormsModule } from '@angular/forms';
import { TableModule } from 'primeng/table';
import { AccordionModule } from 'primeng/accordion';
import {
  DataTableModule,
  ButtonModule,
  ConfirmDialogModule,
  PaginatorModule,
  DropdownModule,
  CalendarModule,
  PanelModule
} from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';
import { PacsService } from '../../services/pacs.service'
import { ToastModule } from 'primeng/toast';


const routes: Routes = [
  { path: '', component: PacsComponent },
];

@NgModule({
  imports: [
    CommonModule,
    RouterModule.forChild(routes),    
    FormsModule,
    SharedModule,
    TableModule,
    DataTableModule,
    ConfirmDialogModule,
    PaginatorModule,
    DropdownModule,
    CalendarModule,
    PanelModule,
    AccordionModule,
    ToastModule
  ],
  declarations: [PacsComponent],
  providers:[PacsService]
})
export class PacsModule { }
