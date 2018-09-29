import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';

import { AppConfigService } from '../../../app.config';
import { AuthHttp } from '../../../core/auth-http';
import { Page, PageRequest, SelectableData } from '../../../shared/models';
import { TreeNode } from 'primeng/primeng';


@Injectable()
export class ContouringService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService
  ) { }

}
