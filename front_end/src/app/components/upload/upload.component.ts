import {
    Component,
} from '@angular/core';

import { FileUploader } from 'ng2-file-upload';
import { UploadService } from '../../services/upload.service'
import { AppConfigService } from '../../app.config';
import { forEach } from '@angular/router/src/utils/collection';
import { ElMessageService } from 'element-angular'
import { ElNotificationService } from 'element-angular'
import { MessageService } from 'primeng/api';

import { config } from 'rxjs';

declare var $: any;


@Component({
    selector: 'mpt-upload',
    templateUrl: './upload.component.html',
    styleUrls: ['./upload.component.less'],
})
export class UploadComponent {

    uploader: FileUploader;
    hasAnotherDropZoneOver: boolean;
    response: string;
    uploadPercent: any = 0;
    hiddenPercent: any = 'false';

    constructor(
        private appConfig: AppConfigService,
        private UploadService: UploadService,
        private elMessage: ElMessageService,
        private elnotify: ElNotificationService,
        private priMessageService: MessageService
    ) {
        this.uploader = new FileUploader({
            url: `${this.appConfig.apiUrl}/dicom/patinfos/`,
            method: "POST",
            autoUpload: false,
            itemAlias: "a",
            queueLimit: 2000,
            // allowedFileType: ['dcm','DCM']
        });
        // this.uploader.queue[0]. 
        this.hasAnotherDropZoneOver = false;
        this.response = '';
        this.uploader.response.subscribe((res) => {
            this.response = res;
        });
    }

    public fileOverAnother(e: any): void {
        this.hasAnotherDropZoneOver = e;
    }

    public selectedFileOnChanged(event) {

    }
    public uploadAll() {
        let files: Array<File> = [];
        let fileNames: Array<string> = [];
        let repeatNum:number = 0;
        let typeErrorNum:number = 0;
        let typeReg = new RegExp('\.(dcm)$','i')
        this.uploader.queue.forEach(fileItems => {
            if(fileNames.includes(fileItems.file.name)){
                repeatNum++;
            }
            else if(!typeReg.test(fileItems.file.name)){
                typeErrorNum++;
            }
            // else{
            fileNames.push(fileItems.file.name);
            files.push(fileItems._file);
            // }
        })
        if(repeatNum > 0){
            this.priMessageService.add({severity:'info', detail:`${repeatNum} files have the same name. The system has removed the ${repeatNum} files automatically.`, life:5000});
        }
        if(typeErrorNum > 0){
            this.priMessageService.add({severity:'info', detail:`${typeErrorNum} files type is wrong(DCM only). The system has removed the ${typeErrorNum} files automatically.`, life:5000});
        }
        if(files.length > 0){
            this.priMessageService.add({severity:'info', detail:'Uploading, please wait.', life:5000});
            this.UploadService.makeFileRequest(`${this.appConfig.apiUrl}/dicom/patinfos/`, files).subscribe(result => {
                if(result == "success"){
                    this.priMessageService.add({severity:'success', detail:'Build volume finished, you can check the detail now.', life:5000});
                }
            });
            this.UploadService.progress$.subscribe(result => {
                this.uploadPercent = result
                if(this.uploadPercent == 100){
                    this.priMessageService.add({severity:'success', detail:`Upload finished(${files.length} files totally), building volume now.`, life:5000});
                }
            }
            )
        }
    }
}
