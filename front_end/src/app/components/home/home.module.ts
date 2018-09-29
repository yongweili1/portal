import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {
  SharedModule as primengSharedModule,
  PanelModule,
  SelectButtonModule,
  DropdownModule,
} from 'primeng/primeng';
import { FileUploadModule } from 'ng2-file-upload';
import { SharedModule } from '../../shared/shared.module';

import { HomeComponent } from './home.component';
import { HomeService } from '../../services/home.service';
import { MessageService } from '../../shared/common/messageservice';


const primengModule = [primengSharedModule, PanelModule,
  SelectButtonModule, DropdownModule];

const routes: Routes = [
  { path: '', component: HomeComponent },
];

@NgModule({
  imports: [
    ...primengModule,
    SharedModule,
    FileUploadModule,
    RouterModule.forChild(routes)
  ],
  declarations: [
    HomeComponent,
  ],
  exports: [
    HomeComponent,
  ],
  providers: [HomeService,MessageService],
  entryComponents: [
  ]
})
export class HomeModule {
}
