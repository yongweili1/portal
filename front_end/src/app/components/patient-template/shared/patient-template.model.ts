import { PageRequest } from '../../../shared/models/pagination';
export interface PatientPageRequest extends PageRequest {
  patientId: string; 
  patientName: string;
  gender: string;
  modality: string;
  studyDescription: string;
  studyDate: Date[];
}

export interface PatientPageDeleteRequest extends PageRequest {
    patientId: string; 
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
    modality: string;
    studyDescription: string;
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
 