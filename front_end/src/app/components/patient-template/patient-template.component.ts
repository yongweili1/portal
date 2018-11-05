import { Component, OnInit } from '@angular/core';
import { MenuItem, PatientPageRequest, PatientTemplateInfo } from './shared/patient-template.model'
import { AppConfigService } from '../../app.config';
import { PatientTemplateService } from '../../services/patient-template.service';
import { Page, PageRequest } from '../../shared/models/pagination';
import { LazyLoadEvent } from 'primeng/primeng';
import { SelectItem } from 'primeng/api';
import { Router } from '@angular/router';
import { MessageService } from 'primeng/api';
import { ToastService } from '../../core/toast.service'

@Component({
    selector: 'mpt-palist',
    templateUrl: './patient-template.component.html',
    styleUrls: ['./patient-template.component.less'],
})
export class PaListComponent implements OnInit {
    patientId: string;
    public gender: MenuItem[] = [
        { label: 'Male' },
        { label: 'Female' },
        { label: 'Other' }
    ];
    public modality: MenuItem[] = [
        { label: 'MR' },
        { label: 'CT' },
        { label: 'CR' },
        { label: 'DX' }
    ];
    cols: any[] = [];
    studycols: any[] = [];
    seriescols: any[] = [];
    rangeDate = [];
    pageModel: Page<PatientTemplateInfo>;
    selectedPageModel: any;
    selectedStudyModel: any;
    selectedSeriesModel: any;
    tablePageRows: number[] = [10, 15, 20, 50, 100];
    patientParam: PatientPageRequest = {} as PatientPageRequest;

    constructor(
        private patientTemplateService: PatientTemplateService,
        private router: Router,
        private priMessageService: MessageService,
    ) {
        this.cols = [
            { field: 'patientId', header: 'PATIENT ID' },
            { field: 'patientName', header: 'PATIENT NAME' },
            { field: 'gender', header: 'GENDER' },
            { field: 'modality', header: 'MODALITY' },
            { field: 'studyDescription', header: 'STUDYDESCRIPTION'}
        ];

        this.studycols=[
            { field: 'studyid', header: 'STUDY ID' },
            { field: 'studyuid', header: 'STUDY UID' },
            { field: 'studydescription', header: 'STUDYDESCRIPTION'},
        ];

        this.seriescols=[
            { field: 'seriesuid', header: 'SERIES ID' },
            { field: 'seriesdescription', header: 'SERIESSCRIPTION'},
        ];
    }

    public ngOnInit(): void {
        this.loadPatientTemplate();
        this.selectedPageModel = {};
        this.selectedStudyModel = {};
        this.selectedSeriesModel = {};
    }

    public search() {
        this.loadPatientTemplate();
    }

    public loadPatientTemplate(event: LazyLoadEvent = { first: 0, rows: this.tablePageRows[1] }) {
        this.requestPatientTemplate(
            Math.floor(event.first / (event.rows || this.tablePageRows[1])),
            event.rows || this.tablePageRows[1]
        )
    }

    private requestPatientTemplate(page: number = 0, size: number = this.tablePageRows[1]) {
        this.setDateParams();
        this.patientTemplateService.getPatientTemplate({
            page,
            size,
            patientId: this.patientParam.patientId,
            patientName: this.patientParam.patientName,
            gender: this.patientParam.gender,
            modality: this.patientParam.modality,
            studyDescription: this.patientParam.studyDescription,
            studyDate: this.rangeDate
        }).subscribe((data) => {
            this.pageModel = data;
            for (let i = 0; i < this.pageModel.numberOfElements; i++) {
                this.pageModel.content[i].no = i + 1;
                this.patientId = this.pageModel.content[i].patientId;
            }
        });
        return;
    }

    private setDateParams() {
        if (this.patientParam !== null && this.patientParam.studyDate) {
            if (null !== this.patientParam.studyDate && undefined !== this.patientParam.studyDate && !this.patientParam.studyDate[1]) {
                this.rangeDate = [this.patientParam.studyDate[0].getTime(), this.patientParam.studyDate[0].getTime()];
            } else {
                this.rangeDate = [this.patientParam.studyDate[0].getTime(), this.patientParam.studyDate[1].getTime()];
            }
        }
    }

    onDelete(page: number = 0, size: number = this.tablePageRows[1]) {
        let patientIdArray = []
        let patientIdString = "";
        if(this.selectedPageModel.content != undefined){
            this.selectedPageModel.content.forEach(element => {
                patientIdArray.push(element.patientId);
            });
            patientIdString = patientIdArray.join(',');
        }
        let studyIdArray = []
        let studyIdString = "";
        if(this.selectedStudyModel.content != undefined){
            this.selectedStudyModel.content.forEach(element => {
                studyIdArray.push(element.studyid);
            });
            studyIdString = studyIdArray.join(',');
        }

        let seriesIdArray = []
        let seriesIdString = "";
        if(this.selectedSeriesModel.content != undefined){
            this.selectedSeriesModel.content.forEach(element => {
                seriesIdArray.push(element.seriesuid);
            });
            seriesIdString = seriesIdArray.join(',');
        }
        this.patientTemplateService.deletePatientTemplate({
            page,
            size,
            patientId: patientIdString,
            studyId: studyIdString,
            seriesId: seriesIdString
        }).subscribe(result => {
            if (result.code == '200') {
                this.pageModel = result.data;
                this.priMessageService.add({ severity: 'success', detail: 'del succeed.' });
            }
            else{
                console.log("=== del failed ===")
                this.priMessageService.add({ severity: 'error', detail: 'del failed.' });
            }
        },(error)=>{
            this.priMessageService.add({ severity: 'error', detail: 'del failed.' });
        })
    }

    // public  download(){
    //   this.setDateParams();
    //   this.patientTemplateService.downloadExcel({
    //     patientId:this.patientParam.patientId,
    //     patientName:this.patientParam.patientName,
    //     gender:this.patientParam.gender,
    //     modality:this.patientParam.modality,
    //     studyDescription:this.patientParam.studyDescription,
    //     studyDate:this.rangeDate
    //   });
    // }

    public toContouring(id: string) {
        this.router.navigate(['/base/contouring'], {
            queryParams: {
                patientId: id,
            }
        });

    }

}
