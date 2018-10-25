import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { PacsComponent } from './pacs.component';
import { Routes, RouterModule } from '@angular/router';


const routes: Routes = [
  { path: '', component: PacsComponent },
];

@NgModule({
  imports: [
    CommonModule,
    RouterModule.forChild(routes),
  ],
  declarations: [PacsComponent]
})
export class PacsModule { }
