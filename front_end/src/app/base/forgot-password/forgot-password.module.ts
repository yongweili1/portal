import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import {
  InputTextModule, DialogModule, PanelModule, ButtonModule
} from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';

import { ForgotPasswordComponent } from './forgot-password.component';
import { SentEmailComponent } from './sent-email/sent-email.component';
import { ResetComponent } from './reset/reset.component';
import { ChangedComponent } from './changed/changed.component';
import { ForgotPasswordService } from '../../services/forgot-password.service';

const primengModule = [
  InputTextModule, DialogModule, PanelModule, ButtonModule
];

const routes: Routes = [
  { path: 'forgot-password', component: ForgotPasswordComponent },
  { path: 'forgot-password/sent-email', component: SentEmailComponent },
  { path: 'forgot-password/reset/:id', component: ResetComponent },
  { path: 'forgot-password/changed', component: ChangedComponent }
];

@NgModule({
  imports: [
    ...primengModule,
    SharedModule,
    RouterModule.forChild(routes)
  ],
  declarations: [
    ForgotPasswordComponent,
    SentEmailComponent,
    ResetComponent,
    ChangedComponent
  ],
  exports: [
    ForgotPasswordComponent,
    SentEmailComponent,
    ResetComponent,
    ChangedComponent
  ],
  providers: [ForgotPasswordService]
})
export class ForgotPasswordModule {
}
