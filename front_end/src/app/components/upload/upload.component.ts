import { Component } from '@angular/core';
import { FileUploader } from 'ng2-file-upload';
import { MessageService } from 'primeng/api';
import { AppConfigService } from '../../app.config';
import { UploadService } from '../../services/upload.service';

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
        private priMessageService: MessageService
    ) {
        this.uploader = new FileUploader({
            url: `${this.appConfig.apiUrl}/dicom/upload/`,
            method: "POST",
            autoUpload: false,
            itemAlias: "a",
            queueLimit: 2000,
            // allowedFileType: ['dcm','DCM']
        });
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
        let repeatNum: number = 0;
        let typeErrorNum: number = 0;
        let typeReg = new RegExp('\.(dcm)$', 'i')
        this.uploader.queue.forEach(fileItems => {
            if (fileNames.includes(fileItems.file.name)) {
                repeatNum++;
            } else if (!typeReg.test(fileItems.file.name)) {
                typeErrorNum++;
            }
            fileNames.push(fileItems.file.name);
            files.push(fileItems._file);
        });
        if (repeatNum > 0) {
            const msg = `${repeatNum} files have the same name. The system has removed the ${repeatNum} files automatically.`;
            this.priMessageService.add({ severity: 'info', detail: msg, life: 5000 });
        }
        if (typeErrorNum > 0) {
            const msg = `${typeErrorNum} files type is wrong(DCM only). The system has removed the ${typeErrorNum} files automatically.`
            this.priMessageService.add({ severity: 'info', detail: msg, life: 5000 });
        }
        if (files.length > 0) {
            this.priMessageService.add({ severity: 'info', detail: 'Uploading, please wait.', life: 5000 });
            this.UploadService.makeFileRequest(`${this.appConfig.apiUrl}/dicom/upload/`, files).subscribe(result => {
                if (result == "success") {
                    const msg = 'Build volume finished, you can check the detail now.';
                    this.priMessageService.add({ severity: 'success', detail: msg, life: 5000 });
                }
            });
            this.UploadService.progress$.subscribe(result => {
                this.uploadPercent = result
                if (this.uploadPercent == 100) {
                    const msg = `Upload finished(${files.length} files totally), building volume now.`;
                    this.priMessageService.add({ severity: 'success', detail: msg, life: 5000 });
                }
            });
        }
    }
}
