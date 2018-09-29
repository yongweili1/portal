import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { MessagesModule, ButtonModule, PanelModule, InputTextModule } from 'primeng/primeng';
import { SharedModule } from '../../shared/shared.module';
import { LoginComponent } from './login.component';
import { VerifyCodeService } from '../../services/verify-code.service';

const routes: Routes = [
  { path: 'login', component: LoginComponent }
];

@NgModule({
  imports: [
    RouterModule,
    MessagesModule,
    ButtonModule,
    PanelModule,
    InputTextModule,
    SharedModule,
    RouterModule.forChild(routes)
  ],
  declarations: [
    LoginComponent
  ],
  exports: [
    LoginComponent
  ],
  providers: [
    VerifyCodeService
  ]
})
export class LoginModule {
}
