import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {
  ButtonModule,
  PanelModule,
  DataTableModule,
  DialogModule,
  DropdownModule,
  PickListModule,
  PaginatorModule,
  ConfirmDialogModule,
  ConfirmationService,
  InputSwitchModule,
  // TableModule
} from 'primeng/primeng';

const primengModule = [
  ButtonModule,
  PanelModule,
  DataTableModule,
  DialogModule,
  DropdownModule,
  PickListModule,
  PaginatorModule,
  ConfirmDialogModule,
  InputSwitchModule
  // TableModule
];

import { SharedModule } from '../../shared/shared.module';
import { EmailTemplateComponent } from './email-template.component';
import { AddEmailTemplateComponent } from './add-email-template/add-email-template.component';
import { EditEmailTemplateComponent } from './edit-email-template/edit-email-template.component';
import { EmailTemplateService } from '../../services/email-template.service';



const routes: Routes = [
  {
    path: '', component: EmailTemplateComponent,
    children: [
      { path: 'add-email-template', component: AddEmailTemplateComponent },
      { path: 'edit-email-template/:id', component: EditEmailTemplateComponent },
    ]
  }
];

@NgModule({
  imports: [
    ...primengModule,
    SharedModule,
    RouterModule.forChild(routes)
  ],
  declarations: [
    EmailTemplateComponent,
    AddEmailTemplateComponent,
    EditEmailTemplateComponent
  ],
  exports: [
    EmailTemplateComponent
  ],
  providers: [
    EmailTemplateService,
    ConfirmationService
  ]
})
export class EmailTemplateModule {
}
