import { Component, ViewEncapsulation } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import * as moment from 'moment';

@Component({
  selector: 'mpt-app',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.less'],
  encapsulation: ViewEncapsulation.None
})
export class AppComponent {

  constructor(translate: TranslateService) {
    const appConfig = window['appConfig'] || {};
    translate.setDefaultLang('zh_CN');
    translate.use(appConfig.locale || 'zh_CN');
    moment.locale(appConfig.locale || 'zh_CN');

  }

}
