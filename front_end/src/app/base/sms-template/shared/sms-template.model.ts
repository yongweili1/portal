import { PageRequest } from '../../../shared/models/pagination';

export interface Sms {
  id?: string;
  name?: string;
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

export interface SmsPageRequest extends PageRequest {
  name?: string;
}
