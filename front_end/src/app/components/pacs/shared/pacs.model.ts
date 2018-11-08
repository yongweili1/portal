import { PageRequest } from '../../../shared/models/pagination';
export interface PacsPageRequest extends PageRequest {
  patientId: string; 
  patientName: string;
  patientAge: string;
  gender: string;
  modality?: string;
//  studyDescription: string;
//  studyDate: Date[];
}

export interface EachPageRequest {
  page: number;
  size: number;
}

export interface PacsDownloadRequest{
  patientId: string;
}

export interface MenuItem {
    label?: string;
}


export interface PacsInfo {
    no?;
    templateId?;
    templateIndex?;
    templateName?;
    patientId: string; 
    patientName: string;
    patientAge: string;
    gender: string;
    modality?: string;
  }
  
  export interface PacsInfoRequest {
    templateName?;
    patientId: string; 
    patientName: string;
    patientAge: string;
    gender: string;
    modality?: string;
 //   studyDescription: string;
 //   studyDate: any[];
  }
 