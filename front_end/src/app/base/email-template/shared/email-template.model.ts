import { PageRequest } from '../../../shared/models/pagination';

export interface Email {
  id?: string;
  name?: string;
  subject?: string;
  content?: string;
  createTime?: string;
  updateTime?: string;
  enable?: string;
  no?: number;
}

export interface Param {
  id?: string;
  name?: string;
  describe?: string;
  purpose?: string;
  value?: string;
}

export interface EmailPageRequest extends PageRequest {
  name?: string;
}

