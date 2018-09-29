import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { ButtonModule, InputTextModule } from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';
import { RenewPasswordComponent } from './renew-password.component';

const routes: Routes = [
  { path: 'renew-password', component: RenewPasswordComponent },
];

@NgModule({
  imports: [
    SharedModule,
    RouterModule.forChild(routes),
    ButtonModule,
  ],
  declarations: [
    RenewPasswordComponent,
  ],
  exports: [
    RenewPasswordComponent,
  ]
})
export class RenewPasswordModule {
}
