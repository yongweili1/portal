import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {
  ButtonModule,
  InplaceModule,
  InputTextModule,
  InputTextareaModule
} from 'primeng/primeng';

import { SharedModule } from '../../shared/shared.module';
import { ConfigComponent } from './config.component';
import { ConfigService } from './config.service';

const routes: Routes = [
  { path: '', component: ConfigComponent },
];

@NgModule({
  imports: [
    SharedModule,
    ButtonModule,
    InplaceModule,
    InputTextModule,
    InputTextareaModule,
    RouterModule.forChild(routes)
  ],
  declarations: [
    ConfigComponent,
  ],
  exports: [
    ConfigComponent,
  ],
  providers: [ConfigService]
})
export class ConfigModule {
}
