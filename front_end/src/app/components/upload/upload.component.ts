import {
  Component,
  OnInit,
  ViewChild
} from '@angular/core';

import { FileUploader } from 'ng2-file-upload';
import {ToastService} from '../../core/toast.service';
import { UploadService } from '../../services/upload.service'
import { AppConfigService } from '../../app.config';
@Component({
  selector: 'mpt-upload',
  templateUrl: './upload.component.html',
  styleUrls: ['./upload.component.less'],
})
export class UploadComponent {

  uploader:FileUploader;
  hasAnotherDropZoneOver:boolean;
  response:string;
  
  constructor(
    private appConfig:AppConfigService,
    private messageService: ToastService,
    private UploadService: UploadService
  ) {
      this.uploader = new FileUploader({    
        url: `${this.appConfig.apiUrl}/dicom/patinfos/`,    
        method: "POST",    
        autoUpload: false,
        itemAlias: "a"
    });
    // this.uploader.queue[0]. 
    this.hasAnotherDropZoneOver = false;
    this.response = '';
    this.uploader.response.subscribe( (res) => 
    {
      this.response = res;
    });
  }
 
  public fileOverAnother(e:any):void {
    this.hasAnotherDropZoneOver = e;
  }

  public selectedFileOnChanged(event) {
    
  }
}
