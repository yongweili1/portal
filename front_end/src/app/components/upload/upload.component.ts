import {
  Component,
  OnInit,
  ViewChild
} from '@angular/core';

import { FileUploader } from 'ng2-file-upload';
import {ToastService} from '../../core/toast.service';
import { UploadService } from '../../services/upload.service'
import { AppConfigService } from '../../app.config';
import { forEach } from '@angular/router/src/utils/collection';
import { ElMessageService } from 'element-angular'

declare var $: any;


@Component({
  selector: 'mpt-upload',
  templateUrl: './upload.component.html',
  styleUrls: ['./upload.component.less'],
})
export class UploadComponent {

  uploader:FileUploader;
  hasAnotherDropZoneOver:boolean;
  response:string;
  uploadPercent:any = 0;
  hiddenPercent:any = 'false';
  
  constructor(
    private appConfig:AppConfigService,
    private messageService: ToastService,
    private UploadService: UploadService,
    private elMessage: ElMessageService
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
  public uploadAll(){
    let files : Array<File> = [];
    this.uploader.queue.forEach(fileItems =>{
      files.push(fileItems._file);
    })
    $('#loading').showLoading();
    this.hiddenPercent = 'true'
    this.UploadService.makeFileRequest(`${this.appConfig.apiUrl}/dicom/patinfos/`,files).subscribe(result=>
    {
      $('#loading').hideLoading();
      this.hiddenPercent = 'false'
      this.elMessage.setOptions({ showClose: true,customClass:"elmessage" });
      this.elMessage['info'](result);
    });
    this.UploadService.progress$.subscribe(result=>
      this.uploadPercent = result
    )
  }
}
