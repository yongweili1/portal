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
  InputSwitchModule
} from 'primeng/primeng';

import { SharedModule } from '../../shared/shared.module';
import { SmsTemplateComponent } from './sms-template.component';
import { AddSmsTemplateComponent } from './add-sms-template/add-sms-template.component';
import { EditSmsTemplateComponent } from './edit-sms-template/edit-sms-template.component';
import { SmsTemplateService } from '../../services/sms-template.service';

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
];

const routes: Routes = [
  {
    path: '', component: SmsTemplateComponent,
    children: [
      { path: 'add-sms-template', component: AddSmsTemplateComponent },
      { path: 'edit-sms-template/:id', component: EditSmsTemplateComponent },
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
    SmsTemplateComponent,
    AddSmsTemplateComponent,
    EditSmsTemplateComponent
  ],
  exports: [
    SmsTemplateComponent
  ],
  providers: [
    SmsTemplateService,
    ConfirmationService
  ]
})

export class SmsTemplateModule {
}
