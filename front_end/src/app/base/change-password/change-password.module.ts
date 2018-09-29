import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { ButtonModule, InputTextModule } from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';
import { ChangePasswordComponent } from './change-password.component';
const routes: Routes = [
  {
    path: '',
    component: ChangePasswordComponent
  },
];

@NgModule({
  imports: [
    SharedModule,
    RouterModule.forChild(routes),
    ButtonModule
  ],
  declarations: [
    ChangePasswordComponent,
  ],
  exports: [
    ChangePasswordComponent,
  ]
})
export class ChangePasswordModule {
}
