import { PageRequest } from '../../../shared/models/pagination';
export interface PatientPageRequest extends PageRequest {
    patientId: string;
    patientName: string;
    gender: string;
    modality: string;
    studyDate: Date[];
}

export interface PatientPageDeleteRequest extends PageRequest {
    patientId: string;
    studyId: string;
    seriesId: string;
}

export interface MenuItem {
    label?: string;
}


export interface PatientTemplateInfo {
    no?;
    templateId?;
    templateIndex?;
    templateName?;
    patientId: string;
    patientName: string;
    gender: string;
    // studyInfo?: StudyTemplateInfo[];
    seriesInfo?: SeriesTemplateInfo[];
}

export interface PatientTemplateInfoRequest {
    templateName?;
    patientId: string;
    patientName: string;
    gender: string;
    modality: string;
    studyDescription: string;
    studyDate: any[];
}

export interface StudyTemplateInfo {
    studyid: string;
    studyuid: string;
    studydescription: string;
    seriesInfo?:any[]
}

export interface SeriesTemplateInfo{
    seriesuid: string; 
    imagequantity: string;
    seriesdescription: string;
}
