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
import { ElModule } from 'element-angular'

import { UploadComponent } from './upload.component';
import { UploadService } from '../../services/upload.service';
import { MessageService } from '../../shared/common/messageservice';


const primengModule = [primengSharedModule, PanelModule,
  SelectButtonModule, DropdownModule];

const routes: Routes = [
  { path: '', component: UploadComponent },
];

@NgModule({
  imports: [
    ...primengModule,
    SharedModule,
    FileUploadModule,
    RouterModule.forChild(routes),
    ElModule.forRoot(),
  ],
  declarations: [
    UploadComponent,
  ],
  exports: [
    UploadComponent,
  ],
  providers: [UploadService,MessageService],
  entryComponents: [
  ]
})
export class UploadModule {
}
