import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { PacsComponent } from './pacs.component';
import { Routes, RouterModule } from '@angular/router';


import { FormsModule } from '@angular/forms';
import { TableModule } from 'primeng/table';
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
    PanelModule
  ],
  declarations: [PacsComponent]
})
export class PacsModule { }
